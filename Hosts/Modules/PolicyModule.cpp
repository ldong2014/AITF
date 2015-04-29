/*
 * PolicyModule.cpp
 */

#include "PolicyModule.hpp"


PolicyModule::PolicyModule(){

	bandwidthUsage = new Async_Auto_Table( "../hostBandwidth.log", 1000);
	//bandwidthUsage = new Async_Auto_Table();

	//filterRequests = new Async_Auto_Table( "../filterRequests.log", 1000);
	filterRequests = new Async_Auto_Table();

	defaults[strToIP("10.4.13.128")] = 1000;
	defaults[strToIP("10.4.13.129")] = 1000;
	defaults[strToIP("10.4.13.130")] = 1000;
	defaults[strToIP("10.4.13.131")] = 1000;
	defaults[strToIP("10.4.13.132")] = 1000;
	defaults[strToIP("10.4.13.133")] = 1000;
	defaults[strToIP("10.4.13.134")] = 1000;
	defaults[strToIP("10.4.13.135")] = 1000;
	defaults[strToIP("10.4.13.136")] = 1000;
	defaults[strToIP("10.4.13.137")] = 1000;
	defaults[strToIP("10.4.13.138")] = 1000;
	defaults[strToIP("10.4.13.139")] = 1000;
	defaults[strToIP("10.4.13.140")] = 1000;
	defaults[strToIP("10.4.13.141")] = 1000;
	defaults[strToIP("10.4.13.142")] = 1000;
	defaults[strToIP("10.4.13.143")] = 1000;
	defaults[strToIP("10.4.13.144")] = 1000;
	defaults[strToIP("10.4.13.145")] = 1000;
	defaults[strToIP("10.4.13.146")] = 1000;
	defaults[strToIP("10.4.13.147")] = 1000;
	defaults[strToIP("10.4.13.148")] = 1000;
	defaults[strToIP("10.4.13.149")] = 1000;
	defaults[strToIP("10.4.13.150")] = 1000;
	defaults[strToIP("10.4.13.151")] = 1000;
	defaults[strToIP("10.4.13.152")] = 1000;
	defaults[strToIP("10.4.13.153")] = 1000;
	defaults[strToIP("10.4.13.154")] = 1000;
	defaults[strToIP("10.4.13.155")] = 1000;
	defaults[strToIP("10.4.13.156")] = 1000;
	defaults[strToIP("10.4.13.157")] = 1000;
	defaults[strToIP("10.4.13.158")] = 1000;
	defaults[strToIP("10.4.13.159")] = 1000;
	defaults[strToIP("10.4.13.160")] = 1000;
	defaults[strToIP("10.4.13.161")] = 1000;
	defaults[strToIP("10.4.13.162")] = 1000;
	defaults[strToIP("10.4.13.163")] = 1000;
	defaults[strToIP("10.4.13.164")] = 1000;
	defaults[strToIP("10.4.13.165")] = 1000;
	defaults[strToIP("10.4.13.166")] = 1000;
	defaults[strToIP("10.4.13.167")] = 1000;
	defaults[strToIP("10.4.13.168")] = 1000;
	defaults[strToIP("10.4.13.169")] = 1000;
	defaults[strToIP("10.4.13.170")] = 1000;
	defaults[strToIP("10.4.13.171")] = 1000;
	defaults[strToIP("10.4.13.172")] = 1000;
	defaults[strToIP("10.4.13.173")] = 1000;
	defaults[strToIP("10.4.13.174")] = 1000;
	defaults[strToIP("10.4.13.175")] = 1000;
	defaults[strToIP("10.4.13.176")] = 1000;
	defaults[strToIP("10.4.13.177")] = 1000;
	defaults[strToIP("10.4.13.178")] = 1000;
	defaults[strToIP("10.4.13.179")] = 1000;
	defaults[strToIP("10.4.13.180")] = 1000;
	defaults[strToIP("10.4.13.181")] = 1000;
	defaults[strToIP("10.4.13.182")] = 1000;
	defaults[strToIP("10.4.13.183")] = 1000;
	defaults[strToIP("10.4.13.184")] = 1000;
	defaults[strToIP("10.4.13.185")] = 1000;
	defaults[strToIP("10.4.13.186")] = 1000;
	defaults[strToIP("10.4.13.187")] = 1000;
	defaults[strToIP("10.4.13.188")] = 1000;
	defaults[strToIP("10.4.13.189")] = 1000;
	defaults[strToIP("10.4.13.190")] = 1000;
	defaults[strToIP("10.4.13.191")] = 1000;
	defaults[strToIP("10.4.13.192")] = 1000;
	defaults[strToIP("10.4.13.193")] = 1000;
	defaults[strToIP("10.4.13.194")] = 1000;
	defaults[strToIP("10.4.13.195")] = 1000;
	defaults[strToIP("10.4.13.196")] = 1000;
	defaults[strToIP("10.4.13.197")] = 1000;
	defaults[strToIP("10.4.13.198")] = 1000;
	defaults[strToIP("10.4.13.199")] = 1000;
	defaults[strToIP("10.4.13.200")] = 1000;
	defaults[strToIP("10.4.13.201")] = 1000;
	defaults[strToIP("10.4.13.202")] = 1000;
	defaults[strToIP("10.4.13.203")] = 1000;
	defaults[strToIP("10.4.13.204")] = 1000;
	defaults[strToIP("10.4.13.205")] = 1000;
	defaults[strToIP("10.4.13.206")] = 1000;
	defaults[strToIP("10.4.13.207")] = 1000;
	defaults[strToIP("10.4.13.208")] = 1000;
	defaults[strToIP("10.4.13.209")] = 1000;
	defaults[strToIP("10.4.13.210")] = 1000;
	defaults[strToIP("10.4.13.211")] = 1000;
	defaults[strToIP("10.4.13.212")] = 1000;
	defaults[strToIP("10.4.13.213")] = 1000;
	defaults[strToIP("10.4.13.214")] = 1000;
	defaults[strToIP("10.4.13.215")] = 1000;
	defaults[strToIP("10.4.13.216")] = 1000;
	defaults[strToIP("10.4.13.217")] = 1000;
	defaults[strToIP("10.4.13.218")] = 1000;
	defaults[strToIP("10.4.13.219")] = 1000;
	defaults[strToIP("10.4.13.220")] = 1000;
	defaults[strToIP("10.4.13.221")] = 1000;
	defaults[strToIP("10.4.13.222")] = 1000;
	defaults[strToIP("10.4.13.223")] = 1000;
	defaults[strToIP("10.4.13.224")] = 1000;
	defaults[strToIP("10.4.13.225")] = 1000;
	defaults[strToIP("10.4.13.226")] = 1000;
	defaults[strToIP("10.4.13.227")] = 1000;
	defaults[strToIP("10.4.13.228")] = 1000;
	defaults[strToIP("10.4.13.229")] = 1000;
	defaults[strToIP("10.4.13.230")] = 1000;
	defaults[strToIP("10.4.13.231")] = 1000;
	defaults[strToIP("10.4.13.232")] = 1000;
	defaults[strToIP("10.4.13.233")] = 1000;
	defaults[strToIP("10.4.13.234")] = 1000;
	defaults[strToIP("10.4.13.235")] = 1000;
	defaults[strToIP("10.4.13.236")] = 1000;
	defaults[strToIP("10.4.13.237")] = 1000;
	defaults[strToIP("10.4.13.238")] = 1000;
	defaults[strToIP("10.4.13.239")] = 1000;
	defaults[strToIP("10.4.13.240")] = 1000;
	defaults[strToIP("10.4.13.241")] = 1000;
	defaults[strToIP("10.4.13.242")] = 1000;
	defaults[strToIP("10.4.13.243")] = 1000;
	defaults[strToIP("10.4.13.244")] = 1000;
	defaults[strToIP("10.4.13.245")] = 1000;
	defaults[strToIP("10.4.13.246")] = 1000;
	defaults[strToIP("10.4.13.247")] = 1000;
	defaults[strToIP("10.4.13.248")] = 1000;
	defaults[strToIP("10.4.13.249")] = 1000;
	defaults[strToIP("10.4.13.250")] = 1000;
	defaults[strToIP("10.4.13.251")] = 1000;
	defaults[strToIP("10.4.13.252")] = 1000;
	defaults[strToIP("10.4.13.253")] = 1000;
	defaults[strToIP("10.4.13.254")] = 1000;
	defaults[strToIP("10.4.13.255")] = 1000;
	defaults[strToIP("10.4.13.256")] = 1000;

}


PolicyModule::~PolicyModule() {
	delete bandwidthUsage();
	delete filterRequests();
}
/*
 * receivedPacket: determines what should happen based on the source ip and the size of the packet
 * 	input: 	source_ip - the ip that is going to be cataloged
 * 			size - the size of the packet that was received
 * 	output: size - everything works out and nothing should be done
 * 			-1 - the source exceeded it's limit for the first time
 * 			-2 - the source exceeded it's limit for within t_long time meaning some form of escelation should take place
 */
int PolicyModule::receivedPacket(uint32_t source_ip, int size){
	int max = 0, bwu_ret;
	if (defaults.count(source_ip) > 0) {
		max = defaults[source_ip];
	} else {
		max = -1;
	}

	bwu_ret = bandwidthUsage->addValue(source_ip, size, max, 1000);

	if (bwu_ret < 0) {
		if(filterRequests->getValue(source_ip) > 0) {
			bwu_ret = -2;
		}

		filterRequests->addValue(source_ip, 1, -1, 10*60*1000);//TLONG);
	}

	return bwu_ret;
}

static uint32_t PolicyModule::strToIP(const char * str_ip){
	uint32_t part1, part2, part3, part4, num_ip;
	part1 = part2 = part3 = part4 = num_ip = 0;
	sscanf(str_ip, "%d.%d.%d.%d", &part1, &part2, &part3, &part4);
	num_ip += part1;
	num_ip << 8;
	num_ip += part2;
	num_ip << 8;
	num_ip += part3;
	num_ip << 8;
	num_ip += part4;
	num_ip << 8;

	return num_ip;
}
