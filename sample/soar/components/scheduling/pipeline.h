/*
 * pipeline.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SCHEDULING_PIPELINE_H_
#define SOAR_COMPONENTS_SCHEDULING_PIPELINE_H_

#include <deque>
#include <vector>
#include "../system/thread.h"
#include "../system/lock.h"
#include "../errctl/exception.h"

struct SoarDataBlock{
	int length;
	void* data;
};

namespace soar_components_scheduling {

typedef std::deque<SoarDataBlock , soar_components_system::SoarSTLAllocator< SoarDataBlock > > DataQueue_t;
typedef soar_components_system::StdMutexLock MutexLock;
typedef soar_components_system::SoarThread	Thread_t;
typedef SoarDataBlock (*ProcedureFunc)(SoarDataBlock);

enum PipeStatus {
	PIPE_INIT = 0,
	PIPE_RUNNING
};


class PipeNode {
	friend void* work_thread_callback_(void* /*arg*/);
private:
	PipeNode* 		next_;
	int 			n_worker_;
	ProcedureFunc 	process_func_;

	MutexLock	 	buffer_lock_;
	DataQueue_t 	input_buffer_;
	Thread_t*		worker_threads;
public:
	PipeNode():next_(NULL) , process_func_(NULL),n_worker_(0),worker_threads(NULL){}
	PipeNode(ProcedureFunc func , int n_workers):next_(NULL),process_func_(func),n_worker_(n_workers),worker_threads(NULL){}
	explicit PipeNode(const PipeNode& src):next_(src.next_),process_func_(src.process_func_),n_worker_(src.n_worker_),worker_threads(NULL){}

	inline void setNext(PipeNode* next) {next_ = next;}
	inline void setProcessFunc(ProcedureFunc func){process_func_ = func;}
	inline void setNWorkers(int nworkers){ n_worker_ = nworkers;}

	inline void NewTask(const SoarDataBlock& new_data) {
		buffer_lock_.Lock();
		input_buffer_.push_back(new_data);
		buffer_lock_.Unlock();
	}

	inline void SaveTask(const SoarDataBlock& process_result){
		if (!next_)
			return;
		next_->NewTask(process_result);
	}

	void SwitchON()throw(soar_components_errctl::SoarException);

};
void* work_thread_callback_(void*);

class PipeLine {
private:
	std::vector< PipeNode* > nodes_;//cuz PipeLines were generally created during the service's initialization,so no need to utilize memory pool
public:
	PipeLine() {}
	PipeLine(std::vector< PipeNode* >::iterator begin, std::vector< PipeNode* >::iterator end):nodes_(begin,end){}
	explicit PipeLine(const std::vector< PipeNode* >& src_nodes):nodes_(src_nodes){}
	void AddNode(const PipeNode& node) throw();
	void NewTask(const SoarDataBlock& data);
	void SwitchON() throw(soar_components_errctl::SoarException);
	~PipeLine();
};

}/*namespace soar_components_scheduling*/

#endif /* SOAR_COMPONENTS_SCHEDULING_PIPELINE_H_ */
