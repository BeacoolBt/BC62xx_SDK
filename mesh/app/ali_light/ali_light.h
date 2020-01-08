#ifndef __ALI_LIGHT_H__
#define __ALI_LIGHT_H__
#include "co_com.h"
#include "mm_com.h"

#include "bc62xx_timer.h"
#include "ali_config.h"

#include "bc_mm_gen_oo.h"
#include "bc_mm_light_ln.h"
#include "bc_mm_light_ctl.h"

#include "led.h"
#include "app_mesh.h"


typedef struct
{
	//saved data
	uint8_t quickTime;
	uint8_t provState;
	uint8_t dType;
	uint8_t on;
	uint16_t tp;
	uint16_t ln;
#if (LIGHT_CW_WITH_RGB)
	uint16_t r;
	uint16_t g;
	uint16_t b;
#endif
	uint8_t saveCfgCrc;
	
	SYS_TIMER_TYPE startDelayTimer;//delay timer	
	SYS_TIMER_TYPE bindDelayTimer;//delay timer	
	SYS_TIMER_TYPE periodIndTimer;//indicate timer	
	#ifdef FOR_ALI_TEST
	SYS_TIMER_TYPE keyTimer;//indicate timer
	uint8_t key_pre;
	uint8_t key_next;
	#endif
#ifdef BREATH_CONFIG
	breath_t breath;
#endif
	uint8_t model_cnt;
	mesh_model_t model[4];

	onff_ctrl_t oo_ctrl;
	light_ln_ctrl_t light_ln_ctrl;
	light_ctl_ctrl_t light_ctl_ctrl;
}ali_light_t;
#define ALI_LIGHT_SAVE_LEN ((uint32_t)&((ali_light_t*)0)->saveCfgCrc+1)

void ali_light_prov_state_cb(uint8_t state);

uint8_t ali_light_prov_state_get(void);

void ali_light_factory_reset();

uint8_t ali_light_param_init();

uint8_t ali_light_init(mesh_model_t** mdl, uint8_t* mcnt);

uint8_t ali_light_deinit(mesh_model_t** mdl);

void ali_light_hal_rx(const HAL_TYPE type,const uint8_t *data , uint8_t len);

#endif /*__ALI_LIGHT_H__*/