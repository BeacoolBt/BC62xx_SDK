#include "ali_light.h"
#include "mm_defines.h"
#include "meshTimer.h"

#include "mm_ali.h"
#include "bc_flash.h"
#include "bc_sys.h"

#ifdef BREATH_CONFIG
#define REPEAT_COUNTER	(100)
#endif

static ali_light_t* p_ali_light = NULL;

static void ali_light_period_ind_timer_cb(void* param);

static void _ready_to_indicate(bool random)
{
	if(p_ali_light == NULL)
		return;
	uint32_t delay_time = M_ALIS_INDICATION_PERIOD;
	if(random){
		delay_time = bc_mm_alis_get_random_time();
	}
	
	//if bound
	if(p_ali_light->provState){
		SYS_SetTimer(&p_ali_light->periodIndTimer, (int)delay_time/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_light_period_ind_timer_cb);
	}
}


//light ctrl
static void _light_cw_init(void)
{
	static uint8_t _init = 0;
	if(_init == 1)
		return;
	_init = 1;
#ifdef BREATH_CONFIG
	light_cw_t t = {.freq = ALI_CURRENT_FREQ,
					.ln = ALI_MIN_LN_VAL,
					.on = ALI_CW_OFF,
					.tp = ALI_DEFAULT_TP_VAL};
#else
	light_cw_t t = {.freq = ALI_CURRENT_FREQ,
					.ln = ALI_DEFAULT_LN_VAL,
					.on = ALI_DEFAULT_ON_VAL,
					.tp = ALI_DEFAULT_TP_VAL};
#endif
	t.tp = p_ali_light->tp;
	t.ln = p_ali_light->ln;
#if (LIGHT_CW_WITH_RGB)
	t.rVal = p_ali_light->r;
	t.gVal = p_ali_light->g;
	t.bVal = p_ali_light->b;
#endif
	led_cw_init(&t);
}
void ali_light_cw_set_on_off(uint8_t on)
{
#ifdef BREATH_CONFIG
	p_ali_light->breath.mode = BREATH_MODE_FLOW;
	p_ali_light->breath.rCnt = 1;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_on_off(on, &p_ali_light->breath);
#else
	led_cw_set_on_off(on, NULL);
#endif
}
uint8_t ali_light_cw_get_on_off(void)
{
	return led_cw_get_on_off();
}
void ali_light_cw_set_lightness(uint16_t ln)
{
#ifdef BREATH_CONFIG
	if(ln < ALI_MIN_LN_VAL){
		ln = ALI_MIN_LN_VAL;
	}
	p_ali_light->breath.mode = BREATH_MODE_FLOW;
	p_ali_light->breath.rCnt = 1;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_lightness(ln, &p_ali_light->breath);
#else
	led_cw_set_lightness(ln, NULL);
#endif
	p_ali_light->ln = ln;
	//do save
	p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
}
uint16_t ali_light_cw_get_lightness(void)
{
	return led_cw_get_lightness();
}
void ali_light_cw_set_temperature(uint16_t tp)
{
#ifdef BREATH_CONFIG
	p_ali_light->breath.mode = BREATH_MODE_FLOW;
	p_ali_light->breath.rCnt = 1;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_temperature(tp, &p_ali_light->breath);
#else
	led_cw_set_temperature(tp, NULL);
#endif
	p_ali_light->tp = tp;
	//do save 
	p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
}
uint16_t ali_light_cw_get_temperature(void)
{
	return led_cw_get_temperature();
}

void ali_light_ltn_set_on_off(uint8_t on)
{
	led_ltn_set_on_off(on);
}
uint8_t ali_light_ltn_get_on_off(void)
{
	return led_ltn_get_on_off();
}

void ali_light_ltn_set_lightness(uint16_t ln)
{
	led_ltn_set_lightness(ln);
}
uint16_t ali_light_ltn_get_lightness(void)
{
	return led_ltn_get_lightness();
}
int ali_light_rgb_get(uint16_t* l, uint16_t* h, uint16_t* s)
{
	*l = 0;
	*h = 0;
	*s = 0;
	return 0;
}
int ali_light_rgb_set(uint16_t l, uint16_t h, uint16_t s)
{
#if (LIGHT_CW_WITH_RGB)
	uint16_t r,g,b;
	if(l == 0x8000 && h == 0xaaaa && s == 0xffff){
		//blue (0,0,255)
		M_PRINTF(L_APP, "color blue");
		r = 0;
		g = 0;
		b = 65535;
	}
	else if(l == 0x8000 && h == 0x0000 && s == 0xffff){
		//red (255,0,0)
		M_PRINTF(L_APP, "color red");
		r = 65535;
		g = 0;
		b = 0;
	}
	else if(l == 0x8000 && h == 0x5555 && s == 0xffff){
		//green (0, 255, 0)
		M_PRINTF(L_APP, "color green");
		r = 0;
		g = 65535;
		b = 0;
	}
	else if(l == 0x8000 && h == 0x8000 && s == 0xffff){
		//cyan (0, 255, 255)
		M_PRINTF(L_APP, "color cyan");
		r = 0;
		g = 65535;
		b = 65535;
	}
	else if(l == 0x4040 && h == 0xD555 && s == 0xffff){
		//Purple (128, 0, 128)
		M_PRINTF(L_APP, "color Purple");
		r = 32768;
		g = 0;
		b = 32768;
	}
	else if(l == 0x8000 && h == 0x1b9b && s == 0xffff){
		//orange (255, 170, 0)
		M_PRINTF(L_APP, "color orange");
		r = 65535;
		g = 43520;
		b = 0;
	}
	else if(l == 0x8000 && h == 0x2aaa && s == 0xffff){
		//yellow (255,255,0)
		M_PRINTF(L_APP, "color yellow");
		r = 65535;
		g = 65535;
		b = 0;
	}
	else if(l == 0xffff && h == 0x0000 && s == 0x0000){
		//write (255,255,255)
		M_PRINTF(L_APP, "color write");
		r = 65535;
		g = 65535;
		b = 65535;
	}
	else{
		M_PRINTF(L_APP, "color error");
		return 0;
	}

	led_cwrgb_set_color(r, g, b, NULL);
	p_ali_light->r = r;
	p_ali_light->g = g;
	p_ali_light->b = b;
	//do save
	p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
#endif
	return 0;
}
static void ali_light_bind_timer_cb(void* param)
{
	uint16_t status;

	if(!p_ali_light->provState){
		p_ali_light->provState = 1;
		p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
		bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
		bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
	}
	//model bind
	m_lid_t app_key_lid = 0;
	status = bc_m_key_app_find(0, &app_key_lid);
	if (status != M_ERR_NO_ERROR){
		M_PRINTF(L_FND, "m_tb_key_app_find error");
		return;
	}
	for(uint8_t i = 0; i < p_ali_light->model_cnt; ++i){
		mesh_model_t* t = &p_ali_light->model[i];
		status = bc_m_key_model_bind(app_key_lid, t->lid);
		if (status == M_ERR_NO_ERROR){
			// Inform the Model IO manager
			bc_m_mio_bind(t->lid);
		}
	}
	
	//model subscription
	// Add address to the subscription list
	//灯的所有model自行绑定组播地址0xC000，0xCFFF。
	for(uint8_t i = 0; i < p_ali_light->model_cnt; ++i){
		mesh_model_t* t = &p_ali_light->model[i];
		//status = bc_m_mio_add_subscription(t->lid, 0xF000);
		bc_m_mio_add_subscription(t->lid, ALI_GROUP_ADDRESS_LIGHT);
		bc_m_mio_add_subscription(t->lid, 0xCFFF);
		
	}
	mesh_model_t* t = &p_ali_light->model[p_ali_light->model_cnt-1];
	bc_m_mio_add_subscription(t->lid, M_ALI_VENDER_ADDR);

	// Set publication parameters
    status = bc_m_mio_set_publi_param(t->lid, M_ALI_VENDER_ADDR, NULL,
                                      app_key_lid, M_ALI_PUBLISH_TTL, M_ALI_PUBLISH_PERIOD,
                                     M_ALI_PUBLISH_RETX, 0);
	M_PRINTF(L_APP, "t->lid[%x] p_ali_light->model_cnt[%x] status[%x]",t->lid, p_ali_light->model_cnt, status);

#ifdef BREATH_CONFIG
	p_ali_light->breath.mode = BREATH_MODE_H2L2H;
	p_ali_light->breath.rCnt = 3;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_breath(&p_ali_light->breath);
#endif
	_ready_to_indicate(true);

	mesh_con_stop(0);//disconncet ble
}

static void ali_light_period_ind_timer_cb(void* param)
{
	uint8_t data[12];
	
	int len = 1;
	data[0] = 0;
	len += bc_mm_alis_set_onoff_status(&data[len], ali_light_cw_get_on_off());
	len += bc_mm_alis_set_ltn_status(&data[len], ali_light_cw_get_lightness());
	len += bc_mm_alis_set_tmp_status(&data[len], ali_light_cw_get_temperature());
	
	m_printf_hex(L_APP, "ali_light_period_ind_timer_cb", data, len);
	bc_mm_ali_vendor_send(p_ali_light->model[3].lid, MM_MSG_ALIS_ATTR_INDICATION, data, len);
	_ready_to_indicate(false);
	bc_m_ali_save_seq_iv();
}

static void ali_light_quick_restore_timer_cb(void *t)
{
	M_PRINTF(L_APP, "");
	p_ali_light->quickTime = 0;
	p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
}

uint8_t ali_light_quick_restore_init(void)
{
	if(p_ali_light->quickTime++ < ALI_QUICK_START_CNT){//0 1 2 3 4
		p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
		bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
		bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
		return 0;
	}
	M_PRINTF(L_APP, "");
#ifdef BREATH_CONFIG
	p_ali_light->breath.mode = BREATH_MODE_FLOW;
	p_ali_light->breath.rCnt = 1;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_on_off(ALI_CW_ON, &p_ali_light->breath);

	p_ali_light->breath.mode = BREATH_MODE_H2L2H;
	p_ali_light->breath.rCnt = 5;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_breath(&p_ali_light->breath);
#else
	//TODO: 1HZ flash 3 timers
	uint8_t i = 6;
	uint32_t t = mesh_timer_get_time();
	do{
		while(mesh_timer_get_time()-t < ALI_QUICK_BLINK_TIME){//delay 1s
			continue;
		}
		t = mesh_timer_get_time();
		if(i == 1){
			break;
		}
		led_cw_set_on_off(i%2,NULL);
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->1);
#endif
	//restore flash parameter
	bc_m_clear_config();
	//clear quick count
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);

	bc_sys_reset();
	return 1;
}

void ali_light_prov_state_cb(uint8_t state)
{
#ifdef APP_FOR_ALI
	if(state == M_PROV_SUCCEED_APP){
		SYS_SetTimer(&p_ali_light->bindDelayTimer, (int)50/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_light_bind_timer_cb);
		return;
	}
#endif 
}

uint8_t ali_light_prov_state_get(void)
{
	M_PRINTF(L_APP, "bond status[%d]", p_ali_light->provState);
	return p_ali_light->provState == 1;
}

void ali_light_factory_reset()
{
	bc_m_clear_config();
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
#ifdef BREATH_CONFIG
	p_ali_light->breath.mode = BREATH_MODE_H2L2H;
	p_ali_light->breath.rCnt = 5;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_breath(&p_ali_light->breath);
#endif
	bc_sys_reset();
}
#ifdef FOR_ALI_TEST
static void ali_light_key_timer_cb(void* param)
{
	p_ali_light->key_pre = p_ali_light->key_next;
	p_ali_light->key_next = bc_gpio_get(GPIO_PWM1);
	if(p_ali_light->key_next == 1 && p_ali_light->key_pre == 0){
		ali_light_cw_set_on_off(!ali_light_cw_get_on_off());
		//use vedor to indicate
		_ready_to_indicate(true);
	}
	enable_intr(GPIO6_intr);
}
void gpio6_handler(void)
{
	//printf("gpio6_handler\r\n");
	//start enable irq timer
	disable_intr(GPIO6_intr);
	SYS_SetTimer(&p_ali_light->keyTimer, (int)200/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_light_key_timer_cb);
}
void ali_light_key_init()
{
	GPIO_CFG cfg;
	cfg.pin_func = FUNC_INPUT;
	cfg.pin_cfg = GFG_PULLDOWN;
	bc_gpio_init(GPIO_PWM1, &cfg);

	enable_intr(GPIO6_intr);

	p_ali_light->key_pre = 0;
	p_ali_light->key_next = 0;
}
#endif
static void _reload_defalut_param()
{
	memset((uint8_t*)p_ali_light, 0, ALI_LIGHT_SAVE_LEN);
	p_ali_light->dType = ali_config_device_type_get();
	p_ali_light->on = ALI_DEFAULT_ON_VAL;
	p_ali_light->ln = ALI_DEFAULT_LN_VAL;
	p_ali_light->tp = ALI_DEFAULT_TP_VAL;
	#if (LIGHT_CW_WITH_RGB)
	p_ali_light->r = ALI_DEFAULT_R_VAL;
	p_ali_light->g = ALI_DEFAULT_G_VAL;
	p_ali_light->b = ALI_DEFAULT_B_VAL;
	#endif
	p_ali_light->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
}
uint8_t ali_light_param_init()
{
	memset(p_ali_light, 0, sizeof(ali_light_t));
	//read saved parameter
	bc_flash_read(FLASH_ALI_START_ADDRRESS, ALI_LIGHT_SAVE_LEN,(uint8_t*)p_ali_light);
	uint8_t tCrc = bc_sys_check_sum((uint8_t*)p_ali_light, ALI_LIGHT_SAVE_LEN-1);
	if(p_ali_light->saveCfgCrc != tCrc){
		_reload_defalut_param();
	}else{
		if(p_ali_light->dType != ali_config_device_type_get()){//change device type
			M_PRINTF(L_APP, "device type is changed [%d] to [%d]", p_ali_light->dType, ali_config_device_type_get());
			bc_m_clear_config();
			_reload_defalut_param();
		}
	}

	if(p_ali_light->dType == PID_CW){
		_light_cw_init();
	}else{//PID_LTN
		led_ltn_init(NULL);
	}
	return 0;
}

static void _ali_vendor_rx_cb(const m_lid_t model_lid, const uint8_t opcode, const uint8_t* data, const uint16_t len)
{
	if(model_lid != p_ali_light->model[3].lid){
		return;
	}
	M_PRINTF(L_APP, "opcode[%x]", opcode);
	switch(opcode){
	case MM_MSG_ALIS_ATTR_CONFIRMATION:{
		m_printf_hex(L_APP, "ALI VENDOR RECV", data, len);
	}break;
	case MM_MSG_ALIS_ATTR_SET:{
		m_printf_hex(L_APP, "ALI VENDOR SET RECV", data, len);
		static uint8_t tid = 0;
		if(tid != data[0]){
			tid = data[0];
			uint16_t cmd = data[1] + (data[2]<<8);
			if(cmd == 0x0123){
				uint16_t l = data[3] + (data[4]<<8);
				uint16_t h = data[5] + (data[6]<<8);
				uint16_t s = data[7] + (data[8]<<8);
				ali_light_rgb_set(l,h,s);
			}
		}
		//do response
		bc_mm_ali_vendor_send(model_lid, MM_MSG_ALIS_ATTR_STATUS, (uint8_t*)data, len);
	}break;
	default:break;
	}
}

uint8_t ali_light_init(mesh_model_t** mdl, uint8_t* mcnt)
{
	M_PRINTF(L_APP, "-------init light---------");
	p_ali_light = ke_malloc(sizeof(ali_light_t), 0);
	if(p_ali_light == NULL)
		return 1;
	
	ali_light_param_init();
	//ali local quick restore parameter
	ali_light_quick_restore_init();
#ifdef BREATH_CONFIG
	//open light 
	p_ali_light->breath.mode = BREATH_MODE_FLOW;
	p_ali_light->breath.rCnt = 1;
	p_ali_light->breath.pCnt = REPEAT_COUNTER;
	led_cw_set_on_off(ALI_CW_ON, &p_ali_light->breath);
#endif
	
	//start timer for clear
	SYS_SetTimer(&p_ali_light->startDelayTimer, (int)ALI_QUICK_START_TIME/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_light_quick_restore_timer_cb);

	if(p_ali_light->dType == PID_CW){
		p_ali_light->oo_ctrl.set = ali_light_cw_set_on_off;
		p_ali_light->oo_ctrl.get = ali_light_cw_get_on_off;
	}else{//PID_LTN
		p_ali_light->oo_ctrl.set = ali_light_ltn_set_on_off;
		p_ali_light->oo_ctrl.get = ali_light_ltn_get_on_off;
	}
	mesh_model_t* tmModel = &p_ali_light->model[p_ali_light->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_gens_oo_init(tmModel, &p_ali_light->oo_ctrl);

	if(p_ali_light->dType == PID_CW){
		p_ali_light->light_ln_ctrl.set = ali_light_cw_set_lightness;
		p_ali_light->light_ln_ctrl.get = ali_light_cw_get_lightness;
	}else{//PID_LTN
		p_ali_light->light_ln_ctrl.set = ali_light_ltn_set_lightness;
		p_ali_light->light_ln_ctrl.get = ali_light_ltn_get_lightness;
	}
	tmModel = &p_ali_light->model[p_ali_light->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_light_ln_init(tmModel, &p_ali_light->light_ln_ctrl);

	p_ali_light->light_ctl_ctrl.ctl_set = ali_light_cw_set_temperature;
	p_ali_light->light_ctl_ctrl.ctl_get = ali_light_cw_get_temperature;
	p_ali_light->light_ctl_ctrl.ln_set = ali_light_cw_set_lightness;
	p_ali_light->light_ctl_ctrl.ln_get = ali_light_cw_get_lightness;
	tmModel = &p_ali_light->model[p_ali_light->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_light_ctl_init(tmModel, &p_ali_light->light_ctl_ctrl);

	tmModel = &p_ali_light->model[p_ali_light->model_cnt++];
	tmModel->offset = 0;//element num
	bc_mm_alis_vendor_init(tmModel, _ali_vendor_rx_cb);
	
	*mdl = p_ali_light->model;
	*mcnt = p_ali_light->model_cnt;

	_ready_to_indicate(true);

#ifdef FOR_ALI_TEST
	ali_light_key_init();
#endif
	return 0;
}

uint8_t ali_light_deinit(mesh_model_t** mdl)
{
	bc_mm_alis_vendor_deinit(&p_ali_light->model[--p_ali_light->model_cnt]);
	bc_mm_light_ctl_deinit(&p_ali_light->model[--p_ali_light->model_cnt]);
	bc_mm_light_ln_deinit(&p_ali_light->model[--p_ali_light->model_cnt]);
	bc_mm_gens_oo_deinit(&p_ali_light->model[--p_ali_light->model_cnt]);
	if(p_ali_light){
		ke_free(p_ali_light);
		p_ali_light = NULL;
	}
	*mdl = NULL;
	return 0;
}

void ali_light_hal_rx(const HAL_TYPE type,const uint8_t *data , uint8_t len)
{
	if(type == HAL_TST){
#ifdef CONFIG_FACTORY_TEST
		if(p_ali_light->provState != 1 && len == 25 
			&&data[len-4] == 0x78 && data[len-3] == 0x56
			&&data[len-2] == 0x34 && data[len-1] == 0x12
			){
			
			static uint8_t tid;
			if(tid == data[9]){
				return;
			}
			tid = data[9];
			m_printf_hex(L_APP, "RECV FROM HAL_TST", data, len);
			//do light test
			const uint8_t*attVal = data+10;
			uint8_t attLen = len-10-4;
			uint8_t pos = 0;
			do{
				const uint8_t* attTmp = attVal+pos;
				uint16_t cmd = attTmp[0]+(attTmp[1]<<8);
				M_PRINTF(L_APP, "CMD[%x]", cmd);
				switch(cmd){
					case M_ATTR_ONOFF:{
						pos+=3;
						led_cw_set_on_off(attTmp[2], NULL);
						M_PRINTF(L_APP, "M_ATTR_ONOFF[%x]", attTmp[2]);
					}break;
					case M_ATTR_LTN:{
						uint16_t val = attTmp[2]+(attTmp[3]<<8);
						pos+=4;
						led_cw_set_lightness(val, NULL);
						M_PRINTF(L_APP, "M_ATTR_LTN[%x]", val);
					}break;
					case M_ATTR_TEMP:{
						uint16_t val = attTmp[2]+(attTmp[3]<<8);
						pos+=4;
						led_cw_set_temperature(val, NULL);
						M_PRINTF(L_APP, "M_ATTR_TEMP[%x]", val);
					}break;
					default:return;
				}
			}while(pos<attLen);
		}
#endif
	}
}

