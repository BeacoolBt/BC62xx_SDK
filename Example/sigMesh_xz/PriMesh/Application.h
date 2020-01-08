#ifndef _APPLICATION_H
#define _APPLICATION_H
#include "Function.h"
#include "PriMesh.h"
#include "bc62xx_gpio.h"

#define APP_LED_BOND_ID_NUM             ( 10u )

#define LED_TURN_ON                     ( 1u )
#define LED_TURN_OFF                    ( 0u )

#define LED_GRADUAL_ONCE                ( 0u )
#define LED_GRADUAL_CYCLE               ( 1u )

#define LED_ADC		                    ( GPIO_ADC )
#define LED_PWM_RED                     ( GPIO_PWM1 )
#define LED_PWM_GREEN	                ( GPIO_PWM2 )
#define LED_PWM_BLUE	                ( GPIO_PWM3 )
 
#define APP_LED_CONTROL_ALL             ( 0x05u )                                   /* 全部控制 */
#define APP_LED_CONTROL_GROUP           ( 0x03u )                                   /* 群组控制 */
#define APP_LED_ADD                     ( 0x70u )                                   /* 加灯 */
#define APP_LED_TURN_OFF                ( 0x7Du )                                   /* 关灯 */
#define APP_LED_TURN_ON                 ( 0x76u )                                   /* 开灯 */
#define APP_LED_DEVICE_ID_CLEAN         ( 0x0Au )                                   /* 清楚设备ID */
#define APP_LED_RGB_SET                 ( 0x01u )                                   /* 设置RGB颜色 */
#define APP_LED_BLINK                   ( 0x80u )                                   /* 灯闪烁 */
#define APP_LED_COLOR_HOPPING           ( 0x81u )                                   /* 颜色跳变 */
#define APP_LED_COLOR_GRADUAL           ( 0x82u )                                   /* 颜色渐变 */

typedef struct 
{
    uint16_t usVendorID;                                                            /* 厂商ID */
    uint32_t ulDeviceID;                                                            /* 设备ID */
    uint8_t  ucGroupID;                                                             /* 组ID */
    uint32_t ulLedNumber;                                                           /* 灯编号 */
}MeshParam_t;

typedef enum
{
    eStatic = 0,
    eBlink,
    eHopping,
    eGradual
}LedMode_t;

typedef struct 
{
    uint16_t ucR;                                                                   /* 红色比例 */
    uint16_t ucG;                                                                   /* 绿色比例 */
    uint16_t ucB;                                                                   /* 蓝色比例 */
}Color_t;

typedef struct 
{
    uint16_t ucBrightness;                                                          /* 静态亮度 */
    uint8_t  ucType;                                                                /* 静态模式，渐变、跳变、跳变（亮度保留） ，参考协议文档*/
    uint8_t  ucParam;                                                               /* 保留 */
}Static_t;

typedef struct 
{
    uint8_t ucRate;                                                                 /* 闪烁速度 */
    uint8_t ucType;                                                                 /* 闪烁类型，00-七色频闪 01-任意颜色频闪 */
}Blink_t;

typedef struct 
{
    uint8_t ucRate;                                                                 /* 跳变速度 */
    uint8_t ucType;                                                                 /* 跳变类型，00-三色跳变； 01-七色跳变 */
}Hopping_t;

typedef struct 
{
    uint8_t  ucRate;                                                                /* 渐变速度 */
    uint8_t  ucParam;                                                               /* 保留 */
    uint8_t  ucType;                                                                /* 渐变模式, 红色呼吸、黄色呼吸、绿色呼吸...等等，具体参考协议文档*/
    uint16_t usMinBrightness;                                                       /* 渐变最小亮度 */
    uint16_t usMaxBrightness;                                                       /* 渐变最大亮度 */
    uint16_t usStep;                                                                /* 渐变步长 */
}Gradual_t;

typedef struct 
{
    LedMode_t eMode;
    Color_t   xColorParam;
    Static_t  xStaticParam;
    Blink_t   xBlinkParam;
    Hopping_t xHoppingParam;
    Gradual_t xGradualParam;
}LedStatus_t;

typedef struct 
{
    uint8_t  ucDIndex;
    uint16_t usVendorID;                                                            /* 厂商ID */
    uint32_t ulDeviceID;                                                            /* 设备ID */
    uint8_t  ucRelay;                                                               /* 保留 */
    uint8_t  ucEventID;                                                             /* 事件ID */
    uint8_t  ucCmdType;                                                             /* 指令类型 */
    uint8_t  ucGroupID;                                                             /* 组ID */
    uint32_t ulLedNumber;                                                           /* 灯编号 */
    //uint8_t  *pucAddData;                                                         /* 用户自定义参数 */   
    Data_t   xUserData;                                                             /* 用户自定义参数 */ 
}MeshPayload_t;

typedef struct 
{
    uint8_t ucNormalState;
};


typedef struct
{
    uint8_t  ucValid;
    uint32_t ulID;
    uint8_t  ucGrpupID;
}ID_t;

typedef struct
{
    uint8_t ucValueCheck;
    uint8_t ucBondNum;
    ID_t pxBondID[APP_LED_BOND_ID_NUM];
}BondID_t;

typedef enum
{
    eIDLE,
    eWorking
}eStatus;

typedef struct
{
    eStatus    eGraStatus;
    uint8_t    ucGradualType;
    uint16_t   usGradualStep;                                                       /* 渐变步长 */

    uint16_t   usBrightTarget;
    
    uint16_t   usRedStart;
    uint16_t   usGreenStart;
    uint16_t   usBlueStart;

    uint16_t   usRedTarget;
    uint16_t   usGreenTarget;
    uint16_t   usBlueTarget;
}GradualControl_t;

extern LedStatus_t xLedStatus;
extern const uint16_t pucBlinkColor[14][3];
extern GradualControl_t xGradualStatus;
extern uint8_t ucFlashTimes;

uint32_t ucFlashErase(uint32_t ulAddress, uint32_t ulPageNum);
uint32_t ulFlashWrite(uint32_t ulAddress, uint32_t ulDlen, uint8_t *pucDataBuf);
uint32_t ulFlashRead(uint32_t ulAddress, uint32_t ulDlen, uint8_t *pucDataBuf);
void vFlashDataSaveProcess(void);
uint8_t ucFlashNeedSaveCheck(void);

void vApplicationInit(void);
void vRGBColorSet (uint8_t ucBright,  
                   uint8_t ucRedPer, 
                   uint8_t ucGreenPer, 
                   uint8_t ucBluePer, 
                   uint8_t ucReserve, 
                   uint8_t ucModeType);
void vLedLightnessSet(uint16_t usLightness);
void vLEDGradualProcess(void);
void vLEDGradualProcessSingle(uint8_t ucType);
void cycle(void);
void vLedColorPwmSet(uint16_t usRed, uint16_t usGreen, uint16_t usBlue);
void vPwmLowLevelCntGet(uint8_t ucPwmChannel, uint16_t *usCnt);
void vLedTurnOnoff(uint8_t ucSwitch);
void vAdvDataParse(uint8_t *pucData, uint16_t usDlen);
#endif
/**************************************END OF FILE ****************************/