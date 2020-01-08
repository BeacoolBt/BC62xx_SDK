#ifndef __ALI_SOCKET_H__
#define __ALI_SOCKET_H__
#include "bc62xx_timer.h"

#include "co_com.h"
#include "mm_com.h"
#include "ali_config.h"
#include "bc_mm_gen_oo.h"
#include "mm_ali.h"


#define SOCKET_GROUP_ADDR1 (0xC002)
#define SOCKET_GROUP_ADDR2 (0xCFFF)

#define SOCKET_BTN_INTERVAL (200)//200ms
#define SOCKET_LED_INTERVAL (500)//500ms

#define SOCKET_RESET_TIME_CNT	(5000/SOCKET_BTN_INTERVAL)//5s
#define SOCKET_SWITH_TIME_CNT	(200/SOCKET_BTN_INTERVAL)//200ms

enum{
	SOCKET_NONE = 0,
	SOCKET_UNPROV_ADV,
	SOCKET_STATIC_ADV,
	SOCKET_PROV_SUCC,
	SOCKET_PROVED,
	SOCKET_ENTER_UNPROV_ADV,
	SOCKET_FACTORY_RESET,
};

typedef struct
{
	//for saved config, will write to falsh
 	uint8_t eRelayS;//electronic relay status
 	uint8_t bindStatus;//0 unbind; 1 bind
 	uint8_t dType;//device type
	uint8_t saveCfgCrc;

	//no saved
	SYS_TIMER_TYPE btnGetTimer;	
	SYS_TIMER_TYPE bindDelayTimer;
	SYS_TIMER_TYPE indStsTimer;
	uint8_t model_cnt;
	mesh_model_t model[2];
	uint32_t btnPressedCnt;
	
	onff_ctrl_t oo_ctrl;
}ali_socket_t;
#define ALI_SOCKET_SAVE_LEN ((uint32_t)&((ali_socket_t*)0)->saveCfgCrc+1)

void ali_socket_prov_state_cb(uint8_t state);

uint8_t ali_socket_prov_state_get(void);

void ali_socket_factory_reset();

uint8_t ali_socket_init(mesh_model_t** mdl, uint8_t* mcnt);

uint8_t ali_socket_deinit(mesh_model_t** mdl);

void ali_socket_ind_status_set(uint8_t on);

void ali_socket_ind_onoff_set(uint8_t on);

void ali_socket_elec_relay_set(uint8_t on);

void ali_socket_elec_relay_reverse(void);

uint8_t ali_socket_elec_relay_get(void);

uint8_t ali_socket_elec_relay_init();

void ali_socket_key_init(void);

uint8_t ali_socket_key_get(void);

#endif /*__ALI_SOCKET_H__*/

