#include "PriMesh.h"
#include "btreg.h"
#include "common.h"
#include "portBle.h"
#include "gatt_ota.h"
#include "Function.h"

uint8_t pucMacAddr[6] = {0xFF, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5};
/**
 *  -------------------------------
 * |  0  |  1  |  2  |   3  |   4  |
 * --------------------------------
 * | 0dB | 3dB | 5dB | -3dB | -5dB |
 *  -------------------------------
 */
uint8_t pucPowerList[5] = {0x00, 0x01, 0x02, 0x03, 0x04};
/* 事件ID缓存 */
uint8_t ucEventIDIndex;
uint8_t pucEventIDBuf[MESH_EVENT_ID_BUFF_LEN] = { 0 };
/* 发送队列 */
static MeshDataQueue_t  xMeshDataQueueHigh = { 0 };
static MeshDataQueue_t  xMeshDataQueueNormal = { 0 };
static MeshDataQueue_t  xMeshDataQueueLow = { 0 };
/* 状态控制 */
static MeshAdvControl_t xMeshAdvControl = { 0 };


/**
 * @brief    获取Mesh广播的状态
 * @param    
 * @returns  eAdvRuning - 广播中; eAdvIdle - 空闲
 */
eAdvState eMeshGetAdvStatus (void)
{
    return xMeshAdvControl.eAdvStatus;
}

/**
 * @brief   获取设备MAC地址
 * @param   
 * @return  
 */
void vGetMacAddr(uint8_t *pucAddr)
{
	for(uint8_t  i = 0; i < 6; i++)
    {
		pucAddr[5-i] = HR_REG_8BIT(reg_map(mem_le_lap + i));
	}
}

/**
 * @brief   设置蓝牙MAC地址
 * @param   ucReverse  0：地址倒序设置. 其它:正序设置
 * @return  
 */
void MeshMacAddressSet(uint8_t *pucAddr, uint8_t ucReverse)
{
	uint8_t *pucAddrReg = (uint8_t*)reg_map(mem_le_lap);
    if(ucReverse)
    {
        for(uint8_t i = 0; i < 6; i++)
        {
            pucAddrReg[5-i] = pucAddr[i];
        }
    }
    else
    {
        memcpy(pucAddrReg, pucAddr, 6);	
    }
}

/**
 * @brief   设置发射功率
 * @param   ucTxpower：发射功率
 * @return  
 */
void vMeshTxPowerSet(uint8_t ucTxpower)
{
    for(uint8_t i = 0; i < 5; i++)
    {
        if(ucTxpower == pucPowerList[i])
        {
            HWRITE(mem_tx_power, ucTxpower);
            return;
        }
    }
    
    /* 默认最大发射功率+5dB */
    HWRITE(mem_tx_power, 0x02);
}

/**
 * @brief   当前事件ID是否已处理过
 * @param   
 * @return  
 */
uint8_t vEventIDCheck(uint8_t ucEventID)
{
#if configUSE_UART && configUART_LOG_ADV_EVENT_ID
    m_print("Event_ID -> %02X\r\n",ucEventID);
    m_print("Local_ID -> ");
    for(uint8_t i = 0; i < MESH_EVENT_ID_BUFF_LEN; i++)
    {
        m_print("%02X ",pucEventIDBuf[i]); 
    }
    m_print("\r\n");
    m_print("ucEventIDIndex -> %02X\r\n\r\n",ucEventIDIndex);
#endif

    for(uint8_t i = 0; i < MESH_EVENT_ID_BUFF_LEN; ++i)
    {
        if(ucEventID == pucEventIDBuf[i]) return 1;
    }
    
    pucEventIDBuf[ucEventIDIndex] = ucEventID;

    ucEventIDIndex++;
    if(ucEventIDIndex >= MESH_EVENT_ID_BUFF_LEN)  ucEventIDIndex = 0;
    return 0;
}

/**
 * @brief   Mesh广播数据
 * 
 * @param   ucAdvInterval  广播间隔，uint: 0.625
 * @param   ucPacketNum  发包数量
 * @param   ucDlen  数据长度
 * @param   pucData  数据
 * 
 * @return
 */
uint8_t ucMeshAdvDataSend (uint16_t ucAdvInterval, uint8_t ucPacketNum, uint16_t ucDlen, uint8_t *pucData)
{
    if(ucDlen > 31) return RESULT_ERROR;
#if configUSE_UART && configUART_LOG_ADV_RELAY
    m_print("\r\nAdv_Data , %d -> ",ucDlen); 
    for(uint8_t i = 0; i < ucDlen; i++)
    {
        m_print("%02X ",pucData[i]); 
    }
    printf("\r\n");
#endif
    mesh_adv_data_send(ucAdvInterval, ucPacketNum, ucDlen, pucData);

    return RESULT_SUCCESS;
}

/**
 * @brief   扫描响应数据设置
 * @param
 * @return
 */
uint8_t ucMeshScanRspDataSet (uint8_t *pucData, uint16_t ucDlen)
{
    if(ucDlen > 31) return RESULT_ERROR;

    BLE_SetScanData(pucData, ucDlen);

    return RESULT_SUCCESS;
}

/**
 * @brief  开启广播
 * 
 * @param  usAdvInternal: 广播间隔，unit: 0.625ms
 * @param  ucPacketNumber：广播包数
 * @param  ucDlen：广播数据长度
 * @param  pucData ：广播数据
 * 
 * @return RESULT_SUCCESS：广播成功
 *         RESULT_ERROR：广播失败
 */
uint8_t ucMeshAdvStart (uint16_t usAdvInternal, uint8_t ucPacketNumber, uint16_t ucDlen, uint8_t pucData)
{
    mesh_adv_data_send(usAdvInternal, ucPacketNumber, ucDlen, pucData);
    HWRITE(mem_le_adv_enable, 0x01);

    return RESULT_SUCCESS;
}

/**
 * @brief  停止广播
 * @param
 * @return
 */
uint8_t ucMeshAdvStop (void)
{
    HWRITE(mem_le_adv_enable, 0x00);

    xMeshAdvControl.eAdvStatus = eAdvIdle;

    return RESULT_SUCCESS;
}

/**
 * @brief  开启扫描
 * @param
 * @return
 */
uint8_t ucMeshScanStart (void)
{
    HWRITE(mem_le_scan_enable, 0x01);
    return RESULT_SUCCESS;
}

/**
 * @brief  停止扫描
 * @param
 * @return
 */
uint8_t ucMeshScanStop (void)
{
    HWRITE(mem_le_scan_enable, 0x00);
    return RESULT_SUCCESS;
}

/**
 * @brief   扫描间隔设置
 * @param
 * @returns
 */
uint8_t ucMeshScanIntervalSet (uint16_t usInterval)
{
    HW_REG_16BIT(reg_map(mem_le_scan_interval), usInterval);
    return RESULT_SUCCESS;
}

/**
 * @brief   扫描窗口设置
 * @param
 * @returns
 */
uint8_t ucMeshScanWindowSet (uint16_t usWindow)
{
    HW_REG_16BIT(reg_map(mem_le_scan_window), usWindow);
    return RESULT_SUCCESS;
}

/**
 * @brief  获取设备连接状态
 * @param
 * @return OTANONE - 没有连接（连接主要为了OTA升级）; 其他 - 已连接
 */
uint8_t ucMeshGetConnectedStatus (void)
{
    return  gatt_ota_state_get();
}

/**
 * @brief  从发送队列中取数据
 * @param
 * @return pdTRUE - success; pdFALSE - failed
 */
uint8_t ucMeshDataQueueReceive(eAdvPriority ePriority, uint8_t *pucData, uint8_t *pucDlen)
{
    uint8_t ucIndex;

    switch(ePriority)
    {
    case ePriorityHigh:
        /* 队列空 */
        if (xMeshDataQueueHigh.ucSenIndex == xMeshDataQueueHigh.ucRecIndex)  return pdFALSE;

        ucIndex = xMeshDataQueueHigh.ucSenIndex;
        *pucDlen = xMeshDataQueueHigh.pxDataQueue[ucIndex].ucDlen;
        memcpy (pucData, xMeshDataQueueHigh.pxDataQueue[ucIndex].pucData, *pucDlen);
        /* 取出数据之后清空对应缓存 */
        memset (&xMeshDataQueueHigh.pxDataQueue[ucIndex], 0, sizeof(MeshData_t));
        
        xMeshDataQueueHigh.ucSenIndex++;
        if (xMeshDataQueueHigh.ucSenIndex >= MESH_SEND_QUEUE_LEN)
        {
            xMeshDataQueueHigh.ucSenIndex = 0;
        } 
        break;

    case ePriorityNormal:
        if (xMeshDataQueueNormal.ucSenIndex == xMeshDataQueueNormal.ucRecIndex)  return pdFALSE;

        ucIndex = xMeshDataQueueNormal.ucSenIndex;
        *pucDlen = xMeshDataQueueNormal.pxDataQueue[ucIndex].ucDlen;
        memcpy (pucData, xMeshDataQueueNormal.pxDataQueue[ucIndex].pucData, *pucDlen);
        memset (&xMeshDataQueueNormal.pxDataQueue[ucIndex], 0, sizeof(MeshData_t));
        
        xMeshDataQueueNormal.ucSenIndex++;
        if (xMeshDataQueueNormal.ucSenIndex >= MESH_SEND_QUEUE_LEN)  
            xMeshDataQueueNormal.ucSenIndex = 0;
        break;

    case ePriorityLow:
        if (xMeshDataQueueLow.ucSenIndex == xMeshDataQueueLow.ucRecIndex)  return pdFALSE;

        ucIndex = xMeshDataQueueLow.ucSenIndex;
        *pucDlen = xMeshDataQueueLow.pxDataQueue[ucIndex].ucDlen;
        memcpy (pucData, xMeshDataQueueLow.pxDataQueue[ucIndex].pucData, *pucDlen);
        memset (&xMeshDataQueueLow.pxDataQueue[ucIndex], 0, sizeof(MeshData_t));
        
        xMeshDataQueueLow.ucSenIndex++;
        if (xMeshDataQueueLow.ucSenIndex >= MESH_SEND_QUEUE_LEN)  
        {
            xMeshDataQueueLow.ucSenIndex = 0;
        }
        break;
    default:
        break;
    }

    return pdTRUE;
}

/**
 * @brief   要发送的数据插入到队列中
 * @param
 * @return pdTRUE - success; pdFALSE - failed
 */
uint8_t ucMeshDataQueueSend(eAdvPriority ePriority, uint8_t *pucAdvData, uint8_t ucDlen)
{
    if (ucDlen > 28) return pdFALSE;

    switch (ePriority)
    {
    case ePriorityHigh:                                                             /* 插入到高优先级队列 */
        /* 拷贝数 */
        memcpy(xMeshDataQueueHigh.pxDataQueue[xMeshDataQueueHigh.ucRecIndex].pucData, pucAdvData, ucDlen);
        /* 数据长度 */
        xMeshDataQueueHigh.pxDataQueue[xMeshDataQueueHigh.ucRecIndex].ucDlen = ucDlen;
        /* 缓存索引+1 */
        xMeshDataQueueHigh.ucRecIndex++;
        if(xMeshDataQueueHigh.ucRecIndex >= MESH_SEND_QUEUE_LEN) 
            xMeshDataQueueHigh.ucRecIndex = 0;

        /* 缓存索引等于发送索引，对列满，发送索引+1 */
        if (xMeshDataQueueHigh.ucRecIndex == xMeshDataQueueHigh.ucSenIndex)
        {
            xMeshDataQueueHigh.ucSenIndex++;
            if (xMeshDataQueueHigh.ucSenIndex >= MESH_SEND_QUEUE_LEN)  
                xMeshDataQueueHigh.ucSenIndex = 0;
        }
        break;

    case ePriorityNormal:                                                           /* 插入到普通优先级队列 */
        memcpy(xMeshDataQueueNormal.pxDataQueue[xMeshDataQueueNormal.ucRecIndex].pucData, pucAdvData, ucDlen);
        xMeshDataQueueNormal.pxDataQueue[xMeshDataQueueNormal.ucRecIndex].ucDlen = ucDlen;

        xMeshDataQueueNormal.ucRecIndex++;
        if(xMeshDataQueueNormal.ucRecIndex >= MESH_SEND_QUEUE_LEN) 
            xMeshDataQueueNormal.ucRecIndex = 0;
        
        if (xMeshDataQueueNormal.ucRecIndex == xMeshDataQueueNormal.ucSenIndex)
        {
            xMeshDataQueueNormal.ucSenIndex++;
            if (xMeshDataQueueNormal.ucSenIndex >= MESH_SEND_QUEUE_LEN)  
                xMeshDataQueueNormal.ucSenIndex = 0;
        }
    #if configUSE_UART && configUART_LOG_ADV_RELAY
        m_print("\r\nAdd_Data , %d -> ",ucDlen); 
        for(uint8_t i = 0; i < ucDlen; i++)
        {
            m_print("%02X ",pucAdvData[i]); 
        }
        m_print("\r\n"); 
    #endif
        break;

    case ePriorityLow:                                                              /* 插入到低优先级队列 */
        memcpy(xMeshDataQueueLow.pxDataQueue[xMeshDataQueueLow.ucRecIndex].pucData, pucAdvData, ucDlen);
        xMeshDataQueueLow.pxDataQueue[xMeshDataQueueLow.ucRecIndex].ucDlen = ucDlen;

        xMeshDataQueueLow.ucRecIndex++;
        if(xMeshDataQueueLow.ucRecIndex >= MESH_SEND_QUEUE_LEN) 
            xMeshDataQueueLow.ucRecIndex = 0;

        if (xMeshDataQueueLow.ucRecIndex == xMeshDataQueueLow.ucSenIndex)
        {
            xMeshDataQueueLow.ucSenIndex++;
            if (xMeshDataQueueLow.ucSenIndex >= MESH_SEND_QUEUE_LEN)  
                xMeshDataQueueLow.ucSenIndex = 0;
        }
        break;
    default:
        break;
    }
    return pdTRUE;
}

/**
 * @brief  Mesh广播切换，timeout 50ms
 * @param
 * @return
 */
void vMeshAdvSwitchProcess (void)
{
    /* 连接状态不做切换 */
    if (ucMeshGetConnectedStatus() != OTANONE)   return;

    /* 广播中 */
    if (xMeshAdvControl.ulAdvTimeCnt > 0) 
	{
        if (xMeshAdvControl.ulAdvTimeCnt < xMeshAdvControl.ulAdvTimeTarget)
        {
            xMeshAdvControl.ulAdvTimeCnt += 50;
            return;
        }
    }

    /* 触发广播 */
    uint8_t  ucNeedAdv = 0;
    uint8_t  pucAdvData[26] = { 0 };
    uint8_t  ucDatalen = 0;

    while(1)
    {
        /* 检查高优先级队列 */
        if (ucMeshDataQueueReceive(ePriorityHigh, pucAdvData, &ucDatalen) == pdTRUE) 
		{
            xMeshAdvControl.ulAdvTimeTarget = 5000;
            ucNeedAdv = 1;
            break;
        }
        /* 检查普通优先级队列 */
        if (ucMeshDataQueueReceive(ePriorityNormal, pucAdvData, &ucDatalen) == pdTRUE) 
		{
        #if configUSE_UART && configUART_LOG_ADV_RELAY
            m_print("\r\nGet_Data , %d -> ",ucDlen); 
            for(uint8_t i = 0; i < ucDlen; i++)
            {
                m_print("%02X ",pucAdvData[i]); 
            }
            printf("\r\n");
        #endif
            xMeshAdvControl.ulAdvTimeTarget = 150;
            ucNeedAdv = 1;
            break;
        }
        /* 检查高优先级队列 */
        if (ucMeshDataQueueReceive(ePriorityLow, pucAdvData, &ucDatalen) == pdTRUE) 
		{
            xMeshAdvControl.ulAdvTimeTarget = 150;
            ucNeedAdv = 1;
            break;
        }
        break;
    }

    if (ucNeedAdv == 1)
    {
        xMeshAdvControl.ulAdvTimeCnt = 50;
        xMeshAdvControl.ucMeshRxCnt = 0;
        xMeshAdvControl.eAdvStatus = eAdvRuning;
        ucMeshAdvDataSend (64, (xMeshAdvControl.ulAdvTimeTarget / 40), ucDatalen, pucAdvData);
    }
    else
    {
        xMeshAdvControl.ulAdvTimeCnt = 0;
        xMeshAdvControl.ucMeshRxCnt = 0;
        xMeshAdvControl.eAdvStatus = eAdvIdle;
    }
}

/**
 * @brief   Mesh初始化
 * @param
 * @returns
 */
void vMeshInit (void)
{
    ucEventIDIndex = 0;
    memset(pucEventIDBuf, 0, MESH_EVENT_ID_BUFF_LEN);
    memset (&xMeshAdvControl, 0, sizeof(MeshAdvControl_t));
    memset (&xMeshDataQueueHigh, 0, sizeof(MeshDataQueue_t));
    memset (&xMeshDataQueueNormal, 0, sizeof(MeshDataQueue_t));
    memset (&xMeshDataQueueLow, 0, sizeof(MeshDataQueue_t));
}
/**************************************END OF FILE ****************************/
