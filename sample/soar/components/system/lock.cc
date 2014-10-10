/*
 * lock.cc
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */


#include "lock.h"
#include "../../communal.h"

using namespace soar_components_system;



StdRWLock::StdRWLock() throw() {
	pthread_rwlock_init(&lock_ , NULL);
}

void StdRWLock::LockR() throw() {
	pthread_rwlock_rdlock(&lock_);
}

void StdRWLock::LockW() throw() {
	pthread_rwlock_wrlock(&lock_);
}

void StdRWLock::Unlock() throw() {
	pthread_rwlock_unlock(&lock_);
}

StdRWLock::~StdRWLock() {
	pthread_rwlock_destroy(&lock_);
}

StdMutexLock::StdMutexLock() throw() {
	pthread_mutex_init(&lock_ , NULL);
}

void StdMutexLock::Lock() throw(){
	pthread_mutex_lock(&lock_);
}

void StdMutexLock::Unlock() throw(){
	pthread_mutex_unlock(&lock_);
}

StdMutexLock::~StdMutexLock() {
	pthread_mutex_destroy(&lock_);
}


void SoarWRLock::LockR() throw(){
	while(!soar_communal::atomic_compare_and_swap_bool(&writer_cnt_ , 0 , 0));
	while(soar_communal::atomic_add_then_fetch(&reader_cnt_ , 1) <= 0);
}

void SoarWRLock::UnlockR() throw() {
	soar_communal::atomic_fetch_then_sub(&reader_cnt_ , 1);
//	if (reader_cnt_ < 0)
//		reader_cnt_ = 0;
}

void SoarWRLock::LockW() throw(){
	while(soar_communal::atomic_add_then_fetch(&writer_cnt_ , 1) <= 0);
	while( !soar_communal::atomic_compare_and_swap_bool(&reader_cnt_ , 0 , 0) || !soar_communal::atomic_compare_and_swap_bool(&is_writing_ , 0 , 1))
		cpu_relax();
//	while(reader_cnt_ || is_writing_) cpu_relax();
//	is_writing_ = 1;
//	soar_communal::atomic_compare_and_swap_value(&is_writing_ , 0 , 1);
}

void SoarWRLock::UnlockW() throw(){
	soar_communal::atomic_fetch_then_sub(&writer_cnt_ , 1);
	soar_communal::atomic_compare_and_swap_value(&is_writing_ , 1 , 0);
//	if (writer_cnt_ < 0)
//		writer_cnt_ = 0;
}



