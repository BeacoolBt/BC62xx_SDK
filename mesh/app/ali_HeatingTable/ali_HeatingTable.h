#ifndef __ALI_HRATING_TABLE_H__
#define __ALI_HRATING_TABLE_H__
#include "co_com.h"
#include "mm_com.h"

#include "bc62xx_timer.h"
#include "ali_config.h"

#include "bc_mm_gen_oo.h"
#include "bc_mm_light_ln.h"
#include "bc_mm_light_ctl.h"
#include "app_mesh.h"

typedef struct
{
	//saved data
	uint8_t quickTime;
	uint8_t provState;
	uint8_t dType;
	uint8_t saveCfgCrc;
	
	SYS_TIMER_TYPE startDelayTimer;//delay timer	
	SYS_TIMER_TYPE bindDelayTimer;//delay timer	
	SYS_TIMER_TYPE periodIndTimer;//indicate timer	
	uint8_t key_pre;
	uint8_t key_next;
	uint8_t model_cnt;
	uint8_t onOff;
	mesh_model_t model[2];

	onff_ctrl_t oo_ctrl;
	light_ln_ctrl_t light_ln_ctrl;
	light_ctl_ctrl_t light_ctl_ctrl;
}ali_heating_table_t;

#define ALI_HEATING_TABLE_SAVE_LEN ((uint32_t)&((ali_heating_table_t*)0)->saveCfgCrc+1)

void ali_heating_table_prov_state_cb(uint8_t state);

uint8_t ali_heating_table_prov_state_get(void);

void ali_heating_table_factory_reset();

uint8_t ali_heating_table_init(mesh_model_t** mdl, uint8_t* mcnt);

uint8_t ali_heating_table_deinit(mesh_model_t** mdl);

void ali_heating_table_hal_rx(const HAL_TYPE type, const uint8_t *data , uint8_t len);

#endif /*__ALI_HRATING_TABLE_H__*/