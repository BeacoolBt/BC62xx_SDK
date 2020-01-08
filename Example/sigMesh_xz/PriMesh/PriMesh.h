#ifndef _PRI_MESH_H
#define _PRI_MESH_H
#include "Function.h"

#define  MESH_SEND_QUEUE_LEN         (10u)
#define  MESH_EVENT_ID_BUFF_LEN      (30u)

typedef enum
{
    eAdvRuning = 0,
    eAdvIdle
}eAdvState;

typedef struct
{
    eAdvState eAdvStatus;

    uint16_t  ulAdvTimeTarget;
    uint16_t  ulAdvTimeCnt;

    uint8_t   pucMeshEventID;
    uint8_t   ucMeshRxCnt;
}MeshAdvControl_t;

typedef enum 
{
    ePriorityHigh,
    ePriorityNormal,
    ePriorityLow
}eAdvPriority;

typedef struct
{
    uint8_t ucDlen;
    uint8_t pucData[26];
}MeshData_t;

typedef struct 
{
    uint8_t ucSenIndex;
    uint8_t ucRecIndex;
    MeshData_t pxDataQueue[MESH_SEND_QUEUE_LEN];
}MeshDataQueue_t;

extern uint8_t pucMacAddr[6];
eAdvState  MeshGetAdvStatus (void);
void MeshMacAddressSet(uint8_t *pucAddr, uint8_t ucReverse);
void vGetMacAddr(uint8_t *pucAddr);
void vMeshTxPowerSet(uint8_t ucTxpower);
uint8_t vEventIDCheck(uint8_t ucEventID);
uint8_t vFindADStructure(uint8_t ucADStructureType, Data_t *pxAdvData, Data_t *pxStructureData);
uint8_t ucMeshAdvDataSend (uint16_t ucAdvInterval, uint8_t ucPacketNum, uint16_t ucDlen, uint8_t *pucData);
uint8_t ucMeshAdvStop (void);
uint8_t ucMeshScanStart (void);
uint8_t ucMeshScanStop (void);
uint8_t ucMeshScanIntervalSet (uint16_t usInterval);
uint8_t ucMeshScanWindowSet (uint16_t usWindow);
uint8_t ucMeshGetConnectedStatus (void);
void vMeshAdvSwitchProcess(void);
uint8_t ucMeshDataQueueReceive(eAdvPriority ePriority, uint8_t *pucData, uint8_t *pucDlen);
uint8_t ucMeshDataQueueSend(eAdvPriority ePriority, uint8_t *pucAdvData, uint8_t ucDlen);
void vMeshInit(void);
#endif
/**************************************END OF FILE ****************************/