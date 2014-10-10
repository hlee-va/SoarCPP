/*
 * event.cc
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */
#ifdef __linux__
#include "event/epoll.cc"
#else
//utilize kqueue for implentation under Unix

#endif
