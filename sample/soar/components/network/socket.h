/*
 * socket.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_NETWORK_SOCKET_H_
#define SOAR_COMPONENTS_NETWORK_SOCKET_H_


#include <stdint.h>

#include <netinet/in.h>

#include "../errctl/exception.h"
#include "../../communal.h"

namespace soar_components_network {

enum SocketStatus {
	SOCK_INIT = 0,
	SOCK_CREATE,
	SOCK_BIND,
	SOCK_LISTENING,
	SOCK_CONN,
	SOCK_CLOSE,
};

class SoarSocket {
	private:
		struct sockaddr_in sock_addr_;
		int sockfd_;
		enum SocketStatus volatile status_;
		int _CreateSocket(const char* addr , uint16_t port) throw(soar_components_errctl::SoarException);
	private:
		DISALLOW_COPY_AND_ASSIGN(SoarSocket);
	public:
		SoarSocket():sockfd_(0),status_(SOCK_INIT){}
		explicit SoarSocket(const char* addr , uint16_t port);
		explicit SoarSocket(int socket_fd):sockfd_(socket_fd),status_(SOCK_CREATE){}

		inline void set_fd(int socket_fd) {
			sockfd_ = socket_fd;
			status_ = SOCK_CREATE;
		}
		inline int get_fd(){return sockfd_;}
		int ListenAndBind(uint16_t port) throw(soar_components_errctl::SoarException);

		int SetNonBlocking();
		int SetBlocking();

		void SetKeepAlive(int idleTime , int intvlCheckTime , int checkCnt) throw(soar_components_errctl::SoarException);

		int Connect(const char* addr , uint16_t port) throw(soar_components_errctl::SoarException);
		int Connect() throw(soar_components_errctl::SoarException) ;

		int Send(char* src , int nlen) const throw(soar_components_errctl::SoarException);
		int Recv(char* buffer , int max_buffer_len) const throw(soar_components_errctl::SoarException);
		int Recv_n(char* buffer , int required_nbytes) const throw(soar_components_errctl::SoarException);

		void Close() throw();
		~SoarSocket();
	};

}/*namespace soar_components_network*/

#endif /* SOAR_COMPONENTS_NETWORK_SOCKET_H_ */
