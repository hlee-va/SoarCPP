/*
 * soarapp.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOARAPP_H_
#define SOARAPP_H_

#include <stdio.h>
#include <stdint.h>
#include <vector>


#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "components/scheduling/pipeline.h"
#include "components/scheduling/distributor.h"
#include "components/network/socket.h"
#include "components/network/event.h"
#include "components/errctl/exception.h"

#define MAX_EVENTS 1024


template< class Distributor_t >
class SoarApp {
private:
	uint16_t 							port_;
	std::vector< soar_components_scheduling::PipeLine >				pipelines_;
	soar_components_scheduling::Distributor_Interface*				distributor;
	soar_components_network::SoarSocket	core_sock_;
	soar_components_network::SoarEvent*	events_;
private:
	DISALLOW_COPY_AND_ASSIGN(SoarApp);
private:
	void waiting_events() {
		printf("waiting for request\n");
		int n_current_events;
		soar_components_network::EventItem EventItem[MAX_EVENTS];
		soar_components_network::SoarSocket tmpSocket;
		while(1) {
			n_current_events = events_->EventLoop(EventItem , MAX_EVENTS , -1);
			for(int i = 0; i != n_current_events; ++i) {
				if(EventItem[i].is_erroccured()) {
					/* error occured*/
					continue;
				}
				if(EventItem[i].get_fd() == core_sock_.get_fd()) {
					/* new accept */
					/* process all the incoming accept request*/
					printf("incoming request\n");
					while(1) {
						struct sockaddr in_addr;
						socklen_t		in_len(sizeof(in_addr));
						int infd;
						infd = accept(core_sock_.get_fd() , &in_addr , &in_len);
						if (infd < 0) {
							if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
								/*all processed*/
								break;
							} else {
								/* accept error */
								//TODO:log all accept failed
								break;
							}
						}
						soar_components_network::EventItem newEvent;
						tmpSocket.set_fd(infd);
						tmpSocket.SetKeepAlive(20 , 1 , 5);
						if(tmpSocket.SetNonBlocking() < 0 ) {
							throw soar_components_errctl::SoarException(soar_exception::SOCKET_SETOPT_FAILED);
						}
						newEvent.set_fd(tmpSocket.get_fd());
						newEvent.monitor_read();
						if (events_->NewEvent(newEvent) < 0 ) {
							throw soar_components_errctl::SoarException(soar_exception::EVENT_ADD_FAILED);
						}
					}
				} else if(EventItem[i].get_fd()){
					if (EventItem[i].is_readable()) {
						/* Data tobe read from client */
						tmpSocket.set_fd(EventItem[i].get_fd());
						SoarDataBlock IncomingMsg;
						soar_components_scheduling::PipeLineIndex PipeLineID;
						IncomingMsg = distributor->RecvMessage(tmpSocket);
						PipeLineID = distributor->GetMsgDestination();
						pipelines_[PipeLineID].NewTask(IncomingMsg);
					}
				}
			}
		}
	}
public:
	SoarApp():port_(0),events_(NULL),distributor(new Distributor_t){}
	~SoarApp(){
		delete distributor;
		if(events_)
			delete events_;
	}
	inline void SetPort(uint16_t port) {
		port_ = port;
	}
	inline void RegisterPipeLine(const soar_components_scheduling::PipeLine& pipeline) {
		pipelines_.push_back(pipeline);
	}
	void Start() throw(soar_components_errctl::SoarException) {
		if(!port_)
			throw soar_components_errctl::SoarException(soar_exception::SERVICE_PORT_NOT_SET);
		if(pipelines_.empty())
			throw soar_components_errctl::SoarException(soar_exception::SERVICE_PIPELINE_EMPTY);
		for(std::vector< soar_components_scheduling::PipeLine >::iterator pos = pipelines_.begin(); pos != pipelines_.end(); ++pos) {
			pos->SwitchON();
		}
		core_sock_.ListenAndBind(port_);
		printf("bind at %d success" , port_);
		if(core_sock_.SetNonBlocking() < 0) {
			throw soar_components_errctl::SoarException(soar_exception::SOCKET_SETOPT_FAILED);
		}
		events_ = new soar_components_network::SoarEvent;
		if(events_->get_last_error()) {
			char tmp[5];
			snprintf(tmp , 5 , "%d" , events_->get_last_error());
			throw soar_components_errctl::SoarException(soar_exception::EVENT_CREATED_FAILED , soar_communal::SoarString(tmp));
		}
		soar_components_network::EventItem event;
		event.set_fd(core_sock_.get_fd());
		event.monitor_read();
		events_->NewEvent(event);
		waiting_events();
	}

};


#endif /* SOARAPP_H_ */
