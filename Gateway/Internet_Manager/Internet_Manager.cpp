#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <string>
#include "Internet_Manager.hpp"
#include "../Hasher.hpp"
#include "../logger.hpp"

Internet_Manager::Internet_Manager(){
	std::string command = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -A OUTPUT -j NFQUEUE --queue-num 1";
	log(logINFO) << command;
	system( command.c_str() );
	std::string command2 = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -A INPUT -j NFQUEUE --queue-num 1";
	log(logINFO) << command2;
	system( command2.c_str() );

	std::string command3 = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -A FORWARD -j NFQUEUE --queue-num 1";
	log(logINFO) << command3;
	system( command3.c_str() );

}

Internet_Manager::~Internet_Manager(){
	std::string command = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -D OUTPUT -j NFQUEUE --queue-num 1";
	log(logINFO) << command;
	system( command.c_str() );

	std::string command2 = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -D INPUT -j NFQUEUE --queue-num 1";
	log(logINFO) << command2;
	system( command2.c_str() );

	std::string command3 = "iptables -s 10.4.13.0/24 -d 10.4.13.0/24 -D FORWARD -j NFQUEUE --queue-num 1";
	log(logINFO) << command3;
	system( command3.c_str() );
}

void Internet_Manager::start_thread(){
	log(logINFO) << "starting Internet_Manager";
	internet_thread = boost::thread(&Internet_Manager::run, this);
	//timeout_thread = boost::thread(&Internet_Manager::timeout_run, this);
}

void Internet_Manager::stop_thread(){
	packet_manager->stop();
	internet_thread.interrupt();
	internet_thread.join();
	delete(packet_manager);

}

void Internet_Manager::run(){
	packet_manager = new Packet_Manager(this);
	packet_manager->start();
}


bool Internet_Manager::is_allowed(Flow flow, std::vector<uint8_t> payload){
	bool is_allowed = true;
	//Check if a filter exists for the packet
	if(filter_table->flow_is_filtered(flow)){
		is_allowed = false;

		//if this message is from a gateway then it is part of a handshake
		if(flow.src_ip == flow.gtw0_ip){
			handle_handshake(payload);
		}
	}

	return is_allowed;
}

void Internet_Manager::handle_handshake(std::vector<uint8_t> message){
	//check that the message is the right size
	if(message.size() == 91){
		//check that the msg type is correct
		if(message[0] == 2){
			//grab the nonce and flow
			Flow flow(std::vector<uint8_t>(&message[2], &message[2]+81));
			uint64_t nonce;
			memcpy(&nonce, &message[83], 8);

			//compute the expected nonce
			uint64_t actual = Hasher::hash(*gateway_key, &flow.to_byte_vector()[0], 81);

			//grab the atk_gtw ip address
			uint32_t gtw_ip = flow.get_gtw_ip_at(message[1]);

			//add to shadow table
			shadow_table->add_long_filter(flow);

			//if the nonce is the one that this gateway sent
			if(nonce == actual){
				//this means that the rvalue for the gateway was wrong
				//add a filter so that only the correct value is accepted

				//grab the correct rvalue
				uint64_t rvalue = flow.get_gtw_rvalue_at(message[1]);

				filter_table->add_gtw_rvalue(flow.dst_ip, gtw_ip, rvalue);

			}
			else{
				//the rvalue is correct, just reflect the message back
				//TODO: send message to gtw_ip
			}

		}
	}

}
