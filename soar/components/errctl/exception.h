/*
 * exception.h
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_ERRCTL_EXCEPTION_H_
#define SOAR_COMPONENTS_ERRCTL_EXCEPTION_H_

#include "../../communal.h"

namespace soar_exception {
	enum ErrorID {
	SOCKET_NOT_CREATED = 1,
	SOCKET_CREATE_FAILED,
	SOCKET_NOT_CONNECTED,
	SOCKET_CONNECT_FAILED,
	SOCKET_BIND_FAILED,
	SOCKET_SETOPT_FAILED,
	SOCKET_SEND_ERROR,
	SOCKET_RECV_ERROR,
	SOCKET_RECV_NOT_ENOUGH,
	THREAD_CALLBACK_NOT_SET,
	THREAD_RUNNING_IN_PROGRESS,
	WORKER_FUNC_NOT_SET,
	WORKER_NUM_NOT_SET,
	SERVICE_PORT_NOT_SET,
	SERVICE_PIPELINE_EMPTY,
	EVENT_CREATED_FAILED,
	EVENT_ADD_FAILED,
	};
}


namespace soar_components_errctl {

class SoarException {
private:
	int error_id_;
	soar_communal::SoarString error_msg_;
	SoarException();
public:
	explicit SoarException(const int error_id , soar_communal::SoarString error_msg = ""):error_id_(error_id),error_msg_(error_msg){}
	SoarException(const SoarException& exp):error_id_(exp.error_id_),error_msg_(exp.error_msg_){}

	inline void set_error_id(int specified_id){error_id_ = specified_id;}
	inline int get_error_id(){return error_id_;}

	inline void set_error_msg(soar_communal::SoarString& specified_msg) {error_msg_ = specified_msg;}
	inline const soar_communal::SoarString& get_error_msg() const {return error_msg_;}
};

}/*namespace soar_components_errctl*/



#endif /* SOAR_COMPONENTS_ERRCTL_EXCEPTION_H_ */
