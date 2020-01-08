/*
 * Copyright 2016, yichip Semiconductor(shenzhen office)
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Yichip Semiconductor;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Yichip Semiconductor.
 */
#ifndef __BC62XX_DEF_H__
#define __BC62XX_DEF_H__

/* SDK version information */
#define SDK_VERSION			0L              /**< major version number */
#define SDK_SUBVERSION		0L              /**< minor version number */
#define SDK_REVISION			1L              /**< revise version number */


struct list_node
{    
	struct list_node *next;                          /**< point to next node. */    
	struct list_node *prev;                          /**< point to prev node. */
};

typedef struct list_node list_t;                  /**< Type for lists. */

#define typeof(x)  uint32_t;


/**
 * Base structure of Memory pool object
 */
struct mempool
{    
    void            *start_address;                     /**< memory pool start */
    uint16_t         size;                              /**< size of memory pool */

    uint16_t        block_size;                        /**< size of memory blocks */
    uint8_t*        block_list;                        /**< memory blocks list */

    uint8_t       block_total_count;                 /**< numbers of memory block */
    uint8_t       block_free_count;                  /**< numbers of free memory block */
    
};
typedef struct mempool *mp_t;

typedef uint32_t return_code;

/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
	#include <stdarg.h>
	#define SECTION(x)                  __attribute__((section(x)))
	#define YC_UNUSED                   __attribute__((unused))
	#define YC_USED                     __attribute__((used))
	#define ALIGN(n)                    __attribute__((aligned(n)))
	#define WEAK						__weak
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
	#include <stdarg.h>
	#define SECTION(x)                  @ x
	#define YC_UNUSED
	#define YC_USED
	#define PRAGMA(x)                   _Pragma(#x)
	#define ALIGN(n)                    PRAGMA(data_alignment=n)
	#define WEAK                        __weak
#elif defined (__GNUC__)                /* GNU GCC Compiler */
	#define SECTION(x)                  __attribute__((section(x)))
	#define YC_UNUSED                   __attribute__((unused))
	#define YC_USED                     __attribute__((used))
	#define ALIGN(n)                    __attribute__((aligned(n)))
	#define WEAK                        __attribute__((weak))
#else
	#error not supported tool chain
#endif

#define HW_REG_8BIT(reg, value)		(*((volatile uint8_t *)((uint32_t)reg)) = value)
#define HR_REG_8BIT(reg)				(*(volatile uint8_t *)((uint32_t)reg))
#define H_SETBIT(reg, value)			HW_REG_8BIT(reg,( (HR_REG_8BIT(reg)) | (value)))
#define H_CLRBIT(reg, value)			HW_REG_8BIT(reg, (HR_REG_8BIT(reg) & (~(value))))
#define H_READBIT(reg, value)			((HR_REG_8BIT(reg)) & (value))
#define H_BIT(x)						(1 << (x))

typedef enum
{
	SUCCESS,
	ERR_DEVICE_CLOSED,
	ERR_ILLEGAL_PARAM,
}error_t;

void _delay_ms(uint16_t ms);
void _assert_handler(const char* func, uint16_t line);

#define _ASSERT(x)	\
	if (!(x))                                                                    \
		{                                                                             \
		_assert_handler(__FUNCTION__, __LINE__);	}

/**
 * @ingroup BasicDef
 *
 * @def _ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. _ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define _ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def _ALIGN_UP(size, align)
 * Return the down number of aligned at specified width. _ALIGN_DOWN(13, 4)
 * would return 16.
 */
#define _ALIGN_UP(size, align)      ((size + 4) & ~((align) - 1))

/**
 * device (I/O) Ids type
 */
enum device_id
{
	Device_Id_UartA = 0,                           /**< character device */
	Device_Id_UartB,
	Device_Id_Spi,
	Device_Id_I2c,
	Device_Id_Pwm,
	Device_Id_Keyscan
};

typedef struct device *device_t;

#define MAX_DEV_NAME_SIZE 20
/**
 * Device structure
 */
struct device
{
	enum device_id id;
	
	list_t list;					/**< device list */

	/* common device interface */
	error_t  (*reinit)(void);
	error_t (*enterlpm)(void);
};

typedef  error_t (*tReinit)(void); 
typedef  error_t (*tEnterlpm)(void); 

#define RB_UPDATE_PTR(p,s,e) ((p) == (e))?((p)=(s)):((p)++)

/********************************************************************************
** Macros to get and put bytes to and from a stream (small Endian format)
*/
#define STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}
#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#define STREAM_TO_UINT24(u32, p) {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) ); (p) += 3;}
#define STREAM_TO_UINT32(u32, p) {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24)); (p) += 4;}

/********************************************************************************
** Macros to get and put bytes to and from a stream (Big Endian format)
*/
#define BE_STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}
#define BE_STREAM_TO_UINT16(u16, p) {u16 = (uint16_t)(((uint16_t)(*(p)) << 8) + (uint16_t)(*((p) + 1))); (p) += 2;}
#define BE_STREAM_TO_UINT24(u32, p) {u32 = (((uint32_t)(*((p) + 2))) + ((uint32_t)(*((p) + 1)) << 8) + ((uint32_t)(*(p)) << 16)); (p) += 3;}
#define BE_STREAM_TO_UINT32(u32, p) {u32 = ((uint32_t)(*((p) + 3)) + ((uint32_t)(*((p) + 2)) << 8) + ((uint32_t)(*((p) + 1)) << 16) + ((uint32_t)(*(p)) << 24)); (p) += 4;}

#endif/*__BC62XX_DEF_H__*/

 
