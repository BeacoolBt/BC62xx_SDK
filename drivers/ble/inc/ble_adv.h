/** 
  *@file mesh.h
  *@brief mesh support for application.
  */
#ifndef __BLE_ADV_H__
#define __BLE_ADV_H__
#include "common.h"
#include "bc62xx_ble.h"

/**
  *@brief This function will set bluetooth adv data.
  *@param Dt the pointer of adv data.
  *@param Size the size of new adv data.
  *@return None.
  */
void BLE_SetAdvData(uint8_t* Dt, uint16_t Size);

/**
  *@brief This function will set bluetooth scan res.
  *@param Dt the pointer of scan response.
  *@param Size the size of new scan response.
  *@return None.
  */
void BLE_SetScanData(uint8_t* Dt, uint16_t Size);

/**
  *@brief This function will stop adv state.
  *@param None.
  *@return None.
  */
void BLE_StopAdv();

/**
  *@brief This function will start adv state.
  *@param None.
  *@return None.
  */
void BLE_StartAdv();

/**
  *@brief This function will start scan.
  *@param None.
  *@return None.
  */
void BLE_StartScan();

/**
  *@brief This function will stop scan.
  *@parma None.
  *@return None.
  */
void BLE_StopScan();

/**
  *@brief This function set bluetooth mac address.
  *@param Addr the new bluetooth mac to set.
  *@return None.
  */
void BLE_SetMacAddr(tBTADDR Addr);

/**
  *@brief This function set adv type.
  *@param Type the new adv type to set.@ref advType
  *@return None.
  */
void BLE_SetAdvType(enum advType Type);

/**
  *@brief This function set rsp type.
  *@param Type the new rsp type to set.@ref scanType
  *@return None.
  */
void BLE_SetScanRspType(enum scanType Type);

/**
  *@brief This function set Adv interval.
  *@param Type the new rsp type to set.
  *@return None.
  */
void BLE_SetAdvInterval(uint16_t Intv);

/**
  *@brief This function set scan interval.
  *@param Type the new rsp type to set.
  *@return None.
  */
void BLE_SetScanInterval(uint16_t Intv);

/**
  *@brief This function set scan interval and window.
  *@param[in] Intv: scan interval, Unit 0.625ms.
  *@param[in] uIntWindow: scan interval, Unit 0.625ms.
  *@return None.
  */
void BLE_SetScanParameter(uint16_t Intv, uint16_t uIntWindow);

#endif /*__BLE_ADV_H__*/

 
