/*
 * socket.cc
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "socket.h"




using namespace soar_components_network;

int SoarSocket::_CreateSocket(const char* addr , uint16_t port) throw(soar_components_errctl::SoarException)  {
	if (!addr || !port)
		return 0;
	sockfd_ = socket(AF_INET , SOCK_STREAM , 0);
	if (sockfd_ < 0)
		throw soar_components_errctl::SoarException(soar_exception::SOCKET_CREATE_FAILED);
	sock_addr_.sin_family = AF_INET;
	inet_pton(AF_INET , addr , &sock_addr_.sin_addr);
	sock_addr_.sin_port = htons(port);
	status_ = SOCK_CREATE;
	return sockfd_;
}

SoarSocket::SoarSocket(const char* addr , uint16_t port) {
	try {
		sockfd_ = _CreateSocket(addr , port);
	} catch (const soar_components_errctl::SoarException & exp) {
		//TODO: log fail events
		status_ = SOCK_INIT;
		throw;
	}
}

int SoarSocket::ListenAndBind(uint16_t port) throw(soar_components_errctl::SoarException) {
	try {
		sockfd_ = socket(AF_INET , SOCK_STREAM , 0);
		if (sockfd_ < 0)
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_CREATE_FAILED);
		bzero(&sock_addr_ , sizeof(sock_addr_));
		sock_addr_.sin_family = AF_INET;
		sock_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
		sock_addr_.sin_port = htons(port);

		if (bind(sockfd_ , (struct sockaddr*)&sock_addr_ , sizeof(sock_addr_))) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_BIND_FAILED , soar_communal::SoarString(strerror(errno)));
		}
		status_ = SOCK_BIND;
		if (listen(sockfd_ , SOMAXCONN)) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_BIND_FAILED , soar_communal::SoarString(strerror(errno)));
		}
		status_  = SOCK_LISTENING;
		return 0;
	} catch (const soar_components_errctl::SoarException & exp) {
		//TODO: log fail events
		status_ = SOCK_INIT;
		throw;
	}
	return 1;
}

int SoarSocket::SetNonBlocking() {
	int flags;
	flags = fcntl(sockfd_ , F_GETFL , 0);
	if (flags < 0) {
		return flags;
	}
	return fcntl(sockfd_ , F_SETFL , flags | O_NONBLOCK);
}
int SoarSocket::SetBlocking() {
	int flags;
	flags = fcntl(sockfd_, F_GETFL, 0);
	if (flags < 0) {
		return flags;
	}
	return fcntl(sockfd_, F_SETFL, flags & ~O_NONBLOCK);
}

void SoarSocket::SetKeepAlive(int idleTime , int intvlCheckTime , int checkCnt) throw(soar_components_errctl::SoarException) {
//	int optval;
//	socklen_t optlen;
//
//	optval = 1;
//
//
//	if (setsockopt(sockfd_ , SOL_SOCKET , SO_KEEPALIVE , &optval , optlen)) {
//		throw soar_components_errctl::SoarException(soar_exception::SOCKET_SETOPT_FAILED);
//	}
//
//	optval = idleTime;
//	if (setsockopt(sockfd_ , IPPROTO_TCP , TCP_KEEPIDLE , &optval , optlen)) {
//		throw soar_components_errctl::SoarException(soar_exception::SOCKET_SETOPT_FAILED);
//	}
}

int SoarSocket::Connect(const char* addr , uint16_t port)throw(soar_components_errctl::SoarException) {
	try {
		sockfd_ = _CreateSocket(addr, port);
		int conn_res = connect(sockfd_, (struct sockaddr*) &sock_addr_,
				sizeof(struct sockaddr_in));
		if (conn_res) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_CONNECT_FAILED,
					soar_communal::SoarString(strerror(errno)));
		} else {
			status_ = SOCK_CONN;
			return 0;
		}
	} catch (const soar_components_errctl::SoarException& exp) {
		//TODO: log fail events
		//TODO: retry if attempted to connect
		throw;
	}
	return 0;
}

int SoarSocket::Connect()throw(soar_components_errctl::SoarException) {
	try {
		if (status_ != SOCK_CREATE || !sockfd_) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_NOT_CREATED);
		}
		int conn_res = connect(sockfd_, (struct sockaddr*) &sock_addr_,
				sizeof(struct sockaddr_in));
		if (conn_res) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_CONNECT_FAILED,
					soar_communal::SoarString(strerror(errno)));
		} else {
			status_ = SOCK_CONN;
			return 0;
		}
	} catch (const soar_components_errctl::SoarException& exp) {
		//TODO: log fail events
		//TODO: retry if attempted to connect or wait if socket not created
		throw;
	}
	return 0;
}

int SoarSocket::Send(char* src , int nlen) const throw(soar_components_errctl::SoarException) {
	try {
		if (status_ != SOCK_CONN || !sockfd_) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_NOT_CONNECTED);
		}
		int nleft = nlen;
		int nsend = 0;
		char* ptr = src;
		while (nleft > 0) {
			if ((nsend = write(sockfd_ , ptr , nleft)) <0) {
				if (errno == EINTR)
					nsend = 0;
				else
					throw soar_components_errctl::SoarException(soar_exception::SOCKET_SEND_ERROR ,soar_communal::SoarString(strerror(errno)) );
				nleft -= nsend;
				ptr += nsend;
			}
		}
	} catch (const soar_components_errctl::SoarException& exp) {
		//TODO: log fail events
		//TODO: wait if attempted to connect
		throw;
	}
	return 0;
}

int SoarSocket::Recv(char* buffer , int max_buffer_len) const throw(soar_components_errctl::SoarException) {
	try {
		if (status_ != SOCK_CONN || !sockfd_) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_NOT_CONNECTED);
		}
		int recv_cnt = read(sockfd_ , buffer , max_buffer_len);
		if (recv_cnt < 0){
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_RECV_ERROR ,soar_communal::SoarString(strerror(errno)) );
		}
		return recv_cnt;
	} catch (const soar_components_errctl::SoarException& exp) {
		//TODO: log fail events
		//TODO: wait if attempted to connect
		throw;
	}
	return 0;
}
int SoarSocket::Recv_n(char* buffer , int required_nbytes) const throw(soar_components_errctl::SoarException) {
	try {
		if (status_ != SOCK_CONN || !sockfd_) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_NOT_CONNECTED);
		}
		int recv_cnt(0);
		int tmp_cnt(0);
		while (recv_cnt < required_nbytes) {
			tmp_cnt = read(sockfd_ , buffer + recv_cnt ,  required_nbytes - recv_cnt);
			if (tmp_cnt < 0) {
				throw soar_components_errctl::SoarException(soar_exception::SOCKET_RECV_NOT_ENOUGH ,soar_communal::SoarString(strerror(errno)) );
			}
			recv_cnt += tmp_cnt;
		}
		return recv_cnt;
	} catch (const soar_components_errctl::SoarException& exp) {
		//TODO: log fail events
		//TODO: wait if attempted to connect
		throw;
	}
	return 0;
}

void SoarSocket::Close() throw() {
	close(sockfd_);
	status_ = SOCK_CLOSE;
}

SoarSocket::~SoarSocket() {
	Close();
}
