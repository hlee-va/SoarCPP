/*
 * thread.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SYSTEM_THREAD_H_
#define SOAR_COMPONENTS_SYSTEM_THREAD_H_

#include <pthread.h>

#include "../../communal.h"
#include "../errctl/exception.h"

namespace soar_components_system {

typedef void* (*ThreadCallBack)(void*);

class SoarThread {
private:
	ThreadCallBack 	callback_func_;
	void*			arg_;
	pthread_t		id_;
private:
	DISALLOW_COPY_AND_ASSIGN(SoarThread);
public:
	SoarThread():callback_func_(NULL),arg_(NULL),id_(0){}
	explicit SoarThread(ThreadCallBack src_func , void* src_arg = NULL):callback_func_(src_func),arg_(src_arg),id_(0){}

	inline void reset_id() { id_ = 0;}
	inline pthread_t get_id(){return id_;}

	inline void set_arg(void* src_arg) { arg_ = src_arg; }
	inline void* get_arg() { return arg_; }

	inline void set_callback_func(ThreadCallBack callback) { callback_func_ = callback; }
	inline ThreadCallBack get_callback_func() { return callback_func_; }

	void run() throw(soar_components_errctl::SoarException);

	void join();
	void join(SoarThread& thread);
};

}/*namespace soar_components_system*/



#endif /* SOAR_COMPONENTS_SYSTEM_THREAD_H_ */
