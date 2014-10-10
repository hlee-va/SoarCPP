/*
 * memallocator.cc
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#include <cstdlib>
#include <cstring>

#include "memallocator.h"
using namespace soar_components_system;
MemoryPool g_mempool;

#ifndef NULL
#define NULL ((void*)0)
#endif /* ! NULL */


void (*__block_allocator::__malloc_out_of_mem_handler)() = NULL;
void* __block_allocator::out_of_mem_malloc(size_t nsize) {
	void* res;
	for (;;) {
		if (!__malloc_out_of_mem_handler) {
			throw std::bad_alloc();
		}
		__malloc_out_of_mem_handler();
		res = malloc(nsize);
		if (res)
			return res;
	}
	return NULL;
}
void* __block_allocator::out_of_mem_realloc(void* ptr, size_t nsize) {
	void* res;
	for (;;) {
		if (!__malloc_out_of_mem_handler) {
			std::bad_alloc exception;
			throw exception;
		}
		__malloc_out_of_mem_handler();
		res = realloc(ptr, nsize);
		if (res)
			return res;
	}
	return NULL;
}
void* __block_allocator::allocate(size_t nsize) {
	void* res = malloc(nsize);
	if (!res)
		res = out_of_mem_malloc(nsize);
	return res;
}
void __block_allocator::deallocate(void* ptr, size_t /* nsize = 0 */) {
	//trashy parameter: nsize
	free(ptr);
}
void* __block_allocator::reallocate(void* ptr, size_t /* old_size */,
		size_t new_size) {
	void* res = realloc(ptr, new_size);
	if (!res)
		res = out_of_mem_realloc(ptr, new_size);
	return res;
}

__min_allocator::__min_allocator() throw() {
	memset((void*)free_list , 0 , sizeof(free_list));
	chunk_list = free_begin = free_end = NULL;
	heap_size = 0;
}
__min_allocator::~__min_allocator()throw() {
	while (chunk_list) {
		char* ptr = chunk_list;
		chunk_list = *(char**) chunk_list;
		__block_allocator::deallocate(ptr);
	}
}

void* __min_allocator::allocate(size_t nsize) {
	obj* volatile * my_free_list;
	obj* res;
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Lock();
//#endif
	if (nsize > (size_t) __MAX_BYTES) {
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Unlock();
//#endif
		return __block_allocator::allocate(nsize);
	}
	my_free_list = free_list + GET_FREELIST_IDX(nsize);
	res = *my_free_list;

	if (!res) {
		void* r = fill_list(ROUND_UP(nsize));
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Unlock();
//#endif
		return r;
	}
	*my_free_list = res->free_list_link;
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Unlock();
//#endif
	return res;
}

void __min_allocator::deallocate(void* ptr, size_t nsize) {
	obj* volatile * my_free_list;
	obj* q = (obj*) ptr;
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Lock();
//#endif
	if (nsize > (size_t) __MAX_BYTES) {
		__block_allocator::deallocate(ptr);
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Unlock();
//#endif
		return;
	}
	my_free_list = free_list + GET_FREELIST_IDX(nsize);
	q->free_list_link = *my_free_list;
	*my_free_list = q;
//#ifdef __MULTI_THREAD_MEM_SHARE__
	alloc_lock_.Unlock();
//#endif
}

void* __min_allocator::fill_list(size_t nsize) {
	int nobjs = ONCE_FILL_NUM;
	char* chunk = chunk_alloc(nsize, nobjs);
	obj* volatile * my_free_list;
	obj* res;
	obj* cur_obj;
	obj* next_obj;

	if (1 == nobjs)
		return chunk;

	my_free_list = free_list + GET_FREELIST_IDX(nsize);
	res = (obj*) chunk;
	*my_free_list = next_obj = (obj*) (chunk + nsize);
	for (int i = 1;; ++i) {
		cur_obj = next_obj;
		next_obj = (obj*) ((char*) next_obj + nsize);
		if (nobjs - 1 == i) {
			cur_obj->free_list_link = NULL;
			break;
		} else {
			cur_obj->free_list_link = next_obj;
		}
	}
	return (void*) res;
}

char* __min_allocator::chunk_alloc(size_t nsize, int& nobjs) {
	char* res;
	size_t total_bytes = nsize * nobjs;
	size_t bytes_left = free_end - free_begin;
	if (bytes_left >= total_bytes) {
		res = free_begin;
		free_begin += total_bytes;
		return res;
	} else if (bytes_left >= nsize) {
		nobjs = bytes_left / nsize;
		total_bytes = nsize * nobjs;
		res = free_begin;
		free_begin += total_bytes;
		return res;
	} else {
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		if (bytes_left > 0) {
			obj* volatile * my_free_list = free_list
					+ GET_FREELIST_IDX(bytes_left);
			((obj*) free_begin)->free_list_link = *my_free_list;
			*my_free_list = (obj*) free_begin;
		}
		free_begin = (char*) __block_allocator::allocate(bytes_to_get + sizeof(char*));
		if (!free_begin) {
			obj* volatile * my_free_list;
			obj* p;
			for (int i = nsize; i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + GET_FREELIST_IDX(i);
				p = *my_free_list;
				if (p) {
					*my_free_list = p->free_list_link;
					free_begin = (char*) p;
					free_end = free_begin + i;
					return chunk_alloc(nsize, nobjs);
				}
			}
			free_end = NULL;
			free_begin = (char*) __block_allocator::allocate(bytes_to_get + sizeof(char*));
		}
		*(char**)free_begin = chunk_list;
		chunk_list = free_begin;
		free_begin += sizeof(char*);

		heap_size += bytes_to_get;
		free_end = free_begin + bytes_to_get;
		return chunk_alloc(nsize, nobjs);
	}
}

void* __min_allocator::alloc(size_t nsize) throw(std::bad_alloc) {
	if (nsize <= 0)
		return NULL;
	int32_t n_realsize = nsize + 4;
	void* ptr = allocate(n_realsize);
	if (!ptr) {
		throw std::bad_alloc();
//		return NULL;
	}
	*(int32_t*) ptr = n_realsize;
	return (void*) ((char*) ptr + sizeof(int32_t));
}
void __min_allocator::free(void* ptr) {
	if (!ptr)
		return;
	void* p = (void*) ((char*) ptr - sizeof(int32_t));
	int32_t nsize = *(int32_t*) p;
	deallocate(p, nsize);
}

