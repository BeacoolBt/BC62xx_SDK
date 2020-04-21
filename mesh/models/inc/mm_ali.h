#ifndef __MM_ALI_H_
#define __MM_ALI_H_
#include "stdio.h"
#include "co_com.h"
#include "mm_com.h"


#define M_ALIS_ATTR_MAX_LEN  16  

#define M_ALIS_ATTR_MAX_NUM  16

#define M_ALIS_TIMER_NUM	(20)

#define ALI_COMPANY_ID		(0x01A8)

#define ALI_VENDER_MSID		(0x0000)
#define ALI_VENDER_MCID		(0x0001)

#define M_ALIS_MODEL_SID 	((ALI_COMPANY_ID<<16)+ALI_VENDER_MSID)

#define M_ALIS_REPLAY_MS               (6000)

#define M_ALIS_INDICATION_PERIOD		(360000)//6mins


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
	MM_MSG_ALIS_ATTR_REQ_TIME_TX = 0xDE,
	// Vendor message update time
	MM_MSG_ALIS_ATTR_REQ_TIME_RX = 0xDF,
    // Vendor message Transparent
    MM_MSG_ALIS_ATTR_TRANSPARENT = 0xCF,
};

enum m_alis_attr
{
	/*System attribute*/
	/*Error code*/
	M_SATTR_ECODE  = 0x0000,
	
	M_ATTR_COLOR = 0x0123,
	M_ATTR_ONOFF = 0x0100,
	M_ATTR_LTN	 = 0x0121,
	M_ATTR_TEMP	 = 0x0122,
	M_ATTR_MLOO	 = 0x0534,
	M_ATTR_BKLOO = 0x0533,
	
	/*Version information*/
	M_SATTR_VER   = 0xFF01,
	/*Device Feature*/
	M_SATTR_FEATURE = 0xFF02,
	/*Flash size*/
	M_SATTR_FLASH_SIZE = 0xFF03,
	/*Used Flash size*/
	M_SATTR_FLASH_USED = 0xFF04,
	/*Remain FLASH*/
	M_SATTR_FLASH_REMAIN = 0xFF05,
	/*Engineer_Mode*/
	M_SATTR_ENGINEER_MODE = 0xFF06,

	/*Generate attribute*/
	/*Work status*/
	M_ATTR_WORK_STATUS = 0xF001,
	/*User ID*/
	M_ATTR_USER_ID = 0xF002,
	/*Device name*/
	M_ATTR_DEV_NAME = 0xF003,
	/*Scene mode*/
	M_ATTR_SCENE_MODE = 0xF004,
	/*On/Off plan*/
	M_ATTR_ON_OFF_PLAN = 0xF008,
	/*Event trigger*/
	M_ATTR_EVENT_TRIG = 0xF009,
	/*Signal enhance*/
	M_ATTR_SIGNAL_ENHANCE = 0xF00A,
	/*Turn up/down attr*/
	M_ATTR_TURN_UP_DOWN = 0xF00B,
	/*Element number*/
	M_ATTR_ELEMENT_NUM = 0xF00C,
	/*Attribute switch*/		
	M_ATTR_ATTR_SWITCH = 0xF00D,
	/*Remote address*/
	M_ATTR_REMOTE_ADDR = 0xF00E,
	/*Device rssi*/
	M_ATTR_DEV_RSSI = 0xF00F,
	/*Timer*/
	M_ATTR_TIMER = 0xF010,
	/*Period timer*/
	M_ATTR_PTIMER = 0xF011,
	/*Delete timer*/
	M_ATTR_DTIMER = 0xF012,
	/*Request update timer*/
	M_ATTR_REQ_UPD_TMR = 0xF013,
	
	/*Event clear*/
	M_ATTR_EVENT_CLEAR = 0xF019,
	/*unix time parameter*/
	M_ATTR_TIME_PARAM = 0xF01D,
	/*unix time zone*/
	M_ATTR_TIME_ZONE = 0xF01E,
	/*unix time*/
	M_ATTR_TIME = 0xF01F,
	
	/*Power of memory*/
	M_ATTR_PWR_OFFMEM = 0xF021,
};

enum m_alis_event
{
	/*Timer completed event*/
	M_EVT_TIMER_CMP = 0x11,
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

/**
 *@brief ali vendor get total length of attribute type and attribute value.
 	att[0 1] saved attribute type. 
 *param: att	The value attribute structer.
 *return: The Length of attribute structer.
 */
uint8_t bc_mm_alis_get_att_len(uint8_t* att);


#endif/*__MM_ALI_H_*/

