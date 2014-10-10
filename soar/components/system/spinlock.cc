/*
 * spinlock.cc
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#include "spinlock.h"

using namespace soar_components_system;


int SpinLock::TryLock() {
	volatile int rtn = 0;
	__asm __volatile (
			"mfence;\n\t"
			"xorl %%eax,%%eax;\n\t"
			"movl $0,%%eax;\n\t"
			"movl $1,%%ecx;\n\t"
			"lock cmpxchgl %%ecx,%0;\n\t"
			"mfence;\n\t"
			"jnz SOAR_SPIN_LOCK_FAILED;\n\t"
			"movl $1,%1;\n\t"
			"SOAR_SPIN_LOCK_FAILED:\n\t"
			:"+m"(this->locking),"+m"(rtn)
			:
			:"eax","ecx","memory"
	);
	return rtn;
}

void SpinLock::Lock() {
	do {
		while (is_lock())
			cpu_relax();
	}while (!TryLock());
}

void SpinLock::Unlock() {
	__asm __volatile(
			"mfence;\n\t"
			"movl $1,%%eax;\n\t"
			"movl $0,%%ecx;\n\t"
			"lock cmpxchgl %%ecx,%0;\n\t"
			"mfence;\n\t"
			:"+m"(this->locking)
			:
			:"eax","ecx","memory"
	);
}

