/**
*****************************************************************************************
*     Copyright(c) 2015, Beken. All rights reserved.
*****************************************************************************************
  * @file     ali_config.c
  * @brief    Source file for user data.
  * @details  Load and store user data to flash.
  * @author   gang.cheng
  * @date     2018-12-20
  * @version  v1.0
  * *************************************************************************************
  */





#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <flash_config.h>

#define ALI_APP

#define ALI_VER "2018-12-29"

/**
 * @brief alibaba data storage address
 * @note this address must in flash map and
 *  must be 4k alignment
 */
#define FLASH_ALI_PID_LEN	4
#define FLASH_ALI_MAC_ADDR_LEN		6
#define FLASH_ALI_SEC_LEN		16
#define FLASH_ALI_AUT_LEN		16

#define ALI_QUICK_START_DATA	(0xABBA55AA)
#define ALI_PROV_SUCC_DATA		(0x55AA55AA)
#define ALI_QUICK_START_CNT		(5)
#ifndef ALI_QUICK_START_TIME
//#define ALI_QUICK_START_TIME	(5000)//5s
#define ALI_QUICK_START_TIME	(2000)//2s for hongyan
#endif
#define ALI_FLASH_DELAY_TIME	(1000)//1s
#define ALI_QUICK_BLINK_TIME	(1000)//1s

/***************************************************
---------------------------------
	PID(4Byte) 
---------------------------------
	MAC(6Byte)
---------------------------------
	SECRET(16Byte)
---------------------------------
	AUTHVAL(16Byte) sha256("ID,MAC,SECRET")
---------------------------------

***************************************************/
#define FLASH_ALI_DATA_ADDRESS		CONFIG_START_ADDR	/*42 byte*/
#define FLASH_ALI_DATA_SIZE			(FLASH_ALI_PID_LEN+FLASH_ALI_MAC_ADDR_LEN\
										+FLASH_ALI_SEC_LEN+FLASH_ALI_AUT_LEN)
#define FLASH_ALI_PID_ADDR_OFFSET	FLASH_ALI_DATA_ADDRESS
#define FLASH_ALI_MAC_ADDR_OFFSET	(FLASH_ALI_PID_ADDR_OFFSET+FLASH_ALI_PID_LEN)
#define FLASH_ALI_SEC_ADDR_OFFSET	(FLASH_ALI_MAC_ADDR_OFFSET+FLASH_ALI_MAC_ADDR_LEN)
#define FLASH_ALI_AUT_ADDR_OFFSET	(FLASH_ALI_SEC_ADDR_OFFSET+FLASH_ALI_SEC_LEN)

#define FLASH_ALI_START_ADDRRESS	(0)//(APP_START_ADDR)
#define FLASH_ALI_START_SIZE		(ALI_QUICK_START_CNT*4)
#define FLASH_ALI_PROVED_ADDRRESS	(FLASH_ALI_START_ADDRRESS + FLASH_ALI_START_SIZE)
#define FLASH_ALI_PROVED_SIZE		(4)


#define SEC_ONE_TYPE_ONE_SECRET		0
#define SEC_ONE_DEV_ONE_SECRET		1

#define OTA_UNSUPPORT	0
#define OTA_SUPPORT		1

#define BLE_ADV_VER		0x01

#define BLE_VER_4_0		0x00
#define BLE_VER_4_2		0x01
#define BLE_VER_5_0		0x02
#define BLE_VER_5_U		0x03

#define ALI_UUID_VER_1		0x00
#define ALI_UUID_VER_2		0x01

#define BLE_ADV_STATE_UNPROV	(0x00)
#define BLE_ADV_STATE_STATIC	(0x01)

#define ALI_CID		0x01A8


#define ALI_GROUP_ADDRESS_LIGHT			(0xC000)
#define ALI_GROUP_ADDRESS_HEAT_TABLE	(0xC1A5)

#define ALI_GROUP_ADDRESS1				(0xCFFF)

#define ALI_PARAM_FROM_FLASH

#pragma pack(1)
typedef struct
{
    uint32_t pid;
	uint8_t mac[FLASH_ALI_MAC_ADDR_LEN];
	uint8_t secret[FLASH_ALI_SEC_LEN];
	uint8_t authval[FLASH_ALI_AUT_LEN];
	uint8_t ptype;
	uint8_t crc;
} ali_cfg_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t cid;
    struct
    {
        uint8_t adv_ver: 4;
        uint8_t sec: 1;
        uint8_t ota: 1;
        uint8_t bt_ver: 2; //!< 0 bt4.0, 1 bt4.2, 2 bt5.0, 3 higher
    } pid;
    uint32_t product_id;//the same product has the same id
    uint8_t mac_addr[6];//different device has different mac
    
    uint8_t feature_flag;
    uint8_t rfu[2];
} ali_uuid_t;

#pragma pack()

enum{
	PID_CW = 0,
	PID_LTN = 1,
	PID_SOCKET = 2,
	PID_HEAT_TABLE = 3,
	PID_MAX,
};

/**
 * @brief read alibaba uuid
 * @param[out] uuid: alibaba specified uuid
 * @retval TRUE: read data success
 * @retval FALSE: read data failed
 */
bool ali_config_uuid_read(ali_uuid_t *uuid);

/**
 * @brief read alibaba static uuid
 * @param[out] uuid: alibaba specified uuid
 * @retval TRUE: read data success
 * @retval FALSE: read data failed
 */
bool ali_config_static_uuid_read(ali_uuid_t *uuid);

/**
 * @brief read alibaba secret key from flash
 * @param[out] key: alibaba specified secret key
 * @retval TRUE: read data from flash success
 * @retval FALSE: read data from flash failed
 */
bool ali_config_auth_val_read(uint8_t *key);

/**
 * @brief read alibaba product id from flash
 * @param[out] key: alibaba specified secret key
 * @retval TRUE: read data from flash success
 * @retval FALSE: read data from flash failed
 */
uint32_t ali_config_pid_read(void);

/**
 * @brief read alibaba mac_addr from flash
 * @param[out] addr: alibaba specified mac_addr
 * @retval TRUE: read data from flash success
 * @retval FALSE: read data from flash failed
 */

bool ali_config_mac_read(uint8_t *addr,uint8_t mode);

/**
 * @brief write alibaba specified data to flash
 * @param[in] id: alibaba specified product id
 * @param[in] key: alibaba specified secret key
 * @retval TRUE: write data to flash success
 * @retval FALSE: write data to flash failed
 */
bool ali_config_data_write(uint32_t id, const uint8_t *key);

/**
 * @brief get device type
 * @retval device type
 */
uint8_t ali_config_device_type_get(void);

/**
 * @brief check whether flash contains alibaba private date or not
 * @retval TRUE: flash contains alibaba data
 * @retval FALSE: flash has no alibaba data
 */
bool ali_config_data_init(void);

#endif


