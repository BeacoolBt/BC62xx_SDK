#include "bc62xx_timer.h"
#include "systick.h"

SYS_TIMER_TYPE *pheader;
SYS_TIMER_TYPE header_Timer;
uint32_t sys_tick_count;
uint8_t  sys_Timer_Check_Flag;

static uint32_t tmpCnt;

static void SYStick_handle(void);


void SYS_TimerExpireDefaultHandle(int params)
{
	// YC_LOG_ERROR("default timer expire !\r\n");
	while(0);
}

void SYS_TimerInit()
{
	//initial timer lists
	pheader = &header_Timer;
	pheader->pNextTimer = NULL;
	pheader->pfExpireCb = SYS_TimerExpireDefaultHandle;
	pheader->mTimerStatus = TIMER_START;
	pheader->mTimerValue = 0;

	SysTick_Config(SYSTEM_CLOCK/1000); //each  systick interrupt is 1ms

	systick_set_timer_irq_handler(SYStick_handle);
}


//union is 10ms
bool SYS_SetTimer(SYS_TIMER_TYPE *pTimer, int tick,TIMER_TYPE isSingle,Timer_Expire_CB pfExpire_CB)
{
	SYS_TIMER_TYPE *pTemp;
	if(pTimer == NULL)
	{
		return false;
	}
	
	if (pfExpire_CB == NULL)
	{
		pTimer->pfExpireCb = SYS_TimerExpireDefaultHandle;
	}
	else
	{
		pTimer->pfExpireCb = pfExpire_CB;
	}
	OS_ENTER_CRITICAL();
	pTimer->mTick = tick;
	pTimer->mTimerValue = sys_tick_count+tick;
	pTimer->mTimerStatus = TIMER_START;
	pTimer->mIsCycle = isSingle;

	pTimer->cbParams = (int32_t)pTimer;
	OS_EXIT_CRITICAL();
	if (SYS_TimerisExist(pTimer))
	{
		//printf("timer exised and set timer success\r\n");
		return true;
	}
	OS_ENTER_CRITICAL();
	//insert to list
	pTemp = pheader;
	pheader = pTimer;
	pheader->pNextTimer = pTemp;
	OS_EXIT_CRITICAL();
	//printf("set timer success\n");
	//YC_LOG_INFO("set timer %d \r\n",pTimer->cbParams);
	return true;
}

bool SYS_TimerisExist(SYS_TIMER_TYPE *pTimer)
{
	SYS_TIMER_TYPE *pTemp;
	if (pTimer == NULL)
	{
		return false;
	}
	for(pTemp = pheader; pTemp->pNextTimer != NULL; pTemp = pTemp->pNextTimer)
	{
		if (pTimer == pTemp)
		{
			return true;
		}
	}
	return false;
}


bool SYS_ResetTimer(SYS_TIMER_TYPE *pTimer)
{
	SYS_TIMER_TYPE *pTemp;
	if (pTimer == NULL)
	{
		return false;
	}
	for(pTemp = pheader; pTemp->pNextTimer != NULL; pTemp = pTemp->pNextTimer)
	{
		if (pTimer == pTemp)
		{
			pTimer->mTimerValue = sys_tick_count+(pTimer->mTick);
			return true;
		}
	}
	return false;
}


void SYS_TimerTest()
{
	SYS_TIMER_TYPE *pTimer;
	int i = 0;
	for (pTimer = pheader; pTimer->pNextTimer != NULL; pTimer = pTimer->pNextTimer)
	{
		i++;
	}
	//YC_LOG_INFO("timer count %d \r\n",i);
}

void SYS_timerPolling()
{
	SYS_TIMER_TYPE *pTimer;
	
	if (sys_Timer_Check_Flag)
	{
		sys_Timer_Check_Flag = false;
		//SYS_TimerTest();
	//	printf("2\n");
		for (pTimer = pheader; pTimer->pNextTimer != NULL; pTimer = pTimer->pNextTimer)
		{
	//		printf("3\n");
			if (pTimer->mTimerStatus == TIMER_START)
			{
		//		printf("sys_tick_count = %d\n",sys_tick_count);
		//		printf("pTimer->mTimerValue = %d\n",pTimer->mTimerValue);
		//		printf("timer %d in\r\n",pTimer->cbParams);
				if (pTimer->mTimerValue <= sys_tick_count)
				{
		//			printf("5\n");
					if (pTimer->mIsCycle == TIMER_SINGLE)
					{
				//		printf("timer release %d in\r\n",pTimer->cbParams);
						SYS_ReleaseTimer(pTimer);
					}
					else
					{					
				//		printf("timer cycle in\r\n");
				//		printf("timer release %d in\r\n",pTimer->cbParams);
						pTimer->mTimerValue = sys_tick_count+(pTimer->mTick);
					}
			//		printf("6\n");
					pTimer->pfExpireCb(pTimer->cbParams);
				}
			}
		}
	}
}

bool SYS_ReleaseTimer(SYS_TIMER_TYPE *pTimer)
{
	SYS_TIMER_TYPE *pTemp, *pPre;
	if (pTimer == NULL)
	{
		return false;
	}

	if (pTimer == &header_Timer)
	{
		return false;
	}

	for(pTemp = pPre = pheader; pTemp->pNextTimer != NULL; pPre = pTemp,pTemp = pTemp->pNextTimer)
	{
		if (pTimer == pTemp)
		{
			pTemp->mTimerStatus = TIMER_STOP;
			if (pPre == pTemp) //delete the first one element
			{
				pheader = pTemp->pNextTimer;
			}
			else
			{
				pPre->pNextTimer = pTemp->pNextTimer;
			}

			//YC_LOG_INFO("release timer %d,%d \r\n",pPre->cbParams,pTimer->cbParams);
			return true;
		}
	}
	return false;
}

void SYS_ReleaseAllTimer()
{
	SYS_TIMER_TYPE *pTemp;
	for(pTemp = pheader; pTemp->pNextTimer != NULL; pTemp = pTemp->pNextTimer)
	{
		pTemp->mTimerStatus = TIMER_STOP;
	}
	pheader = &header_Timer;
}

static void SYStick_handle(void)
{
	if(++tmpCnt%10 == 0){//each	systick interrupt is 10ms
		sys_tick_count++; //10
		sys_Timer_Check_Flag = true;
	}
}

