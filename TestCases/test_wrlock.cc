/*
 * test_wrlock.cc
 *
 *  Created on: Apr 2, 2013
 *      Author: House_Lee
 */
#include <cstdio>
#include <time.h>
#include <pthread.h>


#include "../soar/components/system/lock.h"

using namespace soar_components_system;

#define MAX_READ_THREAD 40
#define MAX_WRITE_THREAD 3
#define NLOOP 5000

SoarWRLock test_lock;
//StdRWLock	test_lock;
int cnt;

void* write_thread_func(void* arg) {
	int val;
	for(int i = 0; i != NLOOP; ++i) {
		test_lock.LockW();
		val = cnt;
		cnt = val + 1;
		test_lock.UnlockW();
	}
	return NULL;
}

void* read_thread_func(void* arg) {
	int val;
	for(;;) {
		test_lock.LockR();
		val = cnt;
		test_lock.UnlockR();
//		usleep(200);
	}
	return NULL;
}
int main() {
	cnt = 0;
	pthread_t wr_threads[MAX_WRITE_THREAD];
	pthread_t rd_threads[MAX_READ_THREAD];
	clock_t start, end;
	for (int i = 0; i != MAX_READ_THREAD; ++i) {
		pthread_create(&rd_threads[i], NULL, read_thread_func, NULL);
	}
	start = clock();
	for (int i = 0; i != MAX_WRITE_THREAD; ++i) {
		pthread_create(&wr_threads[i], NULL, write_thread_func, NULL);
	}

	for (int i = 0; i != MAX_WRITE_THREAD; ++i) {
		pthread_join(wr_threads[i], NULL);
	}
	end = clock();
	printf("Total cost: %lf , cnt: %d\n" ,(double)(end-start)/CLOCKS_PER_SEC , cnt);
	for (int i = 0; i != MAX_READ_THREAD; ++i) {
			pthread_join(rd_threads[i], NULL);
	}
}
