#include "bc62xx_wdt.h"
#define WDT_ENABLE_BIT	(0x01<<2)

void WDT_Enable(void)
{
	uint8_t config = *((volatile byte*)(0x10008043));
	config |= (1<<2);
	*((volatile byte*)(0x10008043)) = config;
	*((volatile byte*)(0x10008004)) = 0x2;
}

/**
 *@brief Get watch dog status.   
 */ 
bool WDT_Is_Enabled(void)
{
	if(*((volatile byte*)(0x10008043))&WDT_ENABLE_BIT){
		return true;
	}
	return false;
}

void WDT_Disable(void)
{
	uint8_t config = *((volatile byte*)(0x10008043));
	config &= (~(1<<2));
	*((volatile byte*)(0x10008043)) = config;
}

void WDT_Kick(void)
{
	*((volatile byte*)(0x10008004)) = 0x2;
}