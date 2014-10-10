/*
 * pipeline.cc
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#include <cstring>
#include <unistd.h>
#include "pipeline.h"
#include "../system/memallocator.h"

using namespace soar_components_scheduling;

void* soar_components_scheduling::work_thread_callback_(void* arg){
	PipeNode* node = (PipeNode*)arg;
	if (!node->process_func_)
		throw soar_components_errctl::SoarException(soar_exception::WORKER_FUNC_NOT_SET);
	SoarDataBlock tmp_buffer;
	for(;;) {
		memset(&tmp_buffer , 0 , sizeof(tmp_buffer));
		if(!(node->input_buffer_).empty()) {
			(node->buffer_lock_).Lock();
			if(!(node->input_buffer_).empty()) {
				tmp_buffer = node->process_func_(*(node->input_buffer_).begin());
				(node->input_buffer_).pop_front();
			}
			(node->buffer_lock_).Unlock();
		}
		if(tmp_buffer.length && tmp_buffer.data)
			node->SaveTask(tmp_buffer);
		usleep(1000);
	}
	return NULL;
}


void PipeNode::SwitchON()throw(soar_components_errctl::SoarException) {
	if(!n_worker_)
		throw soar_components_errctl::SoarException(soar_exception::WORKER_NUM_NOT_SET);
	if(!process_func_)
		throw soar_components_errctl::SoarException(soar_exception::WORKER_FUNC_NOT_SET);
	worker_threads = (Thread_t*)soar_components_system::SystemAllocator::allocate(n_worker_ * sizeof(Thread_t));
	for(int i = 0; i != n_worker_; ++i) {
		worker_threads[i].set_arg((void*)this);
		worker_threads[i].set_callback_func(work_thread_callback_);
		worker_threads[i].run();
	}
//	for(int i = 0; i != n_worker_; ++i) {
//		worker_threads[i].join();
//	}
//	soar_components_system::SystemAllocator::deallocate(worker_threads);
}


void PipeLine::AddNode(const PipeNode& node) throw(){
	PipeNode* save_node = new PipeNode(node);
	if(!nodes_.empty()) {
		PipeNode* tmp = nodes_.back();
		tmp->setNext(save_node);
	}
	nodes_.push_back(save_node);
}

void PipeLine::NewTask(const SoarDataBlock& data) {
	PipeNode* tmp = nodes_[0];
	tmp->NewTask(data);
}

void PipeLine::SwitchON() throw(soar_components_errctl::SoarException) {
	try {
		PipeNode* tmp = NULL;
		for (std::vector<PipeNode*>::iterator pos = nodes_.begin();pos != nodes_.end(); ++pos) {
			tmp = *pos;
			if(tmp)
				tmp->SwitchON();
		}
	} catch (soar_components_errctl::SoarException& exp) {
		throw;
	}
}

PipeLine::~PipeLine() {
	PipeNode* tmp = NULL;
	for (std::vector<PipeNode*>::iterator pos = nodes_.begin();pos != nodes_.end(); ++pos) {
		tmp = *pos;
		if (tmp);
//			delete tmp;
	}
}
