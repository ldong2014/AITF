#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "../logger.hpp"
#include "Aitf_Manager.hpp"
#include "Udp_Server.hpp"
#include "../Hasher.hpp"

#define DST_REQ_INDEX 78

uint32_t MY_IP = 101;

Aitf_Manager::Aitf_Manager(){

}

void Aitf_Manager::run(){
	server = new Udp_Server(this);
	server->start();
}

void Aitf_Manager::timeout_run(){
	timeout_work.reset(new boost::asio::io_service::work(timeout_io));
	timeout_io.run();
}

void Aitf_Manager::start_thread(){
	log(logINFO) << "starting Aitf_Manager";
	aitf_thread = boost::thread(&Aitf_Manager::run, this);
	timeout_thread = boost::thread(&Aitf_Manager::timeout_run, this);

}

void Aitf_Manager::stop_thread(){
	log(logINFO) << "Stopping aitf manager";


	server->stop();

	//stop timeout thread
	log(logDEBUG2) << "stopping timeout thread";
	timeout_work.reset();
	timeout_io.stop();
	timeout_thread.interrupt();
	timeout_thread.join();
	log(logDEBUG2) << "timeout thread stopped";

	log(logDEBUG2) << "stopping aitf thread";
	aitf_thread.interrupt();
	aitf_thread.join();
	log(logDEBUG2) << "aitf thread stopped";



	log(logDEBUG2) << "deleting server";
	delete(server);
	log(logDEBUG2) << "server deltetd";


	log(logINFO) << "Aitf manager stopped";
}


void Aitf_Manager::packet_arrived(std::vector<uint8_t> recv_buf){
	log(logDEBUG2) << "Checking message type";

	//extract the type of message
	uint8_t msg_type = recv_buf[0];
	log(logDEBUG) << "Message type: " << (int) msg_type; 

	switch(msg_type){
		case 0: //Filter request
			handle_filter_request(recv_buf);
			break;
		case 1: //Handshake request
			handle_handshake_request(recv_buf);
			break;
		case 2: //Handshake Part 2
			//We should never receive one of these
			break;
		case 3: //Handshake part 3
			handle_handshake_finish(recv_buf);
			break;
		case 4: //Attack filter request
			//We should never receive one of these
			break;
		case 5: //Filter request reply
			handle_filter_reply(recv_buf);
			break;
		default:
			log(logWARNING) << "Invalid message type";
			break;
	}
}

void Aitf_Manager::handle_filter_reply(std::vector<uint8_t> message){
	if(message.size() == 9){
		Flow flow;
		memcpy(&flow.src_ip, &message[1], 4);
		memcpy(&flow.dst_ip, &message[5], 4);
		flow.gtw0_ip = MY_IP;
		flow.gtw0_rvalue = Hasher::hash(*gateway_key, (unsigned char*) &flow.dst_ip, 4);

		//if there was a filter request sent for this flow
		if(filter_table->attempt_count(flow) > 0){
			//add to the shadow table
			shadow_table->add_long_filter(flow);
		}
	}
}

void Aitf_Manager::handle_handshake_finish(std::vector<uint8_t> message){
	log(logDEBUG2) << "Received handshake finish";

	if(message.size() == 90){
		Flow flow(std::vector<uint8_t>(&message[1], &message[1] + 81));
		//check that the nonce is correct
		uint64_t nonce;
		memcpy(&nonce, &message[82], 8);
		uint64_t actual = Hasher::hash(*gateway_key, &flow.to_byte_vector()[0], 81);

		//if the nonce is correct then deal with the attacker
		if(nonce == actual){
			filter_table->add_temp_filter(flow);

			log(logDEBUG2) << "checking shadow table";
			//check shadow table
			int request_attempts = shadow_table->attempt_count(flow);

			deal_with_attacker(flow, request_attempts);
		}
	}
}

void Aitf_Manager::handle_handshake_request(std::vector<uint8_t> message){
	log(logDEBUG2) << "Received handshake request";

	//check the message length
	if(message.size() == 94){


		//pullout the requester gateway IP
		uint32_t gtw_ip;
		memcpy(&gtw_ip, &message[1], 4);

		//if the gateway is within its rate limit
		if(aitf_hosts_table->check_from_rate(gtw_ip)){

			Flow flow(std::vector<uint8_t>(&message[5], &message[5]+81));


			uint64_t r_value = flow.get_gtw_rvalue_at(flow.pointer);
			uint32_t atk_gtw_ip = flow.get_gtw_ip_at(flow.pointer);

			log(logDEBUG) << "dst: " << flow.dst_ip << " atk_gtw_ip: " << atk_gtw_ip << " r_value: " << r_value;

			//check that the random value in the flow is correct
			//compute the hash
			uint64_t actual = Hasher::hash(*gateway_key, (unsigned char*) &flow.dst_ip, 4);
			log(logDEBUG2) << "actual: " << actual << " r_value: " << r_value;


			//construct the reply
			std::vector<uint8_t> reply(90);
			reply[0] = 2;
			if(r_value == actual){
				//construct the reply with a new nonce
				log(logDEBUG) << "CORRECT HASH";
				uint64_t nonce = Hasher::hash(*gateway_key, (unsigned char*) &flow.to_byte_vector()[0], 81);
				memcpy(&reply[1], &flow.to_byte_vector()[0], 81);
				memcpy(&reply[82], &nonce, 8);
			}
			else{
				//send back the reply with a correct r_value and same nonce
				log(logDEBUG) << "INCORRECT HASH";
				Flow modified_flow(flow);
				modified_flow.set_gtw_rvalue_at(modified_flow.pointer, actual);

				//copy in the flow to to reply
				memcpy(&reply[1], &modified_flow.to_byte_vector()[0], 81);
				//copy in the nonce from the original message
				memcpy(&reply[82], &message[86], 8);
			}

			//TODO: send the reply to the destination

		}
	}
}

void Aitf_Manager::handle_filter_request(std::vector<uint8_t> message){
	log(logDEBUG2) << "Received filter request";

	//check the message length
	if(message.size() == 83){

		//pull out the dst_ip
		uint32_t dst_ip;
		memcpy(&dst_ip, &message[DST_REQ_INDEX], 4);

		//if the victim is within its rate limit
		if(aitf_hosts_table->check_from_rate(dst_ip)){

			Flow flow(std::vector<uint8_t>(&message[1], &message[1]+81));
			/*for(int i = 0; i < 83; i++){
			  log(logERROR) << (int) message[i];
			  }*/
			//apply temp filter
			filter_table->add_temp_filter(flow);

			log(logDEBUG2) << "checking shadow table";
			//check shadow table
			int request_attempts = shadow_table->attempt_count(flow);

			//if there is only one gateway in the list, then this gateway
			//must deal with it
			if(flow.pointer == 0){
				deal_with_attacker(flow, request_attempts);
			}
			else{
				log(logDEBUG2) << "Dealing with another gateway";
				//if this is a repeat offense for the gateway (twice)
				if(request_attempts > 2){
					attempt_escalation(flow, request_attempts - 1);
				}
				else{
					
					//create a new flow with only the atk gtw
					Flow ammended_flow();
					ammended_flow.src_ip = flow.src_ip;
					ammended_flow.gtw0_ip = flow.gtw0_ip;
					ammended_flow.gtw0_rvalue = flow.gtw0_rvalue;
					ammended_flow.dst_ip = flow.dst_ip;
					ammended_flow.pointer = 0;

					std::vector<uint8_t> handshake = create_handshake(ammended_flow);

					//Add filter for returning handshake with src as gtw0_ip
					Flow handshake_flow(flow);
					handshake_flow.src_ip = handshake_flow.gtw0_ip;
					filter_table->add_temp_filter(handshake_flow);
					//ammended_flow.gtw0_ip
					//TODO: Send the handshake

					//add callback to detect timedout handshakes
					uint8_t escalation = message[message.size()-1];
					boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(timeout_io, boost::posix_time::seconds(TEMP_TIME)));
					timer->async_wait(boost::bind(&Aitf_Manager::unresponsive_gateway, this, boost::asio::placeholders::error, timer, flow, escalation, 0));

				}
			}//else flow pointer != 0
		}//if rate ok
	}//if size ok
	else{
		log(logWARNING) << "Message size is incorrect!!: " << message.size();
	}
}

void Aitf_Manager::deal_with_attacker(Flow flow, int request_attempts){
	log(logDEBUG2) << "dealing with attacker";

	//if the host is AITF enabled
	if(aitf_hosts_table->contains_host(flow.src_ip)){
		//if this is a repeat offense
		if(request_attempts  > 0){
			filter_table->add_long_filter(flow);
		}
		else{
			std::vector<uint8_t> message(flow.to_byte_vector());
			message.insert(message.begin(), 4);
			//contact host
			//TODO CONTACT HOST!!!
			//send udp packet
			boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(timeout_io, boost::posix_time::seconds(TEMP_TIME)));
			timer->async_wait(boost::bind(&Aitf_Manager::unresponsive_host, this, boost::asio::placeholders::error, timer, flow));
		}
	}
	else{
		//For non AITF enabled hosts, just cut them off
		filter_table->add_long_filter(flow);

	}
}

std::vector<uint8_t> Aitf_Manager::create_handshake(Flow flow){
	log(logDEBUG2) << "Creating handshake";
	//generate the nonce
	uint64_t nonce = Hasher::hash(*gateway_key, (unsigned char*) &flow.to_byte_vector()[0], 81);
	//construct the handshake request wiht the flow
	//and prepend the source IP and msg_type and the nonce at the end
	std::vector<uint8_t> handshake(94);
	handshake[0] = 1;
	memcpy(&handshake[1], &MY_IP, 4);
	memcpy(&handshake[5], &flow.to_byte_vector()[0], 81);
	memcpy(&handshake[86], &nonce, 8);
	log(logDEBUG2) << "Finished creating handshake";

	return handshake;
}

void Aitf_Manager::unresponsive_host(const boost::system::error_code& e, boost::shared_ptr<boost::asio::deadline_timer> timer, Flow flow){
	log(logDEBUG2) << "_________________In unresponsive_host callback_________________";

	//if the flow has not been added to the table then cut off the client
	if(shadow_table->attempt_count(flow) == 0){
		filter_table->add_long_filter(flow);
	}

}

void Aitf_Manager::unresponsive_gateway(const boost::system::error_code& e, boost::shared_ptr<boost::asio::deadline_timer> timer, Flow flow, uint8_t escalation, int attempts){
	log(logDEBUG2) << "_________________In unresponsive_gateway callback_________________";

	//if the flow has not been added to the table then check for escalation
	if(shadow_table->attempt_count(flow) == 0){
		if(escalation == 1){
			//if there is escalation attempt to escalate to the next gateway
			attempt_escalation(flow, attempts);

		}
		else{
			//if no escalation block locally
			filter_table->add_long_filter(flow);
		}
	}
	else{
		//increment the shadowtable filter count to show the number of attempts made
		for(int i = 0; i < attempts; i++){
			shadow_table->add_long_filter(flow);
		}
	}

}

void Aitf_Manager::attempt_escalation(Flow flow, int attempts){
	//if there is an available gateway in the flow
	if(attempts < flow.pointer){
		std::vector<uint8_t> handshake = create_handshake(flow);

		//TODO: remove extra gateways
		uint32_t dst_gtw_ip = flow.get_gtw_ip_at(attempts);

		//add filter for response				
		Flow handshake_flow(flow);
		handshake_flow.src_ip = dst_gtw_ip;
		filter_table->add_temp_filter(handshake_flow);
		//TODO SEND HANDSHAKE
		//add callback for timedout connections
		boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(timeout_io, boost::posix_time::seconds(TEMP_TIME)));
		timer->async_wait(boost::bind(&Aitf_Manager::unresponsive_gateway, this, boost::asio::placeholders::error, timer, flow, 1, attempts+1));
	}
	else{
		//no other option but to block locally
		filter_table->add_long_filter(flow);
	}
}
