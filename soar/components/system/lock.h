/*
 * lock.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SYSTEM_LOCK_H_
#define SOAR_COMPONENTS_SYSTEM_LOCK_H_

#include "../../communal.h"
#include "spinlock.h"
#include "../errctl/exception.h"

#include <pthread.h>

namespace soar_components_system {

class StdRWLock {
private:
	DISALLOW_COPY_AND_ASSIGN(StdRWLock);
private:
	pthread_rwlock_t lock_;
public:
	StdRWLock() throw();
	void LockR() throw();
	void LockW() throw();
	void Unlock() throw();
	~StdRWLock();
};

class StdMutexLock {
private:
	DISALLOW_COPY_AND_ASSIGN(StdMutexLock);
private:
	pthread_mutex_t lock_;
public:
	StdMutexLock() throw();
	void Lock() throw();
	void Unlock() throw();
	~StdMutexLock();
};


//#define MAX_SOAR_WRITER_CNT 256
//class SoarRWLock {
//	//TODO to be done~~
//private:
//	DISALLOW_COPY_AND_ASSIGN(SoarRWLock);
//private:
//	SpinLock writing_lock;
//	volatile int reader_cnt;
//	int writer_index_stack[MAX_SOAR_WRITER_CNT];
//	volatile int id_top_;
//	int writer_queue_[MAX_SOAR_WRITER_CNT];
//	volatile int wq_head_;
//	volatile int wq_tail_;
//	volatile int writers_cnt;
//private:
//	inline int get_index() {
//		int tmp(soar_communal::atomic_sub_then_fetch(&id_top_ , 1));
//		if (tmp <= 0)
//			return -1;
//		else
//			return writer_index_stack[tmp];
//	}
//	inline void save_index(int id) {
//		int tmp;
//		while((tmp = soar_communal::atomic_fetch_then_add(&id_top_ , 1))< 0);
//		if (tmp >= MAX_SOAR_WRITER_CNT)
//			return;
//		writer_index_stack[tmp] = id;
//	}
//	inline bool push_writer(int id) {
//		if(id != -1) {
//			writing_lock.Lock();
//			writer_queue_[wq_tail_] = id;
//			wq_tail_ = (++wq_tail_)%MAX_SOAR_WRITER_CNT;
//			++writers_cnt;
//			writing_lock.Unlock();
//			return true;
//		}
//		return false;
//	}
//	inline int get_top_writer() {
//		/*
//		 * TODO
//		 */
//	}
//	inline int pop_top_writer(){
//	}
//public:
//	SoarRWLock() throw();
//
//};


class SoarWRLock {
private:
	DISALLOW_COPY_AND_ASSIGN(SoarWRLock);
private:
	volatile int is_writing_;
	volatile int writer_cnt_;
	volatile int reader_cnt_;
public:
	SoarWRLock():is_writing_(0),writer_cnt_(0),reader_cnt_(0){}
	void LockR() throw();
	void UnlockR() throw();

	void LockW() throw();
	void UnlockW() throw();
};

}/*namespace soar_components_system*/


#endif /* SOAR_COMPONENTS_SYSTEM_LOCK_H_ */
