/**
 *  1、常用函数集合
 */

#include "Function.h"

/**
 * @brief  数据校验
 * @param  ucType 0：累加检验 1：异或校验
 * @return 检验结果
 */
uint8_t ucTotalCheck(uint8_t *pucData, uint8_t ucDataLen, uint8_t ucType)
{
    uint8_t i;
    uint8_t ucCheckValue = 0;
    if(ucType)
    {
        for(i = 0; i < ucDataLen; i++)
            ucCheckValue ^= pucData[i];
    }
    else
    {
        for(i = 0; i < ucDataLen; i++) 
            ucCheckValue += pucData[i];
    } 
    return ucCheckValue;
}

/**
 * @brief   int8类型的rssi信号强度转为BCD码格式
 * @param
 * @return
 */
uint8_t ucIntToBCD(int8_t scRssi)
{
    uint8_t ucValue;

    ucValue = ((-scRssi) / 10) << 4;
    ucValue = ucValue | ((-scRssi) % 10);
    return ucValue;
}

/**
 * @brief   查找指定类型的AD_Structure
 * @param
 * @return  1 - success; 2 - failed;
 */
uint8_t vFindADStructure(uint8_t ucADStructureType, Data_t *pxAdvData, Data_t *pxStructureData)
{
    uint32_t  ulIndex = 0;
    uint8_t   *pucTemData;

    pucTemData = pxAdvData->pucData;

    while (ulIndex < pxAdvData->usDlen)
    {
        uint8_t ucFieldLen = pucTemData[ulIndex];
        uint8_t ucFielType = pucTemData[ulIndex + 1];

        if (ucFielType == ucADStructureType)
        {
        #if 0
            pxStructureData->pucData = &pucTemData[ulIndex + 2];
            pxStructureData->usDlen = ucFieldLen - 1;
        #else
            pxStructureData->pucData = &pucTemData[ulIndex];
            pxStructureData->usDlen = ucFieldLen + 1;
        #endif
            return 1;
        }

        ulIndex += (ucFieldLen + 1);
    }
    return 0;
}
/**************************************END OF FILE ****************************/