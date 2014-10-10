/*
 * main.cc
 *
 *  Created on: Apr 7, 2013
 *      Author: House_Lee
 */
#include <cstdio>
#include <cstring>

#include "soar/soarapp.h"
#include "soar/communal.h"
#include "soar/components/system/memallocator.h"
#include "soar/components/scheduling/distributor.h"

namespace soar_components_scheduling {
class DefaultDistributor: public Distributor_Interface {
public:
	SoarDataBlock RecvMessage(const soar_components_network::SoarSocket& socket);
	PipeLineIndex GetMsgDestination();
	~DefaultDistributor(){}
};
SoarDataBlock DefaultDistributor::RecvMessage(const soar_components_network::SoarSocket& socket) {
	soar_communal::SoarString recv_buffer("");
	char buffer[512];
	int n_recv;
	while((n_recv = socket.Recv(buffer , 512))) {
		buffer[n_recv] = '\0';
		recv_buffer += buffer;
	}
	int len = recv_buffer.length();
	char* request_str = (char*)g_mempool.alloc(len);
	memcpy(request_str , recv_buffer.c_str() , len);
	SoarDataBlock rtn;
	rtn.length = len;
	rtn.data = (void*)request_str;
	return rtn;
}

PipeLineIndex DefaultDistributor::GetMsgDestination() {
	return 0;
}
}

SoarDataBlock print_request( SoarDataBlock src) {
	for(int i = 0; i != src.length; ++i) {
		printf("%c" , *((char*)src.data + i));
	}
	return src;
}

SoarDataBlock process_request(SoarDataBlock src) {
	char* request = (char*)src.data;
	soar_communal::SoarString URL("/www/");
	if (request[0] == 'G' && request[1] == 'E' && request[2] == 'T') {
		int i = 4;
		while(request[i] != ' ')
			URL += request[i];
		printf("URL:%s\n" , URL.c_str());
	}
	return src;
}

int main() {
try {
	soar_components_scheduling::PipeLine pipeline;
	soar_components_scheduling::PipeNode pipenode;
	pipenode.setProcessFunc(print_request);
	pipenode.setNWorkers(5);
	pipeline.AddNode(pipenode);

	pipenode.setProcessFunc(process_request);
	pipenode.setNWorkers(10);
	pipeline.AddNode(pipenode);

	SoarApp< soar_components_scheduling::DefaultDistributor > services;
	services.SetPort(18080);
	services.RegisterPipeLine(pipeline);
	services.Start();
} catch (const soar_components_errctl::SoarException& exp) {
	printf("err_code:%d\n" , exp.get_error_id());
}
}



