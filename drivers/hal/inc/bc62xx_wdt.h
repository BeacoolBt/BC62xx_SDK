#ifndef __BC62XX_WDT_H__
#define __BC62XX_WDT_H__
#include "common.h"

/**
 *@brief enable watch dog.   
 */ 
void WDT_Enable(void);

/**
 *@brief disable watch dog.   
 */ 
void WDT_Disable(void);

/**
 *@brief feed the dog.   
 */ 
void WDT_Kick(void);

#endif /*__BC62XX_WDT_H__*/