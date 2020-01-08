#ifndef __MM_ALI_H_
#define __MM_ALI_H_
#include "stdio.h"
#include "co_com.h"
//#include "m_al.h"
//#include "m_tb.h"
#include "mm_com.h"

#define M_ALIS_ATTR_MAX_LEN  16  

#define M_ALIS_ATTR_MAX_NUM  16

#define M_ALIS_TIMER_NUM	(2)

#define ALI_COMPANY_ID		(0x01A8)

#define ALI_VENDER_MSID		(0x0000)
#define ALI_VENDER_MCID		(0x0001)

#define M_ALIS_MODEL_SID 	((ALI_COMPANY_ID<<16)+ALI_VENDER_MSID)

#define M_ALIS_REPLAY_MS               (6000)

#define M_ALIS_INDICATION_PERIOD		(180000)//3min


#define M_ALI_VENDER_ADDR		(0xF000)
#define M_ALI_PUBLISH_TTL		(6)
#define M_ALI_PUBLISH_PERIOD	(0x28)
#define M_ALI_PUBLISH_RETX		(0x82) //400ms(8) & 2+1 times   01000 011


// List of 3-byte opcodes for the VENDOR Server model
enum m_alis_3b_opcodes
{   
    // Vendor message Attr get
    MM_MSG_ALIS_ATTR_GET            = 0xD0,
    // Vendor message Attr set
    MM_MSG_ALIS_ATTR_SET		= 0xD1,
    // Vendor message command set
    MM_MSG_ALIS_ATTR_SET_UNACK = 0xD2,
    // Vendor message response stauts
    MM_MSG_ALIS_ATTR_STATUS	= 0xD3,
    // Vendor message indication
    MM_MSG_ALIS_ATTR_INDICATION		= 0xD4,
    // Vendor message confirmation
    MM_MSG_ALIS_ATTR_CONFIRMATION	= 0xD5,
	// Vendor message update time
	MM_MSG_ALIS_ATTR_UPDATE_TIME = 0xDE,
    // Vendor message Transparent
    MM_MSG_ALIS_ATTR_TRANSPARENT = 0xCF,
};

enum m_alis_attr
{
	M_ATTR_COLOR = 0x0123,
	M_ATTR_ONOFF = 0x0100,
	M_ATTR_LTN	 = 0x0121,
	M_ATTR_TEMP	 = 0x0122,
	M_ATTR_MLOO	 = 0x0534,
	M_ATTR_BKLOO = 0x0533,
	M_ATTR_MNUM	 = 0xF004,
	M_ATTR_EVENT = 0xF009,

	M_ATTR_SET_GET_TIME = 0xF01F,
	M_ATTR_SET_GET_TIME_ZONE = 0xF01E,
	M_ATTR_SET_GET_TIME_PARAM = 0xF01D,

	M_ATTR_SET_GET_TIMER = 0xF010,
	M_ATTR_SET_TIMER_PERIOD = 0xF011,
	M_ATTR_SET_DELETE_TIMER = 0xF012,
};

typedef struct attr_type_len
{   
    uint16_t attr_type;
    uint8_t  attr_len;
}attr_type_len_t;

typedef struct alis_attr
{
    uint16_t attr_type;
    uint16_t attr_len;
    uint8_t  attr_param[M_ALIS_ATTR_MAX_LEN];
    uint8_t  tgt_attr_param[M_ALIS_ATTR_MAX_LEN];
}alis_attr_t;

#pragma pack(1)

/*unix time model start*/
typedef struct
{
	uint8_t tid;
	uint16_t attrType;
}alis_msf_head_t;

/*unix time model start*/
typedef struct
{
	uint32_t time;
}alis_time_t;

typedef struct
{
	uint16_t period_time;
	uint8_t retry_delay;
	uint8_t retry_times;
}alis_time_para_t;

typedef struct{
	char timezone;
}alis_time_zone_t;
/*unix time model end*/

/*timer and period timer model start*/
typedef struct
{
	uint8_t index;
	uint32_t time;
	uint16_t attr_type;
	uint8_t attr_para;
}alis_timer_t;

typedef struct
{
	uint8_t index;
	uint16_t time;
	uint8_t schedule;
	uint16_t attr_type;
	uint8_t attr_para;
}alis_period_timer_t;

typedef union{
	alis_period_timer_t period_timer;
	alis_timer_t timer;
}alis_utimer_t;
/*timer and period timer model end*/
#pragma pack()

/**
 *@brief ali vendor received data callback function. 
 *param: model_lid	model indentifier id
 *param: opcode		@ref enum m_alis_3b_opcodes
 *param: data		received data buff start address
 *param: len		received data length
 *return: 0 success; 1 failed
 */
typedef void  (*bc_mm_ali_vendor_rx_cb)(const m_lid_t model_lid, const uint8_t opcode, const uint8_t* data, const uint16_t len);

uint8_t bc_mm_alis_set_onoff_status(uint8_t* data, bool on);

uint8_t bc_mm_alis_set_ltn_status(uint8_t* data, uint16_t ltn);

uint8_t bc_mm_alis_set_tmp_status(uint8_t* data, uint16_t tmp);

/**
 *@brief ali vendor data indicate. 
 *param: model_lid	model indentifier id
 *param: data		indicate data pointer
 *param: len		indicate data lenth
 *return: 0 success; 1 failed
 */
int  bc_mm_alis_send_indication(m_lid_t model_lid, uint8_t* data, int len);

/**
 *@brief ali vendor data send. 
 *param: model_lid	model indentifier id
 *param: opcode		@ref enum m_alis_3b_opcodes
 *param: data		indicate data pointer
 *param: len		indicate data lenth
 *return: 0 success; 1 failed
 */
int  bc_mm_ali_vendor_send(m_lid_t model_lid, uint8_t opcode, uint8_t* data, uint16_t len);

/**
 *@brief ali vendor get random time. 
 *param: none
 *return: random time value
 */
uint16_t bc_mm_alis_get_random_time(void);

/**
 *@brief ali vendor model init. 
 *param: mdl		mesh mode struct pointer
 *param[in]:rx_cb	vendor model recv data callback
 *return: 0 success; 1 failed
 */
int bc_mm_alis_vendor_init(mesh_model_t* mdl, bc_mm_ali_vendor_rx_cb rx_cb);

/**
 *@brief ali vendor model deinit. 
 *param: mdl     mesh mode struct pointer
 *return: 0 success; 1 failed
 */
int bc_mm_alis_vendor_deinit(mesh_model_t* mdl);

#endif

