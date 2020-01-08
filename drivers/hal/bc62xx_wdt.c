#include "bc62xx_wdt.h"

void WDT_Enable(void)
{
	uint8_t config = *((volatile byte*)(0x10008043));
	config |= (1<<2);
	*((volatile byte*)(0x10008043)) = config;
	*((volatile byte*)(0x10008004)) = 0x2;
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