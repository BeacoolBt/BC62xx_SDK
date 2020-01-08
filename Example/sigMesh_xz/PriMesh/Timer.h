#ifndef  _TIME_H
#define  _TIME_H

#include "Function.h"
#include "bc62xx_timer.h"
#include "PriMesh.h"

extern uint8_t pucAdvDataTest[];
extern uint32_t ulUnixTime;
uint32_t ulGetUnixTime(void);
uint8_t ucGradualTimerStart(void);
uint8_t ucTimeInit (void);
uint8_t ucTimerCreate (SYS_TIMER_TYPE *pTimer, int tick,TIMER_TYPE isSingle,Timer_Expire_CB pfExpire_CB);
uint8_t ucLEDGradualTimerStart(void);
void vTimerInit (void);
#endif
/**************************************END OF FILE ****************************/
