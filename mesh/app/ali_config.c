/**
*****************************************************************************************
*     Copyright(c) 2015, Beken. All rights reserved.
*****************************************************************************************
  * @file     user_config.c
  * @brief    Source file for user data.
  * @details  Load and store user data to flash.
  * @author   gang.cheng
  * @date     2018-12-20
  * @version  v1.0
  * *************************************************************************************
  */


#include "ali_config.h"
#include <string.h>
#include "portBle.h"
#include "port.h"

#include "bc_sys.h"


static ali_cfg_t* p_ali_cfg;

bool ali_config_uuid_read(ali_uuid_t *uuid)
{
	/** set device uuid */
	uint8_t mac[FLASH_ALI_MAC_ADDR_LEN];
	ali_config_mac_read(mac,0);

	memcpy(uuid->mac_addr, mac, FLASH_ALI_MAC_ADDR_LEN);
	uuid->cid = ALI_CID;
	uuid->pid.adv_ver = BLE_VER_4_2;
	uuid->pid.sec = SEC_ONE_DEV_ONE_SECRET;
	uuid->pid.ota = OTA_UNSUPPORT;
	uuid->pid.bt_ver = BLE_ADV_VER;
	uuid->product_id = p_ali_cfg->pid;// PRODUCT ID

	uuid->feature_flag = (ALI_UUID_VER_2<<1) |BLE_ADV_STATE_UNPROV;
	memset(uuid->rfu, 0, sizeof(uuid->rfu));
	return true;
}

bool ali_config_static_uuid_read(ali_uuid_t *uuid)
{
	/** set device uuid */
	uint8_t mac[FLASH_ALI_MAC_ADDR_LEN];
	ali_config_mac_read(mac,0);

	memcpy(uuid->mac_addr, mac, FLASH_ALI_MAC_ADDR_LEN);
	uuid->cid = ALI_CID;
	uuid->pid.adv_ver = BLE_VER_4_2;
	uuid->pid.sec = SEC_ONE_DEV_ONE_SECRET;
	uuid->pid.ota = OTA_UNSUPPORT;
	uuid->pid.bt_ver = BLE_ADV_VER;
	uuid->product_id = p_ali_cfg->pid;// PRODUCT ID

	uuid->feature_flag = (ALI_UUID_VER_2<<1) |BLE_ADV_STATE_STATIC;
	memset(uuid->rfu, 0, sizeof(uuid->rfu));
	return true;
}



bool ali_config_auth_val_read(uint8_t *key)
{
	for(int i = 0; i < FLASH_ALI_SEC_LEN; ++i){
		key[i] = p_ali_cfg->authval[FLASH_ALI_SEC_LEN - 1 - i];
	}
    m_printf_hex(L_APP, "ali_config_auth_val_read", key, FLASH_ALI_SEC_LEN);
    return true;
}

uint32_t ali_config_pid_read(void)
{
	M_PRINTF(L_APP, "product_id = %d\r\n", p_ali_cfg->pid);
   
    return p_ali_cfg->pid;
}

bool ali_config_mac_read(uint8_t *addr,uint8_t mode) 
{
    if(mode){
        memcpy(addr,  p_ali_cfg->mac, FLASH_ALI_MAC_ADDR_LEN);
    
    }else{
        for(int i = 0; i < FLASH_ALI_MAC_ADDR_LEN; ++i){
            addr[i] =  p_ali_cfg->mac[FLASH_ALI_MAC_ADDR_LEN - 1 - i];
        }
    }

	//m_printf_hex(L_APP, "ali mac read", addr, FLASH_ALI_MAC_ADDR_LEN);
    return true;
}

uint8_t ali_config_device_type_get(void)
{
	return p_ali_cfg->ptype;
}

bool ali_config_data_init(void)
{
	p_ali_cfg = ke_malloc(sizeof(ali_cfg_t), 0);
	if(p_ali_cfg == NULL)
		return false;
	mesh_flash_read(FLASH_ALI_DATA_ADDRESS, sizeof(ali_cfg_t), (uint8_t*)p_ali_cfg);
	uint8_t crc = bc_sys_check_sum((uint8_t *)p_ali_cfg, sizeof(ali_cfg_t)-1);
	M_PRINTF(L_APP, "crcc[%x] crcs[%x]", crc, p_ali_cfg->crc);
	if(p_ali_cfg->crc != crc){		
		return false;
	}
	return true;
}


