/*
 * communal.h
 *
 *  Created on: Feb 26, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMMUNAL_H_
#define SOAR_COMMUNAL_H_

#include <string>

#include "components/system/memallocator.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
			TypeName(const TypeName&);\
			void operator=(const TypeName&)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif /* ! NULL */

namespace soar_components_scheduling {
	typedef int PipeLineIndex;
}

namespace soar_communal {

typedef std::basic_string<char ,std::char_traits<char> , soar_components_system::SoarSTLAllocator<char> > SoarString;
template < class T >
inline T atomic_fetch_then_add(T* src, int value) {
	//return *src
	__asm __volatile(
			"lock; xaddl %%eax,%2;\n\t"
			:"=a"(value)
			:"a"(value),"m"(*src)
			:"memory"
	);
	return value;
}
template < class T >
inline T atomic_add_then_fetch(T* src, int value) {
	//return (*src) + value
	__asm __volatile(
			"lock; xaddl %%eax,%2;\n\t"
			:"=a"(value)
			:"a"(value),"m"(*src)
			:"memory"
	);
	return *src;
}
template < class T >
inline T atomic_fetch_then_sub(T* src, int value) {
	//return *src
	__asm __volatile(
			"lock; xaddl %%eax,%2;\n\t"
			:"=a"(value)
			:"a"(-value),"m"(*src)
			:"memory"
	);
	return value;
}
template < class T >
inline T atomic_sub_then_fetch(T* src, int value) {
	//return (*src) - value
	__asm __volatile(
			"lock; xaddl %%eax,%2;\n\t"
			:"=a"(value)
			:"a"(-value),"m"(*src)
			:"memory"
	);
	return *src;
}
template< class T >
inline bool atomic_compare_and_swap_bool(T* src , int cmp_value , int swap_value) {
	int rtn(0);
	__asm __volatile(
			"lock; cmpxchgl %%ecx,%1;\n\t"
			"jnz SOAR_ATOMIC_CASB_END;\n\t"
			"movl $1,%0;\n\t"
			"SOAR_ATOMIC_CASB_END:\n\t"
			:"=m"(rtn),"+m"(*src)
			:"a"(cmp_value),"c"(swap_value)
			:"memory"
	);
	return rtn;
}
template< class T >
inline T atomic_compare_and_swap_value(T* src , int cmp_value , int swap_value) {
	int rtn(*src);
	__asm __volatile(
			"lock; cmpxchgl %%ecx,%0;\n\t"
			:"=m"(*src)
			:"a"(cmp_value),"c"(swap_value)
			:"memory"
	);
	return rtn;
}

}/*namespace soar_communal*/



#endif /* SOAR_COMMUNAL_H_ */
