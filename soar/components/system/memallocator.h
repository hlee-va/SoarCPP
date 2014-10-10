/*
 * memallocator.h
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SYSTEM_MEMALLOCATOR_H_
#define SOAR_COMPONENTS_SYSTEM_MEMALLOCATOR_H_

#include <cstddef>

#include <limits>
#include <new>

#ifdef __MULTI_THREAD_MEM_SHARE__
#include "lock.h"
#endif

namespace soar_components_system {

class __block_allocator {
	/*
	 * CLASS: __block_allocator
	 *
	 *	This is a class that management memory directly by calling function "malloc","realloc" and "free".
	 *	Moreover, this class will be utilized as the backup mem-allocator when trying to obtained more than __MAX_BYTES
	 *	from SOAR Memory Pool.
	 */
private:
	static void* out_of_mem_malloc(size_t nsize);
	static void* out_of_mem_realloc(void* ptr, size_t nsize);
	static void (*__malloc_out_of_mem_handler)();
public:
	static void* allocate(size_t nsize);
	static void deallocate(void* ptr, size_t nsize = 0 /* nsize */);
	static void* reallocate(void* ptr, size_t /* old_size */, size_t new_size);
	//self implementation of "set_new_handler()"
	static void (*set_new_handler(void (*func)()))() {
		void (*old)() = __malloc_out_of_mem_handler;
		__malloc_out_of_mem_handler = func;
		return old;
	}
};
typedef __block_allocator SystemAllocator;

#define __ALIGN 8
#define __MAX_BYTES 128
#define __NFREELISTS (__MAX_BYTES/__ALIGN)
#define ROUND_UP(bytes) ((bytes + __ALIGN - 1) & ~(__ALIGN - 1))
#define GET_FREELIST_IDX(bytes) ((bytes + __ALIGN - 1)/__ALIGN - 1)
#define ONCE_FILL_NUM 20

class __min_allocator {
private:
	union obj {
		union obj* free_list_link;
		char client_data;
	};
private:
	obj* volatile free_list[__NFREELISTS];
	void* fill_list(size_t nsize);
	char* chunk_alloc(size_t nsize, int& nobjs);

	//Chunk allocation state
	char* free_begin;
	char* free_end;
	size_t heap_size;
	char* chunk_list;
#ifdef __MULTI_THREAD_MEM_SHARE__
private:
	StdMutexLock alloc_lock_;
#endif

private:
	__min_allocator(__min_allocator&)throw(){};
public:
	__min_allocator() throw();
	~__min_allocator() throw();

private:
	void* allocate(size_t nsize);
	void deallocate(void* ptr, size_t nsize);

public:
	void* alloc(size_t nsize) throw(std::bad_alloc);
	void free(void* ptr);
};

typedef __min_allocator MemoryPool;

extern soar_components_system::MemoryPool g_mempool;

template <class T> class SoarSTLAllocator {
private:
	static MemoryPool global_mempool;
public:
	typedef size_t		size_type;
	typedef ptrdiff_t 	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;

	template <class U> struct rebind {
		typedef SoarSTLAllocator< U > other;
	};

	inline pointer address(reference value) const  {
		return &value;
	}
	inline const_pointer address(const_reference value) const {
		return &value;
	}

	SoarSTLAllocator()throw() {}
	SoarSTLAllocator(const SoarSTLAllocator&) throw(){}
	template < class U > SoarSTLAllocator(const SoarSTLAllocator< U > &)throw(){}
	~SoarSTLAllocator()throw(){}
	inline size_type max_size() const throw() {
		return std::numeric_limits< size_t >::max()/sizeof(T);
	}
	pointer allocate(size_type num , const void* hint = 0);
	void deallocate(pointer p , size_type num);
	inline void construct (pointer p , const T& value) {
		new (p)T(value);
	}
	inline void destroy(pointer p) {
		p->~T();
	}
};

template <class T1 , class T2>inline bool operator==(const SoarSTLAllocator< T1 >& , const SoarSTLAllocator< T2 >&) throw() {
	return true;
}
template <class T1 , class T2>inline bool operator!=(const SoarSTLAllocator< T1 >& , const SoarSTLAllocator< T2 >&) throw() {
	return false;
}

template< class T > MemoryPool SoarSTLAllocator<T>::global_mempool;
template< class T > typename SoarSTLAllocator<T>::pointer SoarSTLAllocator<T>::allocate(SoarSTLAllocator<T>::size_type num , const void* hint) {
	return (pointer)SoarSTLAllocator<T>::global_mempool.alloc(num * sizeof(T));
}
template< class T > void SoarSTLAllocator<T>::deallocate(SoarSTLAllocator<T>::pointer p , SoarSTLAllocator<T>::size_type num) {
	SoarSTLAllocator<T>::global_mempool.free((void*)p);
}

}/*namespace soar_components_system*/



#endif /* SOAR_COMPONENTS_SYSTEM_MEMALLOCATOR_H_ */
