#ifndef  _FUNCTION_H
#define  _FUNCTION_H

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stddef.h"

#define  pdTRUE                                          ( 1u )
#define  pdFALSE                                         ( 0u )
#define  RESULT_SUCCESS                                  ( 1u )
#define  RESULT_ERROR                                    ( 0u )

/* 调试Log打印开关 */
#define configUSE_UART                                   ( 0u )
#define configUART_LOG_SCAN_DATA                         ( 1u )
#define configUART_LOG_ADV_EVENT_ID                      ( 0u )
#define configUART_LOG_LED_ADD                           ( 1u )
#define configUART_LOG_LED_TURN_OFF                      ( 1u )
#define configUART_LOG_LED_TURN_ON                       ( 1u )
#define configUART_LOGLED_DEVICE_ID_CLEAN                ( 1u )
#define configUART_LOG_LED_RGB_SET                       ( 1u )
#define configUART_LOG_LED_BLINK                         ( 1u )
#define configUART_LOG_LED_COLOR_HOPPING                 ( 1u )
#define configUART_LOG_LED_COLOR_GRADUAL                 ( 1u )
#define configUART_LOG_LED_COLOR_GRADUAL_SINGLE          ( 0u )
#define configUART_LOG_LED_COLOR_GRADUAL_CYCLE           ( 0u )
#define configUART_LOG_LED_COLOR_GRADUAL_CYCLE_timer     ( 0u )
#define configUART_LOG_ADV_RELAY                         ( 0u )
#define configUART_LOG_FLASH_READ                        ( 1U )
typedef struct 
{
    uint8_t  *pucData;
    uint16_t  usDlen;
}Data_t;

uint8_t ucTotalCheck(uint8_t *pucData, uint8_t ucDataLen, uint8_t ucType);
uint8_t ucIntToBCD(int8_t scRssi);
uint8_t vFindADStructure(uint8_t ucADStructureType, Data_t *pxAdvData, Data_t *pxStructureData);
#endif
/**************************************END OF FILE ****************************/
