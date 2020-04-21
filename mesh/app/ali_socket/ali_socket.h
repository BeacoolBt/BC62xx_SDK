#ifndef __ALI_SOCKET_H__
#define __ALI_SOCKET_H__
#include "bc62xx_timer.h"

#include "co_com.h"
#include "mm_com.h"
#include "ali_config.h"
#include "bc_mm_gen_oo.h"
#include "mm_alis.h"

/*Group addr subscribed by socket*/
#define SOCKET_GROUP_ADDR1 (0xC002)
#define SOCKET_GROUP_ADDR2 (0xCFFF)

/*Led quick flash interval, 250ms*/
#define SOCKET_LED_INTERVAL (250)
/*Led quick flash 3 times*/
#define SOCKET_QUICK_3_TIMES (3)
/*Led quick flash 5 times*/
#define SOCKET_QUICK_5_TIMES (5)

/*Button long press 5s for factory reset*/
#define SOCKET_RESET_TIME_CNT	(5000)//5s

/*Button is Actived at low level */
#define BTN_VALID_VAL		(0)
#define BTN_INVALID_VAL		(1)

/*The structer of Socket*/
typedef struct
{
	//for saved config, will write to falsh
 	uint8_t eRelayS;//electronic relay status
 	uint8_t provState;//0 UnProv; 1 Proved
 	uint8_t dType;//device type
	uint8_t saveCfgCrc;

	//Needn't saved
	/*Key status for button detected*/
	uint8_t skey_pre;
	uint8_t skey_next;
	uint32_t btnPresTime;
	/*Delay timer*/
	SYS_TIMER_TYPE delayTimer;//do delay
	/*Delay timer*/
	SYS_TIMER_TYPE bindDelayTimer;
	/*Indicate status timer*/
	SYS_TIMER_TYPE periodIndTimer;
	/*Used model counter*/
	uint8_t model_cnt;
	mesh_model_t model[2];
	/*Generic on off state control structer*/
	onff_ctrl_t oo_ctrl;
}ali_socket_t;

/*The data length saved in falsh*/
#define ALI_SOCKET_SAVE_LEN ((uint32_t)&((ali_socket_t*)0)->saveCfgCrc+1)

/**
 *@brief When Provision status of the socket changed, The function will be call.
 *param: state	Prov state	@enum m_prov_state.
 *return: None.
 */
void ali_socket_prov_state_cb(uint8_t state);

/**
 *@brief Get socket Provision status.
 *param: None.
 *return: [0] Unprov; [1] Proved
 */
uint8_t ali_socket_prov_state_get(void);

/**
 *@brief Do factory reset, clear parameter saved in flash.
 *param: None.
 *return: None.
 */
void ali_socket_factory_reset(void);


/**
 *@brief This function is called in systick handler with 1 time per 1ms, 
 *			so this function must be short.
 *param: None.
 *return: None.
 */
void ali_socket_tick_handler(void);

/**
 *@brief Init socket.
 *param: mdl.
 *param: mcnt.
 *return: [0] Success; [1] Fail.
 */
uint8_t ali_socket_init(mesh_model_t** mdl, uint8_t* mcnt);

/**
 *@brief Deinit socket.
 *param: mdl.
 *return: [0] Success; [1] Fail.
 */
uint8_t ali_socket_deinit(mesh_model_t** mdl);

#endif /*__ALI_SOCKET_H__*/

