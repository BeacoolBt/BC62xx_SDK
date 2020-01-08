#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bc62xx.h"

#ifdef RELEASE
#define ASSERT(x) 
#else
#define ASSERT(x) while(!(x))
#endif

#define OS_ENTER_CRITICAL() __disable_irq() 
#define OS_EXIT_CRITICAL() __enable_irq() 


static inline void HW_REG_16BIT(uint32_t reg, uint16_t word)
{
    HW_REG_8BIT(reg, word & 0x00FF);
    hw_delay();
    HW_REG_8BIT(reg + 1,(word >> 8));
}
static inline uint16_t HR_REG_16BIT(uint32_t reg)
{
	uint16_t return_data = 0;
	uint16_t H_data = 0;
	hw_delay();
	return_data = HR_REG_8BIT(reg);
	hw_delay();
	H_data = HR_REG_8BIT(reg + 1);
	return_data = (return_data | ((H_data << 8) & 0xFF00));
	return return_data;
}

static inline uint32_t HR_REG_24BIT(uint32_t reg)
{
	uint32_t return_data = 0;
	hw_delay();
	return_data = HR_REG_8BIT(reg);
	hw_delay();
	return_data = return_data |(HR_REG_8BIT(reg + 1)<<8);
	hw_delay();
	return_data = return_data | (HR_REG_8BIT(reg + 2)<<16);
	
	return return_data;
}

#endif/*__COMMON_H__*/
