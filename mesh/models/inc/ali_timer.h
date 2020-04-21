#ifndef __ALI_TIMER_H__
#define __ALI_TIMER_H__

#include "co_com.h"
#include "bc62xx_timer.h"
#include "mm_ali.h"
#include "bc_mm_gen_oo.h"
#include "bc_mm_light_ln.h"
#include "bc_mm_light_ctl.h"

#define MAX_FLASH_LIN	256

/*Define error number*/
#define TIME_NO_ERR 			(0x00) //No error

#define TIME_ERR_UNSET_TIME		(0x80) //UNIX时间未设置
#define TIME_ERR_UNSUP_ATTR 	(0x82) //不支持的属性操作
#define TIME_ERR_ATTR_OPT		(0x83) //属性操作参数错误
#define TIME_ERR_LESS_TIME		(0x84) //定时时间小于当前时间
#define TIME_ERR_UNFIND_INDEX 	(0x85) //未找到需要查询的定时索引
#define TIME_ERR_TIMER_FULL 	(0x86) //定时数量已满
#define TIME_ERR_PTIMER_FORMATE (0x87) //周期定时格式错误

#define TIMER_PERIOD_TICKS		(10*1000)	//10s

#define TIMER_OPERATE_ALL		(0xFF)

/*Default time zone*/
#define TIMER_DFT_TIME_ZONE		(8)
/*Check time period*/
#define TIMER_DFT_CHECK_TIME_PERIOD		(180)	//180mins
/*Retry intervel when check failed*/
#define TIMER_DFT_CHECK_TIME_RETRY_ITL	(1)//(10)	//10mins
/*Retry count when check falied*/
#define TIMER_DFT_CHECK_TIME_RETRY_CNT	(10) //10times

/*ali mesh local time*/
typedef struct{
	/*UNIX time*/
	uint32_t time;
	uint16_t period_time;	//Unit: mins
	uint8_t retry_delay;	//Unit: mins
	uint8_t retry_times;
	uint8_t timezone;
	uint8_t tmr_len;		/*Total length if timers*/
	uint8_t tcrc;			/*all timer check sum value*/
	uint8_t lcrc;			/*check sum value ubove this member*/
}ali_time_t;

#define ALI_TIMER_CRC_LEN	((uint32_t)&((ali_time_t*)0)->lcrc)

typedef struct{
	/*On/off control*/
	onff_ctrl_t* oo_ctrl;
	/*Light lightness control*/
	//light_ln_ctrl_t* light_ln_ctrl;
	/*Light control*/
	//light_ctl_ctrl_t* light_ctl_ctrl;
}ali_timer_ctrl_t;

typedef struct{
	/*saved to flash*/
	/*local time*/
	ali_time_t ltime;
	
	/*Period timer, Period is 60s*/
	SYS_TIMER_TYPE period_timer;
	/*Update local time timer*/
	SYS_TIMER_TYPE upd_timer;
	/*Timer list*/
    co_list_t tmr_list;
	/*Current timer counter*/
	uint8_t tmr_cnt;
	/*System ticks when time sync*/
	uint32_t ticks;//Unit:ms
	/*Current retry_times*/
	uint8_t cur_retry_times;
	/*Pointer model structer*/
	mesh_model_t* mdl;
	/*Timer callback function*/
	ali_timer_ctrl_t* ctl;
}ali_timer_env_t;


#pragma pack(1)
/*ali mesh timer*/
typedef struct{
	/// List header for chaining
    struct co_list_hdr hdr;
	uint8_t len;
	uint8_t index[0];//bit7 is 1; indicate this is period timer
}ali_thead_t;
#if 0
/*ali mesh timer*/
typedef struct{
	/// List header for chaining
    ali_thead_t head;
	uint32_t time;
	uint8_t att[0];
}ali_timer_t;

/*ali mesh period timer*/
typedef struct{
	/// List header for chaining
    ali_thead_t head;
	uint16_t time;
	uint8_t schedule;
	uint8_t att[0];
}ali_ptimer_t;
#endif
#pragma pack(0)

/**
 *@brief Init ali timer, and set timer expired function.
 *param: callback function.
 *return: [0] success; [others] failed
 */
uint8_t ali_timer_init(mesh_model_t* mdl, ali_timer_ctrl_t* ctl);

/**
 *@brief Deinit ali timer.
 *param: None.
 *return: [0] success; [1] failed
 */
uint8_t ali_timer_deinit(void);

/**
 *@brief When recvived data from ali vendor call this function.
 *param: None.
 *return: [true] processed; [false] Not process.
 */
uint8_t ali_timer_rx(m_api_model_msg_ind_t const *ind);

#endif /*__ALI_TIMER_H__*/