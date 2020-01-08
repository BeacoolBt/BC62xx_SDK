#ifndef __CO_COM__
#define __CO_COM__

#include "co_log.h"
#include "co_list.h"

#define MESH_VER "1.0.3"
#define MESH_LVER "a"


#define __STATIC static

#define __INLINE_S__ static inline

/// define size of an empty array (used to declare structure with an array size not defined)
#define __ARRAY_EMPTY

/// ARM is little endian
#define CPU_LE          1


#ifdef MESH_RELEASE
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1)
#else
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond) {while(!(cond));}

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1) {while(!(cond));}

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1) {while(!(cond));}
#endif

__INLINE_S__ void GLOBAL_INT_DISABLE(void )
{
	__asm volatile ("cpsid i" : : : "memory");
}

__INLINE_S__ void GLOBAL_INT_RESTORE(void)
{
	__asm volatile ("cpsie i" : : : "memory");
}

#endif/* __CO_COM__ */
