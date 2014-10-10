/*
 * thread.cc
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#include "thread.h"

using namespace soar_components_system;

void SoarThread::run()throw(soar_components_errctl::SoarException) {
	if (!callback_func_)
		throw soar_components_errctl::SoarException(soar_exception::THREAD_CALLBACK_NOT_SET);
	if (id_)
		throw soar_components_errctl::SoarException(soar_exception::THREAD_RUNNING_IN_PROGRESS);
	pthread_create(&id_ , NULL , callback_func_ , arg_);
}

void SoarThread::join() {
	pthread_join(id_ , NULL);
	id_ = 0;
}

void SoarThread::join(SoarThread& thread) {
	pthread_join(thread.get_id() , NULL);
	thread.reset_id();
}


