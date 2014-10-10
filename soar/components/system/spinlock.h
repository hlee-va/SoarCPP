/*
 * spinlock.h
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SYSTEM_SPINLOCK_H_
#define SOAR_COMPONENTS_SYSTEM_SPINLOCK_H_


namespace soar_components_system {

static inline void cpu_relax() {
	__asm __volatile (
			"rep;nop\n\t"
			:::"memory"
	);
}

class SpinLock {
private:
	volatile int locking;
private:
	SpinLock(SpinLock&);
public:
	SpinLock():locking(0) {}
	inline int is_lock() {
		return this->locking;
	}
	int TryLock();
	void Lock();
	void Unlock();
};

}/*namespace soar_components_system*/


#endif /* SOAR_COMPONENTS_SYSTEM_SPINLOCK_H_ */
