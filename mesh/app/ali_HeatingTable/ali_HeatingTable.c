#include "ali_HeatingTable.h"
#include "mm_defines.h"
#include "meshTimer.h"

#include "mm_alis.h"
#include "bc_flash.h"
#include "bc_sys.h"
#include "led.h"
#include "ProtocolRln.h"

#define CMD_EINX_LEN	(4)
#define BROADCAST_ADDR (0xC1A5)

#define HT_MODE		(0xf004)
#define HT_TURN		(0xf00b)
#define HT_GEAR		(0x0174)//(0xf00b)
#define HT_GEAR_L	(0x0185)//[1 5]
#define HT_GEAR_R	(0x0186)//[1 5]
#define HT_GEAR_F	(0x0187)//[1 5]
#define HT_GEAR_B	(0x0188)//[1 5]
#define HT_DRCT		(0x054A)//Direction up down and stop


static ali_heating_table_t* pstHeatTable = NULL;

static void _period_ind_timer_cb(void* param);

static uint8_t _param_init(void);

static void _ready_to_indicate(bool random);

/*********************Hardware Ctrl Start**********************/
static void _cw_init(light_cw_t* t)
{
	return led_cw_init(t);
}

static void _cw_set_on_off(uint8_t on)
{
	//led_cw_set_on_off(on, NULL);
	//_ready_to_indicate(true);
	uint8_t _len = 4+CMD_EINX_LEN;
	uint8_t* _data = malloc(_len);
	if(!_data)
		return;
	_data[0] = MM_MSG_ALIS_ATTR_SET;
	_data[1] = ALI_COMPANY_ID&0xff;
	_data[2] = ALI_COMPANY_ID>>8;
	_data[3] = 0;
	_data[4] = emmTid;
	_data[5] = M_ATTR_ONOFF&0xff;
	_data[6] = M_ATTR_ONOFF>>8;
	_data[7] = on;
	protocol_send(_data, _len);
	free(_data);
}
static uint8_t _cw_get_on_off(void)
{
	return pstHeatTable->onOff;
}

/*********************Hardware Ctrl End**********************/

static void _ready_to_indicate(bool random)
{
	if(pstHeatTable == NULL)
		return;
	
	uint32_t delay_time = M_ALIS_INDICATION_PERIOD;
	
	if(random){
		delay_time = bc_mm_alis_get_random_time();
	}
	
	//if bound
	if(ali_heating_table_prov_state_get()){
		SYS_SetTimer(&pstHeatTable->periodIndTimer, (int)delay_time/10, 
					TIMER_SINGLE, (Timer_Expire_CB)_period_ind_timer_cb);
	}
}

static void _bind_timer_cb(void* param)
{
	uint16_t status;

	if(!pstHeatTable->provState){
		pstHeatTable->provState = 1;
		pstHeatTable->saveCfgCrc = bc_sys_check_sum((uint8_t*)pstHeatTable, ALI_HEATING_TABLE_SAVE_LEN-1);
		bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
		bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_HEATING_TABLE_SAVE_LEN,(uint8_t*)pstHeatTable);
	}
	//model bind
	m_lid_t app_key_lid = 0;
	status = bc_m_key_app_find(0, &app_key_lid);
	if (status != M_ERR_NO_ERROR){
		M_PRINTF(L_FND, "m_tb_key_app_find error");
		return;
	}
	for(uint8_t i = 0; i < pstHeatTable->model_cnt; ++i){
		mesh_model_t* t = &pstHeatTable->model[i];
		status = bc_m_key_model_bind(app_key_lid, t->lid);
		if (status == M_ERR_NO_ERROR){
			// Inform the Model IO manager
			bc_m_mio_bind(t->lid);
		}
	}
	
	//model subscription
	// Add address to the subscription list
	//model bond 0xC000，0xCFFF。
	for(uint8_t i = 0; i < pstHeatTable->model_cnt; ++i){
		mesh_model_t* t = &pstHeatTable->model[i];
		//status = bc_m_mio_add_subscription(t->lid, 0xF000);
		bc_m_mio_add_subscription(t->lid, ALI_GROUP_ADDRESS_HEAT_TABLE);
		bc_m_mio_add_subscription(t->lid, 0xCFFF);
		
	}
	mesh_model_t* t = &pstHeatTable->model[pstHeatTable->model_cnt-1];
	bc_m_mio_add_subscription(t->lid, M_ALI_VENDER_ADDR);

	// Set publication parameters
    status = bc_m_mio_set_publi_param(t->lid, M_ALI_VENDER_ADDR, NULL,
                                      app_key_lid, M_ALI_PUBLISH_TTL, M_ALI_PUBLISH_PERIOD,
                                     M_ALI_PUBLISH_RETX, 0);
	M_PRINTF(L_APP, "t->lid[%x] pstHeatTable->model_cnt[%x] status[%x]",t->lid, pstHeatTable->model_cnt, status);
	
	_ready_to_indicate(true);
}

static void _period_ind_timer_cb(void* param)
{
	uint8_t data[11];
	int len = 0;
	len += bc_mm_alis_set_onoff_status(&data[0]+len, _cw_get_on_off());
	m_printf_hex(L_APP, "_period_ind_timer_cb", data, len);
	bc_mm_alis_send_indication(pstHeatTable->model[1].lid, data, len);
	
	_ready_to_indicate(false);
	bc_m_ali_save_seq_iv();
}

static void _quick_restore_timer_cb(void *t)
{
	M_PRINTF(L_APP, "");
	pstHeatTable->quickTime = 0;
	pstHeatTable->saveCfgCrc = bc_sys_check_sum((uint8_t*)pstHeatTable, ALI_HEATING_TABLE_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_HEATING_TABLE_SAVE_LEN,(uint8_t*)pstHeatTable);
}

static uint8_t _quick_restore_init(void)
{
	if(pstHeatTable->quickTime++ < ALI_QUICK_START_CNT){//0 1 2 3 4
		pstHeatTable->saveCfgCrc = bc_sys_check_sum((uint8_t*)pstHeatTable, ALI_HEATING_TABLE_SAVE_LEN-1);
		bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
		bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_HEATING_TABLE_SAVE_LEN,(uint8_t*)pstHeatTable);
		return 0;
	}
	M_PRINTF(L_APP, "");
	//TODO: 1HZ flash 3 timers
	uint8_t i = 6;
	uint32_t t = mesh_timer_get_time();
	do{
		while(mesh_timer_get_time()-t < ALI_QUICK_BLINK_TIME){//delay 1s
			continue;
		}
		t = mesh_timer_get_time();
		led_cw_set_on_off(i%2,NULL);
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->1);
	//restore flash parameter
	ali_heating_table_factory_reset();
	return 1;
}

static uint8_t _param_init(void)
{
	memset(pstHeatTable, 0, sizeof(ali_heating_table_t));
	//read saved parameter
	bc_flash_read(FLASH_ALI_START_ADDRRESS, ALI_HEATING_TABLE_SAVE_LEN,(uint8_t*)pstHeatTable);
	uint8_t tCrc = bc_sys_check_sum((uint8_t*)pstHeatTable, ALI_HEATING_TABLE_SAVE_LEN-1);
	if(pstHeatTable->saveCfgCrc != tCrc){
		memset((uint8_t*)pstHeatTable, 0, ALI_HEATING_TABLE_SAVE_LEN);
		pstHeatTable->dType = ali_config_device_type_get();
	}else{
		if(pstHeatTable->dType != ali_config_device_type_get()){//change device type
			M_PRINTF(L_APP, "device type is changed [%d] to [%d]", pstHeatTable->dType, ali_config_device_type_get());
			bc_m_clear_config();
			memset((uint8_t*)pstHeatTable, 0,  ALI_HEATING_TABLE_SAVE_LEN);
			pstHeatTable->dType = ali_config_device_type_get();
			pstHeatTable->saveCfgCrc = bc_sys_check_sum((uint8_t*)pstHeatTable, ALI_HEATING_TABLE_SAVE_LEN-1);
			bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
			bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_HEATING_TABLE_SAVE_LEN,(uint8_t*)pstHeatTable);
		}
	}

	_cw_init(NULL);

	return 0;
}

void ali_heating_table_prov_state_cb(uint8_t state)
{
#ifdef APP_FOR_ALI
	if(state == M_PROV_SUCCEED_APP){
		SYS_SetTimer(&pstHeatTable->bindDelayTimer, (int)500/10, 
					TIMER_SINGLE, (Timer_Expire_CB)_bind_timer_cb);
		return;
	}
#endif 
}

uint8_t ali_heating_table_prov_state_get(void)
{
	M_PRINTF(L_APP, "bond status[%d]", pstHeatTable->provState);
	return pstHeatTable->provState == 1;
}

void ali_heating_table_factory_reset()
{
	bc_m_clear_config();
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, FLASH_ALI_USED_PAGE_SIZE);
	bc_sys_reset();
}
static void _ali_heating_table_vendor_rx_cb(const m_lid_t model_lid, const uint8_t opcode, const uint8_t* data, const uint16_t len)
{
	if(model_lid != pstHeatTable->model[1].lid){
		return;
	}
	M_PRINTF(L_APP, "opcode[%x]", opcode);
	m_printf_hex(L_APP, "ALI VENDOR RECV", data, len);
	switch(opcode){
		case MM_MSG_ALIS_ATTR_SET:{
			uint8_t _len = len+CMD_EINX_LEN;
			uint8_t* _data = malloc(_len);
			if(!_data)
				return;
			_data[0] = opcode;
			_data[1] = ALI_COMPANY_ID&0xff;
			_data[2] = ALI_COMPANY_ID>>8;
			_data[3] = 0;
			memcpy(_data+4, data, len);
			protocol_send(_data, _len);
			free(_data);
		}break;
		case MM_MSG_ALIS_ATTR_CONFIRMATION:{
			
		}break;
		default:break;
	}
}

uint8_t ali_heating_table_init(mesh_model_t** mdl, uint8_t* mcnt)
{
	M_PRINTF(L_APP, "-------init heating table---------");
	pstHeatTable = ke_malloc(sizeof(ali_heating_table_t), 0);
	if(pstHeatTable == NULL)
		return 1;
	
	_param_init();
	//ali local quick restore parameter
	if(1 == _quick_restore_init()){
		_param_init();//reinit
	}
	
	//start timer for clear
	SYS_SetTimer(&pstHeatTable->startDelayTimer, (int)ALI_QUICK_START_TIME/10, 
					TIMER_SINGLE, (Timer_Expire_CB)_quick_restore_timer_cb);

	pstHeatTable->oo_ctrl.set = _cw_set_on_off;
	pstHeatTable->oo_ctrl.get = _cw_get_on_off;
	mesh_model_t* tmModel = &pstHeatTable->model[pstHeatTable->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_gens_oo_init(tmModel, &pstHeatTable->oo_ctrl);

	tmModel = &pstHeatTable->model[pstHeatTable->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_alis_vendor_init(tmModel, _ali_heating_table_vendor_rx_cb, NULL);
	
	*mdl = pstHeatTable->model;
	*mcnt = pstHeatTable->model_cnt;

	//if bond
	_ready_to_indicate(true);

//	_key_init();
	return 0;
}

uint8_t ali_heating_table_deinit(mesh_model_t** mdl)
{
	bc_mm_alis_vendor_deinit(&pstHeatTable->model[--pstHeatTable->model_cnt]);
	bc_mm_gens_oo_deinit(&pstHeatTable->model[--pstHeatTable->model_cnt]);
	if(pstHeatTable){
		ke_free(pstHeatTable);
		pstHeatTable = NULL;
	}
	*mdl = NULL;
	return 0;
}
void ali_heating_table_hal_rx(const HAL_TYPE type,const uint8_t *data , uint8_t len)
{
	if(type == HAL_UART){
		m_printf_hex(L_APP, "recv data from uart", data, len);
		if(data[1] == (ALI_COMPANY_ID&0xff)
			&& data[2] == (ALI_COMPANY_ID>>8)){
			m_printf_hex(L_APP, "indicate uart", data+4, len-4);
			bc_mm_ali_vendor_send(pstHeatTable->model[1].lid, data[0], (uint8_t*)data+4, len-4);
		}
	}
}


