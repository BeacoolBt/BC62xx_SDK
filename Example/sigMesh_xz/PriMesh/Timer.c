#include "Timer.h"
#include "PriMesh.h"
#include "portBle.h"
#include "bc62xx_gpio.h"
#include "Application.h"
#include "bc62xx_pwm.h"
#include "led.h"

uint32_t ulUnixTime = 0;
static uint8_t  ucTimeSecond = 0;
static uint8_t  ucLedPrecessTimerCount = 0;
static uint8_t  ucFlashTimesCount = 0;
SYS_TIMER_TYPE  xMeshAdvTimer;
SYS_TIMER_TYPE  xMeshProcessTimer;
SYS_TIMER_TYPE  xLEDHandleTimer;
SYS_TIMER_TYPE  xLEDGradualTimer;

uint8_t pucAdvDataTest[26] = 
{0x02, 0x01, 0x05, 0x16, 0xFF, 0xFF, 0xFE, 0xFD, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0xC4, 0x06, 0x0A, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, \
 0x00, 0x00, 0x00, 0x00};

/**
 * @brief   系统软件定时器初始化
 * @param
 * @return 
*/
uint8_t ucTimeInit (void)
{
    SYS_TimerInit();
    return RESULT_SUCCESS;
}

/**
 * @brief   创建软件定时器
 * 
 * @param   pTimer       定时器ID
 * @param   isSingle     TIMER_SINGLE - 单次; TIMER_CYCLE - 循环
 * @param   pfExpire_CB  超时回调函数
 * 
 * @return  RESULT_SUCCESS - 创建成功; RESULT_ERROR - 创建失败
*/
uint8_t ucTimerCreate (SYS_TIMER_TYPE *pTimer, int tick,TIMER_TYPE isSingle, Timer_Expire_CB pfExpire_CB)
{
    SYS_SetTimer(pTimer, tick, isSingle, pfExpire_CB);
    return RESULT_SUCCESS;
}

/**
 * @brief   获取32位系统时间戳，单位秒
 * @param   
 * @return  
*/
uint32_t ulGetUnixTime(void)
{
    return ulUnixTime;
}

/**
 * @brief  Mesh广播切换管理，定时器回调Timeout 50ms
 * @param  
 * @return
*/
Timer_Expire_CB vMeshAdvTimeHandler (int scParams)
{
    (void) scParams;                                                                /* Remove warning for unused parameter */
    vMeshAdvSwitchProcess();                                                        /* 广播切换 */
}

/**
 * @brief  Timeout 100ms
 * @param
 * @return
*/
Timer_Expire_CB vMeshProcessTimeHandler (int scParams)
{
    (void) scParams;                                                                /* Remove warning for unused parameter */
    if(++ucTimeSecond >= 10)                                                        /* 1 Second*/
    {
        ucTimeSecond = 0;
        ulUnixTime++;
        /* Flash数据保存 */
        vFlashDataSaveProcess();
    }
}

/**
 * @brief  灯控处理，Timeout 10ms
 * @param
 * @return
*/
Timer_Expire_CB vLEDProcessTimeHandler (int scParams)
{
    (void) scParams;                                                                /* Remove warning for unused parameter */
    static uint8_t ucColorListIndex = 0;

    switch(xLedStatus.eMode)
    {
    case eStatic:/* 静态 */ 
        if(xGradualStatus.eGraStatus == eIDLE) ;
        vLEDGradualProcessSingle(LED_GRADUAL_ONCE);
        break;

    case eBlink:/* 频闪 */ 
        if(++ucLedPrecessTimerCount < xLedStatus.xBlinkParam.ucRate) return;
        ucLedPrecessTimerCount = 0;
        if(xLedStatus.xBlinkParam.ucType)                                           /* 任意颜色频闪 */
        {
            if(ucColorListIndex >= 1)
            {
                ucColorListIndex= 0;
                vLedColorPwmSet(xLedStatus.xColorParam.ucR,
                                xLedStatus.xColorParam.ucG,
                                xLedStatus.xColorParam.ucB);
            }
            else
            {
                ucColorListIndex++;
                vLedColorPwmSet(0,0,0);
            }
        }
        else                                                                        /* 七色频闪 */
        {
            if(ucColorListIndex >= 14) ucColorListIndex = 0;
            vLedColorPwmSet(pucBlinkColor[ucColorListIndex][0],
                            pucBlinkColor[ucColorListIndex][1],
                            pucBlinkColor[ucColorListIndex][2]);
            ucColorListIndex += 1;
        }
        break;
        
    case eHopping: /* 跳变 */ 
        if(++ucLedPrecessTimerCount < xLedStatus.xHoppingParam.ucRate) return;
        ucLedPrecessTimerCount = 0;
        if(xLedStatus.xHoppingParam.ucType)                                         /* 七色跳变 */
        {
            if(++ucColorListIndex >= 7) ucColorListIndex = 0;
            vLedColorPwmSet(pucBlinkColor[2*ucColorListIndex][0],
                            pucBlinkColor[2*ucColorListIndex][1],
                            pucBlinkColor[2*ucColorListIndex][2]);
        }
        else                                                                        /* 三色跳变 */
        {
            if(++ucColorListIndex >= 3) ucColorListIndex = 0;
            vLedColorPwmSet(pucBlinkColor[2*ucColorListIndex][0],
                            pucBlinkColor[2*ucColorListIndex][1],
                            pucBlinkColor[2*ucColorListIndex][2]);
        }
        break;

    case eGradual:/* 渐变 */
        vLEDGradualProcess();
        break;

    default:
        break; 
    }

    /* 手机添加和删除灯的时候灯闪烁3次 */
    ucFlashTimesCount ++;
    if(ucFlashTimesCount  > 30)/* 300ms间隔 */
    {
        ucFlashTimesCount = 0;
        if(ucFlashTimes != 0)
        {
            if((ucFlashTimes % 2) == 0)
                vLedTurnOnoff(LED_TURN_OFF);
            else
                vLedTurnOnoff(LED_TURN_ON);
            
            ucFlashTimes--;
        }
    }
}

/**
 * @brief  Timeout 10ms
 * @param
 * @return
*/
Timer_Expire_CB vLEDGradualTimeHandler (int scParams)
{
    (void) scParams;                                                                /* Remove warning for unused parameter */
    cycle();
}
/**
 * @brief 
 * @param
 * @return
*/
uint8_t ucLEDGradualTimerStart(void)
{
    ucTimerCreate(&xLEDGradualTimer, 1, TIMER_SINGLE, vLEDGradualTimeHandler);
    return RESULT_SUCCESS;
}

/**
 * @brief   定时器初始化，创建软件定时器
 * @param
 * @return
 */
void vTimerInit(void)
{
    /* period = 50ms */
    ucTimerCreate(&xMeshAdvTimer, 5, TIMER_CYCLE, vMeshAdvTimeHandler);
    /* period = 100ms */
    ucTimerCreate(&xMeshProcessTimer, 10, TIMER_CYCLE, vMeshProcessTimeHandler);
    /* period = 10ms */
    ucTimerCreate(&xLEDHandleTimer, 1, TIMER_CYCLE, vLEDProcessTimeHandler);
}
/**************************************END OF FILE ****************************/
