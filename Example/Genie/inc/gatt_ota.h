#ifndef __GATT_OTA_H_
#define __GATT_OTA_H_

#include "common.h"
#include "bc62xx_ble.h"

/*@brief OTA upgrade state value*/
#define OTANONE 		(0x00)
#define NOSTART 		(0x01)
#define OTASTART		(0x02)
#define OTAPROCESSING	(0x03)

/*@brief OTA Character handle value*/
#define OTA_WRITE_HANDLE 	0x0015
#define OTA_INDICATE_HANDLE 0x0019

/**
 ****************************************************************************************
 * @brief: initialize Gatt ota.
 *
 * @param: None
 * @return: None
 ****************************************************************************************
 */
void gatt_ota_init(void);

/**
 ****************************************************************************************
 * @brief: cancel initialize.
 *
 * @param: None
 * @return: None
 ****************************************************************************************
 */
void gatt_ota_deinit(void);

/**
 ****************************************************************************************
 * @brief: Start Gatt Ota.
 *
 * @param: None
 * @return: None
 ****************************************************************************************
 */
void gatt_ota_start(void);

/**
 ****************************************************************************************
 * @brief: Stop Gatt Ota.
 *
 * @param: None
 * @return: None
 ****************************************************************************************
 */
void gatt_ota_stop(void);

/**
 ****************************************************************************************
 * @brief: Get Gatt ota current state.
 *
 * @param: None
 * @return: Current Gatt Ota state value
 ****************************************************************************************
 */
uint8_t gatt_ota_state_get(void);

/**
 ****************************************************************************************
 * @brief: Get ota current state.
 *
 * @param[in]: the Buffer pointer, Recvived data from OTA_WRITE_HANDLE 
 * @param[in]: Recvived data length
 * @return: Current Gatt Ota state value
 ****************************************************************************************
 */
void gatt_ota_data_deal(uint8_t* dat, uint8_t length);

#endif
