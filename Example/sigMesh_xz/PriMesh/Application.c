#include "Application.h"
#include "bc_flash.h"
#include "bc62xx_pwm.h"
#include "bc62xx_flash.h"
#include "led.h"

#define FLASH_START_ADDR         BC_FLASH_START_ADDR + 256                          /* Flash起始地址 */
#define FLASH_PAGE_SIZE          BC_PAGE_SIZE                                       /* Flash页大小 */

/* 该参数需要保存到Flash */
BondID_t xBondPhoneID;

static uint8_t  ucGradualTimerCount = 0;
static int32_t  slCurrentBright = 0;
static uint16_t usGradualRed = 0;
static uint16_t usGradualGreen = 0;
static uint16_t usGradualBlue = 0;
static uint8_t  ucGradualcycleIndex = 1;
static uint8_t  ucFlashSaveFlag = 0;
static uint16_t usLocalVendorID = 0xABCD;
uint8_t ucFlashTimes;

GradualControl_t xGradualStatus = { eIDLE };

/* LED灯运行状态 */
LedStatus_t xLedStatus = 
{
    .eMode = eStatic,
    .xColorParam.ucR = ALI_FREQUENCY_1K,
    .xColorParam.ucG = ALI_FREQUENCY_1K,
    .xColorParam.ucB = ALI_FREQUENCY_1K,
    .xStaticParam.ucBrightness = 0xFF
};

/* 颜色列表 */
const uint16_t pucBlinkColor[14][3] = 
{
    {ALI_FREQUENCY_1K, 0x00, 0x00},                                                 /*0、红色*/
    {0, 0, 0},
    {0x00, ALI_FREQUENCY_1K, 0x00},                                                 /*2、绿色*/
    {0, 0, 0},
    {0x00, 0x00, ALI_FREQUENCY_1K},                                                 /*4、蓝色*/
    {0, 0, 0},
    {ALI_FREQUENCY_1K, ALI_FREQUENCY_1K, 0x00},                                     /*6、黄色*/
    {0, 0, 0},
    {0x00, ALI_FREQUENCY_1K/2, ALI_FREQUENCY_1K},                                   /*8、青色*/
    {0, 0, 0},
    {(0x8B/255.0)*ALI_FREQUENCY_1K, 0x00, ALI_FREQUENCY_1K},                        /*10、紫色*/
    {0, 0, 0},
    {ALI_FREQUENCY_1K, ALI_FREQUENCY_1K, ALI_FREQUENCY_1K},                         /*12、白色*/
    {0, 0, 0}
};

/*******************************************************************************
 *                   Flash操作                                                 *
 ******************************************************************************/
/**
 * @brief  擦除Flsh
 * 
 * @param  ulAddress   Flash起始地址
 * @param  ulPageNum   页数（注意不是页的大小）
 * 
 * @return 0 - success; 1 - failed
 */
uint32_t ulFlashErase(uint32_t ulAddress, uint32_t ulPageNum)
{
    uint32_t ulApiResult;

    ulApiResult = bc_flash_erase(ulAddress, ulPageNum);

    return ulApiResult;
}

/**
 * @brief  写Flash
 * @param
 * @return 0 - success; 1 - failed
 */
uint32_t ulFlashWrite(uint32_t ulAddress, uint32_t ulDlen, uint8_t *pucDataBuf)
{
    uint32_t ulApiResult;

    ulApiResult = bc_flash_write(ulAddress, ulDlen, pucDataBuf);

    return ulApiResult;
}
/**
 * @brief  读Flash
 * @param
 * @return 0 - success; 1 - failed
 */
uint32_t ulFlashRead(uint32_t ulAddress, uint32_t ulDlen, uint8_t *pucDataBuf)
{
    uint32_t ulApiResult;
    
    ulApiResult = bc_flash_read(ulAddress, ulDlen, pucDataBuf);

    return ulApiResult;
}

/**
 * @brief  保存数据到Flash
 * @param  
 * @return 
 */
void vFlashDataSaveProcess(void)
{
    uint32_t ulApiResult;

    if(ucFlashSaveFlag == 0) return;
    /* 写之前先擦除 */
    ulApiResult = ulFlashErase(FLASH_START_ADDR, 1);
    if(ulApiResult == 0)
    {
        ulApiResult = ulFlashWrite(FLASH_START_ADDR, sizeof(BondID_t), (uint8_t *)&xBondPhoneID);
        if(ulApiResult == 1) 
        {
            /*  */
        }
        ucFlashSaveFlag = 0;
    }
}

/**
 * @brief   Flash中加载数据
 * @param
 * @return 
 */
static void prvFlshDataLoad(void)
{
    uint32_t ulApiResult;

    ulApiResult = ulFlashRead(FLASH_START_ADDR, sizeof(BondID_t), (uint8_t *)&xBondPhoneID);
    if(ulApiResult != 0)
    {
        memset(&xBondPhoneID, 0, sizeof(BondID_t));
    }
    if((ucTotalCheck(&xBondPhoneID.ucBondNum, sizeof(BondID_t)-1, 0) != xBondPhoneID.ucValueCheck) ||
       (xBondPhoneID.ucBondNum > APP_LED_BOND_ID_NUM))
    {
        memset(&xBondPhoneID, 0, sizeof(BondID_t));
    }
#if configUSE_UART && configUART_LOG_FLASH_READ
    printf("\r\n");
    printf("Flash read. ulApiResult = %d\r\n", ulApiResult);
    printf("BondNum -> %d\r\n", xBondPhoneID.ucBondNum);
    for(uint8_t i = 0; i < APP_LED_BOND_ID_NUM; i++)
    {
        printf("%02d -> %04X , %d\r\n", i, xBondPhoneID.pxBondID[i].ulID, xBondPhoneID.pxBondID[i].ucGrpupID);
    }
#endif
}

/*******************************************************************************
 *                    灯控制                                                   *
 ******************************************************************************/
/**
 * @brief   检查设备ID和组ID是否在绑定列表
 * @param   ucCMD  0x05：不检查组ID；0x03：检查组ID
 * @return  0：不在绑定列表；1：在绑定列表
 */
uint8_t ucDeviceIDAndGroupIDCheck(uint32_t ulRxID, uint8_t ucGroupID, uint8_t ucCMD)
{
    /* 没有绑定任何设备，不处理指令 */
    if(xBondPhoneID.ucBondNum == 0) return 0;

    for(uint8_t i = 0; i < APP_LED_BOND_ID_NUM; i++)
    {
        if(xBondPhoneID.pxBondID[i].ucValid == 0) continue;

        if(xBondPhoneID.pxBondID[i].ulID == ulRxID) 
        {
            /* 全部控制时不需要判断组ID */
            if(ucCMD == 0x05) return 1;
            if((ucCMD == 0x03)&&(ucGroupID == xBondPhoneID.pxBondID[i].ucGrpupID)) return 1;
        }
    }
    return 0;
}

/**
 * @brief   加灯（绑定手机）指令处理，添加成功后灯会返回提示信息，
 *          厂商ID(2byte) + 灯的编号(3byte) + 当前匹配的总数量(1byte) + 组号(1byte)
 * @param   ulPhoneID：手机的设备ID；ucGroupID：手机给灯分配的组ID
 * @return
 */
static void prvBondLEDProcess(uint32_t ulPhoneID, uint8_t ucGroupID)
{
    /* 检查是否已绑定 */
    for(uint8_t i = 0; i < APP_LED_BOND_ID_NUM; i++)
    {
        if(xBondPhoneID.pxBondID[i].ucValid == 0) continue;
        if(xBondPhoneID.pxBondID[i].ulID == ulPhoneID) return;
    }
    /* 绑定 */ 
    for(uint8_t i = 0; i < APP_LED_BOND_ID_NUM; i++)
    {
        if(xBondPhoneID.pxBondID[i].ucValid == 1) continue;
        
        xBondPhoneID.pxBondID[i].ucValid = 1;
        xBondPhoneID.pxBondID[i].ulID = ulPhoneID;
        xBondPhoneID.pxBondID[i].ucGrpupID = ucGroupID;
        xBondPhoneID.ucBondNum++;
        xBondPhoneID.ucValueCheck = ucTotalCheck(&xBondPhoneID.ucBondNum, sizeof(BondID_t)-1, 0);
        /* 响应数据 */
        uint8_t  pucAdvData[9]={0x08, 0xFF, 0x18, 0x14};
        memcpy(&pucAdvData[4], &pucMacAddr[3], 3);                                      /* 灯编号为MAC地址后三字节 */
        pucAdvData[7] = xBondPhoneID.ucBondNum;                                         /* 匹配总数量 */
        pucAdvData[8] = xBondPhoneID.pxBondID[i].ucGrpupID;                             /* 组号 */
        ucMeshDataQueueSend(ePriorityHigh, pucAdvData, 9);

        ucFlashSaveFlag = 1;                                                            /* 需要保存 */
        ucFlashTimes = 6;
    #if configUSE_UART && configUART_LOG_LED_ADD
        printf("Log - Add led success.\r\n"); 
    #endif
        return;
    }
}

/**
 * @brief   删除设备ID（解绑）指令处理,灯会从绑定列表中删除该设备ID，且不再接收该ID发送的控灯指令，除非再次绑定
 * @param
 * @return
 */
static void prvBondDeleteProcess(uint32_t ulID)
{
    for(uint8_t i = 0; i < APP_LED_BOND_ID_NUM; i++)
    {
        if(xBondPhoneID.pxBondID[i].ucValid == 0) continue;
        if(xBondPhoneID.pxBondID[i].ulID == ulID)
        {
            xBondPhoneID.pxBondID[i].ucValid = 0;
            xBondPhoneID.pxBondID[i].ulID = 0;
            xBondPhoneID.pxBondID[i].ucGrpupID = 0;
            xBondPhoneID.ucBondNum--;
            xBondPhoneID.ucValueCheck = ucTotalCheck(&xBondPhoneID.ucBondNum, sizeof(BondID_t)-1, 0);

            ucFlashSaveFlag = 1;
            ucFlashTimes = 6;
        #if configUSE_UART &&configUART_LOGLED_DEVICE_ID_CLEAN
            printf("Log - Led device id clean success.\r\n");
        #endif
            return;
        }
    }
}

/**
 * @brief   R、G、B三路PWM初始化，频率1kHz，周期1ms，计数为24000，默认占空比为50%
 * @param
 * @return
 */
static void prvRGBPwmInit(void)
{  
    struct PWM_CTRL_BITS xBits = {0};

    /* 红 */
    GPIO_CFG cfg1 ={FUNC_PWM_OUT1, GFG_PULLUP};
	bc_gpio_init(GPIO_PWM1, &cfg1);
	PWM_Init(PWM_CH1, &xBits);
    PWM_SetPnCnt(PWM_CH1, ALI_FREQUENCY_1K - ALI_FREQUENCY_1K/2,ALI_FREQUENCY_1K/2);
    /* 绿 */
    GPIO_CFG cfg2 ={FUNC_PWM_OUT2, GFG_PULLUP};
	bc_gpio_init(GPIO_PWM2, &cfg2);
	PWM_Init(PWM_CH2, &xBits);
    PWM_SetPnCnt(PWM_CH2, ALI_FREQUENCY_1K/2 - ALI_FREQUENCY_1K/2, ALI_FREQUENCY_1K/2);
    /* 蓝 */
    GPIO_CFG cfg3 ={FUNC_PWM_OUT3, GFG_PULLUP};
	bc_gpio_init(GPIO_PWM3, &cfg3);
	PWM_Init(PWM_CH3, &xBits);
    PWM_SetPnCnt(PWM_CH3, ALI_FREQUENCY_1K/2 - ALI_FREQUENCY_1K/2, ALI_FREQUENCY_1K/2);
    /* 启动PWM */
    PWM_Start(PWM_CH1);
    PWM_Start(PWM_CH2);
    PWM_Start(PWM_CH3);
}

/**
 * @brief   设置灯的RGB颜色
 * @param
 * @return
 */
void vRGBColorSet (uint8_t ucBright,  uint8_t ucRedPer,  uint8_t ucGreenPer, 
                   uint8_t ucBluePer, uint8_t ucReserve, uint8_t ucModeType)
{
    //uint8_t ucRed   = (ucBright*1.0 / 255) * ucRedPer;
    uint16_t ucRedValue = (ucRedPer*1.0 / 255) * ALI_FREQUENCY_1K;

    //uint8_t ucGreen = (ucBright*1.0 / 255) * ucGreenPer;
    uint16_t ucGreenValue = (ucGreenPer*1.0 / 255) * ALI_FREQUENCY_1K;

    //uint8_t ucBlue  = (ucBright*1.0 / 255) * ucBluePer;
    uint16_t ucBlueValue = (ucBluePer*1.0 / 255) * ALI_FREQUENCY_1K;

    switch(ucModeType)
    {
    case 0x01:                                                                  /* RGB渐变  */
        vLedColorPwmSet(ucRedValue, ucGreenValue, ucBlueValue);
        break;
    case 0x02:                                                                  /* RGB立即（百分比） */
        vLedColorPwmSet(ucRedValue, ucGreenValue, ucBlueValue);
        break;
    case 0x03:                                                                  /* RGB立即（无百分比） */
        vLedColorPwmSet(ucRedValue, ucGreenValue, ucBlueValue);
        break;
    default:
        break;
    }
}

/**
 * @brief  
 * @param
 * @return
 */
static void prvGradualPreSet(void)
{
    switch(xLedStatus.xGradualParam.ucType)
    {
    case 0x00:                                                                      /* 红色呼吸 */
        vLedColorPwmSet(pucBlinkColor[0][0],
                        pucBlinkColor[0][1],
                        pucBlinkColor[0][2]);
        break;
        
    case 0x01:                                                                      /* 黄色呼吸 */
        vLedColorPwmSet(pucBlinkColor[6][0],
                        pucBlinkColor[6][1],
                        pucBlinkColor[6][2]);
        break;

    case 0x02:                                                                      /* 绿色呼吸 */
        vLedColorPwmSet(pucBlinkColor[2][0],
                        pucBlinkColor[2][1],
                        pucBlinkColor[2][2]);
        break;

    case 0x03:                                                                      /* 青色呼吸 */
        vLedColorPwmSet(pucBlinkColor[8][0],
                        pucBlinkColor[8][1],
                        pucBlinkColor[8][2]);
        break;

    case 0x04:                                                                      /* 蓝色呼吸 */
        vLedColorPwmSet(pucBlinkColor[4][0],
                        pucBlinkColor[4][1],
                        pucBlinkColor[4][2]);
        break;

    case 0x05:                                                                      /* 紫色呼吸 */
        vLedColorPwmSet(pucBlinkColor[10][0],
                        pucBlinkColor[10][1],
                        pucBlinkColor[10][2]);
        break;

    case 0x06:                                                                      /* 白色呼吸 */
        vLedColorPwmSet(pucBlinkColor[12][0],
                        pucBlinkColor[12][1],
                        pucBlinkColor[12][2]);
        break;
    case 0x07:                                                                      /* 红绿渐变 */
        /* 设置到最亮 */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);

        vLedColorPwmSet(pucBlinkColor[0][0],
                        pucBlinkColor[0][1],
                        pucBlinkColor[0][2]);
        xGradualStatus.usGradualStep = xLedStatus.xGradualParam.usStep;
        xGradualStatus.usRedStart = ALI_FREQUENCY_1K;
        xGradualStatus.usGreenStart = 0;
        xGradualStatus.usBlueStart = 0;

        xGradualStatus.usRedTarget = 0;
        xGradualStatus.usGreenTarget = ALI_FREQUENCY_1K;
        xGradualStatus.usBlueTarget = 0;
        break;

    case 0x08:                                                                      /* 红蓝渐变 */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);
        xGradualStatus.usGradualStep = xLedStatus.xGradualParam.usStep;
        vLedColorPwmSet(pucBlinkColor[0][0],
                        pucBlinkColor[0][1],
                        pucBlinkColor[0][2]);

        xGradualStatus.usRedStart = ALI_FREQUENCY_1K;
        xGradualStatus.usGreenStart = 0;
        xGradualStatus.usBlueStart = 0;

        xGradualStatus.usRedTarget = 0;
        xGradualStatus.usGreenTarget = 0;
        xGradualStatus.usBlueTarget = ALI_FREQUENCY_1K;
        break;

    case 0x09:                                                                      /* 绿蓝渐变 */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);
        xGradualStatus.usGradualStep = xLedStatus.xGradualParam.usStep;
        vLedColorPwmSet(pucBlinkColor[2][0],
                        pucBlinkColor[2][1],
                        pucBlinkColor[2][2]);

        xGradualStatus.usRedStart = 0;
        xGradualStatus.usGreenStart = ALI_FREQUENCY_1K;
        xGradualStatus.usBlueStart = 0;

        xGradualStatus.usRedTarget = 0;
        xGradualStatus.usGreenTarget = 0;
        xGradualStatus.usBlueTarget = ALI_FREQUENCY_1K;
        break;

    case 0x0A:                                                                      /* 三色渐变 */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);
        
        ucGradualcycleIndex = 1;
        xGradualStatus.usGradualStep = xLedStatus.xGradualParam.usStep;
        /* 先设置红色 */
        vLedColorPwmSet(pucBlinkColor[0][0],
                        pucBlinkColor[0][1],
                        pucBlinkColor[0][2]);
        break;

    case 0x0B:                                                                      /* 七色渐变 */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);
        
        ucGradualcycleIndex = 1;
        xGradualStatus.usGradualStep = xLedStatus.xGradualParam.usStep;
        /* 先设置红色 */
        vLedColorPwmSet(pucBlinkColor[0][0],
                        pucBlinkColor[0][1],
                        pucBlinkColor[0][2]);
        break;
        
    default:
            break;
    }
}

/**
 * @brief   灯循环渐变处理, timeout: 10ms
 * @param
 * @return
 */
void vLEDGradualProcess(void)
{
    switch(xLedStatus.xGradualParam.ucType)
    {
    case 0x00: /* 红色呼吸 */
    case 0x01: /* 黄色呼吸 */
    case 0x02: /* 绿色呼吸 */
    case 0x03: /* 青色呼吸 */
    case 0x04: /* 蓝色呼吸 */
    case 0x05: /* 紫色呼吸 */
    case 0x06: /* 白色呼吸 */
        if (slCurrentBright < xGradualStatus.usBrightTarget) 
        {
            slCurrentBright += xLedStatus.xGradualParam.usStep;
            if(slCurrentBright >= xGradualStatus.usBrightTarget)  
            {
                slCurrentBright = xGradualStatus.usBrightTarget;
                vLedLightnessSet(slCurrentBright);

                xGradualStatus.usBrightTarget = xLedStatus.xGradualParam.usMinBrightness;
            }
            vLedLightnessSet(slCurrentBright);
        }
        else if (slCurrentBright > xGradualStatus.usBrightTarget) 
        {
            slCurrentBright -= xLedStatus.xGradualParam.usStep;

            if(slCurrentBright <= xGradualStatus.usBrightTarget)  
            {
                slCurrentBright = xGradualStatus.usBrightTarget;
                vLedLightnessSet(slCurrentBright);

                xGradualStatus.usBrightTarget = xLedStatus.xGradualParam.usMaxBrightness;
            }
            vLedLightnessSet(slCurrentBright);
        }
        break;

    case 0x07:                                                                      /* 红绿渐变 */
    case 0x08:                                                                      /* 红蓝渐变 */
    case 0x09:                                                                      /* 绿蓝渐变 */
        vLEDGradualProcessSingle(LED_GRADUAL_CYCLE);
        break;
        
    case 0x0A:                                                                      /* 三色渐变 , interval, 10ms */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);

        if(xGradualStatus.eGraStatus == eIDLE)
        {
            xGradualStatus.eGraStatus = eWorking;

            xGradualStatus.usRedTarget   = pucBlinkColor[ucGradualcycleIndex*2][0];
            xGradualStatus.usGreenTarget = pucBlinkColor[ucGradualcycleIndex*2][1];
            xGradualStatus.usBlueTarget  = pucBlinkColor[ucGradualcycleIndex*2][2];

            if(++ucGradualcycleIndex >= 3)  ucGradualcycleIndex = 0;

            ucLEDGradualTimerStart();
        }
        break;

    case 0x0B:                                                                      /* 七色渐变, interval, 10ms */
        vLedLightnessSet(xLedStatus.xGradualParam.usMaxBrightness);
        if(xGradualStatus.eGraStatus == eIDLE)
        {
            xGradualStatus.eGraStatus = eWorking;

            xGradualStatus.usRedTarget   = pucBlinkColor[ucGradualcycleIndex*2][0];
            xGradualStatus.usGreenTarget = pucBlinkColor[ucGradualcycleIndex*2][1];
            xGradualStatus.usBlueTarget  = pucBlinkColor[ucGradualcycleIndex*2][2];

            if(++ucGradualcycleIndex >= 7)  ucGradualcycleIndex = 0;
        
            ucLEDGradualTimerStart();
            
        }
        break;

    default:
            break;
    }
}

/**
 * @brief   灯颜色渐变处理,单次
 * @param
 * @return
 */
void vLEDGradualProcessSingle(uint8_t ucType)
{
    uint16_t ucStep = 0;
    uint16_t usRedCnt;
    uint16_t usGreenCnt; 
    uint16_t usBlueCnt;

    vPwmLowLevelCntGet(1, &usRedCnt);
    vPwmLowLevelCntGet(2, &usGreenCnt);
    vPwmLowLevelCntGet(3, &usBlueCnt);

    /* 红 */
    if (usRedCnt < xGradualStatus.usRedTarget) 
    {
        ucStep = xGradualStatus.usRedTarget - usRedCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usRedCnt += ucStep;
    }
    else if (usRedCnt > xGradualStatus.usRedTarget) 
    {
        ucStep = usRedCnt - xGradualStatus.usRedTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usRedCnt -= ucStep;
    }
    /* 绿 */
    if (usGreenCnt < xGradualStatus.usGreenTarget) 
    {
        ucStep = xGradualStatus.usGreenTarget - usGreenCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usGreenCnt += ucStep;
    }
    else if (usGreenCnt > xGradualStatus.usGreenTarget) 
    {
        ucStep = usGreenCnt - xGradualStatus.usGreenTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usGreenCnt -= ucStep;
    }

    /* 蓝 */
    if (usBlueCnt < xGradualStatus.usBlueTarget) 
    {
        ucStep = xGradualStatus.usBlueTarget - usBlueCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usBlueCnt += ucStep;
    }
    else if (usBlueCnt > xGradualStatus.usBlueTarget) 
    {
        ucStep = usBlueCnt - xGradualStatus.usBlueTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usBlueCnt -= ucStep;
    }

    vLedColorPwmSet(usRedCnt, usGreenCnt, usBlueCnt);
    
    if((usRedCnt == xGradualStatus.usRedTarget) && 
       (usGreenCnt == xGradualStatus.usGreenTarget) && 
       (usBlueCnt == xGradualStatus.usBlueTarget))
    {
        if(ucType == LED_GRADUAL_CYCLE)
        {
            uint16_t usColorTemp;
            usColorTemp = xGradualStatus.usRedStart;
            xGradualStatus.usRedStart = xGradualStatus.usRedTarget;
            xGradualStatus.usRedTarget = usColorTemp;

            usColorTemp = xGradualStatus.usGreenStart;
            xGradualStatus.usGreenStart = xGradualStatus.usGreenTarget;
            xGradualStatus.usGreenTarget = usColorTemp;

            usColorTemp = xGradualStatus.usBlueStart;
            xGradualStatus.usBlueStart = xGradualStatus.usBlueTarget;
            xGradualStatus.usBlueTarget = usColorTemp;
            ucLEDGradualTimerStart();
        }
        else
        {
           xGradualStatus.eGraStatus = eIDLE;
        }
    }
    else
    {
        ucLEDGradualTimerStart();
    }
}

/**
 * @brief   
 * @param
 * @return
 */
void cycle(void)
{
    uint16_t ucStep = 0;
    uint16_t usRedCnt, usGreenCnt, usBlueCnt;

    vPwmLowLevelCntGet(1, &usRedCnt);
    vPwmLowLevelCntGet(2, &usGreenCnt);
    vPwmLowLevelCntGet(3, &usBlueCnt);

    /* 红 */
    if (usRedCnt < xGradualStatus.usRedTarget) 
    {
        ucStep = xGradualStatus.usRedTarget - usRedCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usRedCnt += ucStep;
    }
    else if (usRedCnt > xGradualStatus.usRedTarget) 
    {
        ucStep = usRedCnt - xGradualStatus.usRedTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usRedCnt -= ucStep;
    }
    /* 绿 */
    if (usGreenCnt < xGradualStatus.usGreenTarget) 
    {
        ucStep = xGradualStatus.usGreenTarget - usGreenCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usGreenCnt += ucStep;
    }
    else if (usGreenCnt > xGradualStatus.usGreenTarget) 
    {
        ucStep = usGreenCnt - xGradualStatus.usGreenTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usGreenCnt -= ucStep;
    }
    /* 蓝 */
    if (usBlueCnt < xGradualStatus.usBlueTarget) 
    {
        ucStep = xGradualStatus.usBlueTarget - usBlueCnt;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usBlueCnt += ucStep;
    }
    else if (usBlueCnt > xGradualStatus.usBlueTarget) 
    {
        ucStep = usBlueCnt - xGradualStatus.usBlueTarget;
        if(ucStep > xGradualStatus.usGradualStep)  ucStep = xGradualStatus.usGradualStep;
        usBlueCnt -= ucStep;
    }

    vLedColorPwmSet(usRedCnt, usGreenCnt, usBlueCnt);
    
    if((usRedCnt == xGradualStatus.usRedTarget) && 
       (usGreenCnt == xGradualStatus.usGreenTarget) && 
       (usBlueCnt == xGradualStatus.usBlueTarget))
    {
        xGradualStatus.eGraStatus = eIDLE;
    } 
    else
    {
        ucLEDGradualTimerStart();
    }
}

/**
 * @brief   获取PWM当前低电平的占空比（计数值）
 * @param   ucPwmChannel：PWM通道（1-3）
 * @return
 */
void vPwmLowLevelCntGet(uint8_t ucPwmChannel, uint16_t *usCnt)
{
    switch(ucPwmChannel)
    {
    case 0x01:
        *usCnt = HR_REG_16BIT(reg_map(CORE_PWM1_NCNT));
        //*usCnt = HR_REG_16BIT(reg_map(CORE_PWM1_PCNT));
        break;
    case 0x02:
        *usCnt = HR_REG_16BIT(reg_map(CORE_PWM2_NCNT));
        //*usCnt = HR_REG_16BIT(reg_map(CORE_PWM2_PCNT));
        break;
    case 0x03:
        *usCnt = HR_REG_16BIT(reg_map(CORE_PWM3_NCNT));
        //*usCnt = HR_REG_16BIT(reg_map(CORE_PWM3_PCNT));
        break;
    default:
        break;
    }
}

/**
 * @brief  LED红、绿、蓝PWM设置（低电平占空比设置）
 * @param
 * @return
 */
void vLedColorPwmSet(uint16_t usRed, uint16_t usGreen, uint16_t usBlue)
{
    PWM_SetPnCnt(PWM_CH1, ALI_FREQUENCY_1K - usRed,   usRed);
    PWM_SetPnCnt(PWM_CH2, ALI_FREQUENCY_1K - usGreen, usGreen);
    PWM_SetPnCnt(PWM_CH3, ALI_FREQUENCY_1K - usBlue,  usBlue);
}

/**
 * @brief  设置灯的亮度
 * @param  usLightness  亮度值（0-65535）
 * @return
 */
void vLedLightnessSet(uint16_t usLightness)
{
    led_cw_set_lightness(usLightness,NULL);
}

/**
 * @brief   开关灯
 * @param   
 * @return
 */
void vLedTurnOnoff(uint8_t ucSwitch)
{
    led_cw_set_on_off(ucSwitch,NULL);
}

/**
 * @brief   Mesh广播数据解析
 * @param   
 * @return
 */
void vAdvDataParse(uint8_t *pucData, uint16_t usDlen)
{
    if(usDlen < 16) return;                                                         /* Mesh包头共14字节 */

    if((pucData[2] != 0x4C) || (pucData[3] != 0x00)) return;                        /* 前两字节固定是0x4C00 */
    pucData += 4;

    uint16_t usVendorID  = (pucData[0] << 8)  | pucData[1];                         /* 1、厂商ID */
    if(usVendorID != usLocalVendorID)  return;
    
    uint32_t ulDeviceID  = (pucData[2] << 16) | (pucData[3] << 8) | pucData[4];     /* 2、设备ID(手机的ID)*/
    uint8_t  ucReserve   = pucData[5];                                              /* 3、保留 */
    uint8_t  ucEventID   = pucData[6];                                              /* 4、事件ID */
    uint8_t  ucCmdType   = pucData[7];                                              /* 5、指令类型 */
    uint8_t  ucGroupID   = pucData[8];                                              /* 6、组ID */
    uint32_t ulLedNumber = (pucData[9] << 16) | (pucData[10] << 8) | pucData[11];   /* 7、灯编号 */
    uint8_t  *pucUserData = pucData + 12;                                           /* 8、用户自定义参数 */

    /* 事件ID判断，新的Mesh消息应该有新的事件ID */
    if(vEventIDCheck(ucEventID)) return;

#if configUSE_UART && configUART_LOG_SCAN_DATA
    printf("Valid_Mesh_Data -> "); 
    for(uint8_t i = 0; i < usDlen; i++)
        printf("%02X ",(pucData-4)[i]); 
    printf("\r\n");
    printf("VendorID -> %04X\r\n",usVendorID); 
    printf("DeviceID -> %04X\r\n",ulDeviceID); 
    printf("Reserve  -> %02X\r\n",ucReserve);
    printf("EventID  -> %02X\r\n",ucEventID);
    printf("CmdType  -> %02X\r\n",ucCmdType);
    printf("GroupID  -> %02X\r\n",ucGroupID);
    printf("LedNumber-> %02X\r\n",ulLedNumber);
    printf("UserData -> %02X\r\n",pucUserData[0]);
#endif
    switch(pucUserData[0])
    {
    case APP_LED_ADD:                                                               /* 加灯（绑定手机）*/
        prvBondLEDProcess(ulDeviceID, ucGroupID);
        break;

    case APP_LED_TURN_OFF:                                                          /* 关灯*/
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;
        vLedTurnOnoff(LED_TURN_OFF);
    #if configUSE_UART && configUART_LOG_LED_TURN_OFF
        printf("Log - Led turn off success.\r\n");
    #endif
        break;

    case APP_LED_TURN_ON:                                                           /* 开灯*/
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;
        vLedTurnOnoff(LED_TURN_ON);
    #if configUSE_UART && configUART_LOG_LED_TURN_ON
        printf("Log - Led turn on success.\r\n");
    #endif
        break;

    case APP_LED_DEVICE_ID_CLEAN:                                                   /* 清除设备ID*/
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;
        prvBondDeleteProcess(ulDeviceID);
        break;

    case APP_LED_RGB_SET:                                                           /* （RGB PWM计算待优化）设置RGB颜色 */
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;
        xLedStatus.eMode = eStatic;
        /* 亮度 */
        xLedStatus.xStaticParam.ucBrightness = pucUserData[1] * 257;
        /* RGB颜色 */
        xLedStatus.xColorParam.ucR = (pucUserData[2] / 255.0) * ALI_FREQUENCY_1K;
        xLedStatus.xColorParam.ucG = (pucUserData[3] / 255.0) * ALI_FREQUENCY_1K;
        xLedStatus.xColorParam.ucB = (pucUserData[4] / 255.0) * ALI_FREQUENCY_1K;
        /* 模式: 1、渐变到设定颜色；2、立即变到设定颜色；3、和2相同但亮度保留*/
        xLedStatus.xStaticParam.ucType = pucUserData[6];
        
        switch(pucUserData[6])
        {
        case 0x01:
            xGradualStatus.usRedTarget = xLedStatus.xColorParam.ucR;
            xGradualStatus.usGreenTarget = xLedStatus.xColorParam.ucG;
            xGradualStatus.usBlueTarget = xLedStatus.xColorParam.ucB;
            /* 设置亮度 */
            vLedLightnessSet(xLedStatus.xStaticParam.ucBrightness);
            /* 开启渐变 */
            xGradualStatus.usGradualStep = 240;
            xGradualStatus.eGraStatus = eWorking;
            break;

        case 0x02:
            /* 设置亮度 */
            vLedLightnessSet(xLedStatus.xStaticParam.ucBrightness);
            /* 立即跳变 */
            vLedColorPwmSet(xLedStatus.xColorParam.ucR,
                            xLedStatus.xColorParam.ucG,
                            xLedStatus.xColorParam.ucB);
            break;

        case 0x03:
            /* 立即跳变 */
            vLedColorPwmSet(xLedStatus.xColorParam.ucR,
                            xLedStatus.xColorParam.ucG,
                            xLedStatus.xColorParam.ucB);
            break;
            
        default:
            return;
        }
    #if configUSE_UART && configUART_LOG_LED_RGB_SET
        printf("Log - Led set RGB seuccess.\r\n");
    #endif
        break;

    case APP_LED_BLINK:                                                             /* 频闪 */
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;
        /* 灯模式*/
        xLedStatus.eMode = eBlink;
        /* 闪烁速度 */
        if(pucUserData[1] == 1)
            /* 200ms */
            xLedStatus.xBlinkParam.ucRate = 20;
        else if(pucUserData[1] == 2)
            /* 300ms */
            xLedStatus.xBlinkParam.ucRate = 30;
        else
            /* 500ms */
            xLedStatus.xBlinkParam.ucRate = 50;
        
        /* 闪烁类型，0-七色频闪；1-任意颜色频闪 */
        xLedStatus.xBlinkParam.ucType = pucUserData[2];
        
        if(xLedStatus.xBlinkParam.ucType)
        {
            xLedStatus.xColorParam.ucR = (pucUserData[3] / 255.0) * ALI_FREQUENCY_1K;
            xLedStatus.xColorParam.ucG = (pucUserData[4] / 255.0) * ALI_FREQUENCY_1K;
            xLedStatus.xColorParam.ucB = (pucUserData[5] / 255.0) * ALI_FREQUENCY_1K;
        }
    #if configUSE_UART && configUART_LOG_LED_BLINK
        printf("Log - Led blink success.\r\n");
    #endif
        break;

    case APP_LED_COLOR_HOPPING:                                                     /* 颜色跳变 */
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;

        xLedStatus.eMode = eHopping;
        /* 跳变速度 */
        if(pucUserData[1] == 1)
            /* 200ms */
            xLedStatus.xHoppingParam.ucRate = 20;
        else if(pucUserData[1] == 2)
            /* 300ms */
            xLedStatus.xHoppingParam.ucRate = 30;
        else
            /* 500ms */
            xLedStatus.xHoppingParam.ucRate = 50;
        /* 跳变类型，0-三色跳变；1-七色跳变 */
        xLedStatus.xHoppingParam.ucType = pucUserData[2];
    #if configUSE_UART && configUART_LOG_LED_COLOR_HOPPING
        printf("Log - Led color hopping success.\r\n");
    #endif
        break;

    case APP_LED_COLOR_GRADUAL:                                                     /* 循环渐变 */
        if(ucDeviceIDAndGroupIDCheck(ulDeviceID,ucGroupID,ucCmdType) == 0)  return;

        xLedStatus.eMode = eGradual;
        /* 渐变速度 */
        if(pucUserData[1] == 1)
            xLedStatus.xGradualParam.ucRate = 20;   /* 200ms 这个值是随意设定的，根据实际效果调动*/
        else if(pucUserData[1] == 2)
            xLedStatus.xGradualParam.ucRate = 50;
        else
            xLedStatus.xGradualParam.ucRate = 100;

        /* 该字节应该固定为0（参考协议文档）*/
        xLedStatus.xGradualParam.ucParam = pucUserData[2];
        /* 渐变类型 */
        xLedStatus.xGradualParam.ucType = pucUserData[3];
        /* 渐变最小、最大亮度 */
        xLedStatus.xGradualParam.usMinBrightness = pucUserData[4] * 257;
        xLedStatus.xGradualParam.usMaxBrightness = pucUserData[5] * 257;

        /* 根据最大最小亮度和速度计算渐变步长 */
        if(xLedStatus.xGradualParam.ucType <= 0x06)
        {
            xGradualStatus.usBrightTarget = xLedStatus.xGradualParam.usMaxBrightness;
            /* 初始亮度 */
            slCurrentBright = xLedStatus.xGradualParam.usMinBrightness;

            vLedLightnessSet(slCurrentBright);
            
            xLedStatus.xGradualParam.usStep = ((pucUserData[5] / 2.55)*655 - (pucUserData[4] / 2.55)*655) / xLedStatus.xGradualParam.ucRate;
        }
        else
        {
            if(pucUserData[1] == 1)
                xLedStatus.xGradualParam.ucRate = 50;   /* 500ms 这个值是随意设定的，根据实际效果调动*/
            else if(pucUserData[1] == 2)
                xLedStatus.xGradualParam.ucRate = 100;
            else
                xLedStatus.xGradualParam.ucRate = 500;
            xLedStatus.xGradualParam.usStep = ALI_FREQUENCY_1K / xLedStatus.xGradualParam.ucRate;
        }
        prvGradualPreSet();
    #if configUSE_UART && configUART_LOG_LED_COLOR_GRADUAL
        printf("Log - Led color gradual success.\r\n");
    #endif
    break;
    
    default:
        break;
    }
    
    /* Mesh广播包转发 */
    ucMeshDataQueueSend(ePriorityNormal, pucData - 4, usDlen);
}

/**
 * @brief   应用初始化
 * @param
 * @return
 */
void vApplicationInit(void)
{ 
    /* R、G、B三路PWM初始化 */
    prvRGBPwmInit();
    /* Flash数据加载 */
    prvFlshDataLoad();
}
/**************************************END OF FILE ****************************/
