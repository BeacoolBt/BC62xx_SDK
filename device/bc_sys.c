#ifndef __BC_SYS_H_
#define __BC_SYS_H_

#include "bc62xx.h"
#include "bc_sys.h"

/**
 ****************************************************************************************
 * @brief system reset
 ****************************************************************************************
 */
uint32_t bc_sys_reset(void)
{
	//HWRITE(CORE_RESET, 1);
	HWRITE(CORE_RESET,0x03);
	return 0;
}

uint8_t bc_sys_check_sum(uint8_t* data, int len)
{
	uint8_t crc = 0x00;/* init crc value*/
	if(data == NULL || len == 0){
		return crc;
	}
	do{
		crc += data[--len];
	}while(len);
	return crc;
}

#endif

