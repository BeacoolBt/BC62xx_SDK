#include "ali_socket.h"

#include "mm_defines.h"
#include "meshTimer.h"

#include "bc_flash.h"
#include "bc_sys.h"
#include "bc62xx_gpio.h"

static ali_socket_t* p_ali_socket = NULL;

static void ali_socket_bind_timer_cb(void* param)
{
	uint16_t status;
	if(p_ali_socket->bindStatus == 1){
		return;
	}
	p_ali_socket->bindStatus = 1;

	p_ali_socket->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);

	//model bind
	m_lid_t app_key_lid = 0;
	status = bc_m_key_app_find(0, &app_key_lid);
	if (status != M_ERR_NO_ERROR){
		M_PRINTF(L_FND, "m_tb_key_app_find error");
		return;
	}
	for(uint8_t i = 0; i < p_ali_socket->model_cnt; ++i){
		mesh_model_t* t = &p_ali_socket->model[i];
		status = bc_m_key_model_bind(app_key_lid, t->lid);
		if (status == M_ERR_NO_ERROR){
			// Inform the Model IO manager
			bc_m_mio_bind(t->lid);
		}
	}
	
	//model subscription
	// Add address to the subscription list
	//灯的所有model自行绑定组播地址0xC000，0xCFFF。
	for(uint8_t i = 0; i < p_ali_socket->model_cnt; ++i){
		mesh_model_t* t = &p_ali_socket->model[i];
		//status = bc_m_mio_add_subscription(t->lid, 0xF000);
		bc_m_mio_add_subscription(t->lid, SOCKET_GROUP_ADDR1);
		bc_m_mio_add_subscription(t->lid, SOCKET_GROUP_ADDR2);
		
	}

	mesh_model_t* t = &p_ali_socket->model[p_ali_socket->model_cnt-1];
	bc_m_mio_add_subscription(t->lid, M_ALI_VENDER_ADDR);

	// Set publication parameters
    status = bc_m_mio_set_publi_param(t->lid, M_ALI_VENDER_ADDR, NULL,
                                      app_key_lid, M_ALI_PUBLISH_TTL, M_ALI_PUBLISH_PERIOD,
                                      M_ALI_PUBLISH_RETX, 0);
	M_PRINTF(L_APP, "status[%x]", status);


	uint8_t i = 6;
	uint32_t tTime = mesh_timer_get_time();
	do{
		while(mesh_timer_get_time()-tTime < SOCKET_LED_INTERVAL){//delay 1s
			continue;
		}
		tTime = mesh_timer_get_time();
		ali_socket_ind_status_set(i%2);
		//flash indicator led
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->0);
}

static void ali_socket_btn_get_timer_cb(void* param)
{
	SYS_SetTimer(&p_ali_socket->btnGetTimer, (int)SOCKET_BTN_INTERVAL/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_btn_get_timer_cb);
	uint8_t val;
	//read btn pin level
	val = ali_socket_key_get();
	M_PRINTF(L_APP, "val[%x]", val);
	if(val == 1){
		if(p_ali_socket->btnPressedCnt > SOCKET_SWITH_TIME_CNT
			&& p_ali_socket->btnPressedCnt < SOCKET_RESET_TIME_CNT){
			ali_socket_elec_relay_reverse();
		}
		p_ali_socket->btnPressedCnt = 0;
		return ;
	}
	p_ali_socket->btnPressedCnt++;

	//check pressed time greater than SOCKET_RESET_TIME_CNT
	if(p_ali_socket->btnPressedCnt >= SOCKET_RESET_TIME_CNT){
		//do reset function
		ali_socket_factory_reset();
	}
}

static void ali_socket_ind_sts_timer_cb(void* param)
{
	//reset timer
	SYS_SetTimer(&p_ali_socket->btnGetTimer, M_ALIS_INDICATION_PERIOD/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_ind_sts_timer_cb);

	//send indicate
	uint8_t data[3];
	int len = 0;
	len += bc_mm_alis_set_onoff_status(&data[0]+len, p_ali_socket->oo_ctrl.get());
	m_printf_hex(L_APP, "ali_socket_ind_sts_timer_cb", data, len);
	bc_mm_alis_send_indication(p_ali_socket->model[1].lid, data, len);
}

void ali_socket_prov_state_cb(uint8_t state)
{
	if(state == M_PROV_STARTED){
		//open indicate led
		ali_socket_ind_status_set(1);
		//start 10 minutes timer to switch static adv
		
	}
#ifdef APP_FOR_ALI
	if(state == M_PROV_SUCCEED_APP){
		//flash indicate led 3 times
		
		SYS_SetTimer(&p_ali_socket->bindDelayTimer, (int)500/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_bind_timer_cb);
		return;
	}
#endif 
}

uint8_t ali_socket_prov_state_get(void)
{
	M_PRINTF(L_APP, "bound satus[%d]", p_ali_socket->bindStatus);
	return p_ali_socket->bindStatus;
}

void ali_socket_factory_reset()
{
	bc_m_clear_config();
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	uint8_t i = 10;
	uint32_t t = mesh_timer_get_time();
	do{
		while(mesh_timer_get_time()-t < SOCKET_LED_INTERVAL){//delay 1s
			continue;
		}
		t = mesh_timer_get_time();
		ali_socket_ind_status_set(i%2);
		//flash indicator led
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->0);
	bc_sys_reset();
}

uint8_t ali_socket_init(mesh_model_t** mdl, uint8_t* mcnt)
{
	p_ali_socket = ke_malloc(sizeof(ali_socket_t), 0);
	if(p_ali_socket == NULL)
		return 1;
	memset(p_ali_socket, 0, sizeof(ali_socket_t));
	
	//read saved parameter
	bc_flash_read(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);
	uint8_t tCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	if(p_ali_socket->saveCfgCrc != tCrc){
		memset((uint8_t*)p_ali_socket, 0, ALI_SOCKET_SAVE_LEN);
	}

	if(p_ali_socket->dType != ali_config_device_type_get()){//change device type
		M_PRINTF(L_APP, "device type is changed [%d] to [%d]", p_ali_socket->dType, ali_config_device_type_get());
		bc_m_clear_config();
		memset((uint8_t*)p_ali_socket, 0,  ALI_SOCKET_SAVE_LEN);
		p_ali_socket->dType = ali_config_device_type_get();
		p_ali_socket->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
		bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
		bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);
	}
	
	M_PRINTF(L_APP, "p_ali_socket[%p] eRelayS[%p]", p_ali_socket, &p_ali_socket->eRelayS);
	//control socket 
	ali_socket_elec_relay_init();

	//key init
	ali_socket_key_init();
		
	//start timer for clear

	//start timer for button press;
	SYS_SetTimer(&p_ali_socket->btnGetTimer, (int)SOCKET_BTN_INTERVAL/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_btn_get_timer_cb);

	//start indicate timer
	if(p_ali_socket->bindStatus){
		SYS_SetTimer(&p_ali_socket->btnGetTimer, (int)bc_mm_alis_get_random_time()/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_ind_sts_timer_cb);
	}

	p_ali_socket->oo_ctrl.get = ali_socket_elec_relay_get;
	p_ali_socket->oo_ctrl.set = ali_socket_elec_relay_set;
	bc_mm_gens_oo_init(&p_ali_socket->model[p_ali_socket->model_cnt++], &p_ali_socket->oo_ctrl);
	bc_mm_alis_vendor_init(&p_ali_socket->model[p_ali_socket->model_cnt++], NULL);
	*mdl = p_ali_socket->model;
	*mcnt = p_ali_socket->model_cnt;
	return 0;
}

uint8_t ali_socket_deinit(mesh_model_t** mdl)
{
	bc_mm_alis_vendor_deinit(&p_ali_socket->model[--p_ali_socket->model_cnt]);
	bc_mm_gens_oo_deinit(&p_ali_socket->model[--p_ali_socket->model_cnt]);
	if(p_ali_socket){
		ke_free(p_ali_socket);
		p_ali_socket = NULL;
	}
	*mdl = NULL;
	return 0;
}

void ali_socket_ind_status_set(uint8_t on)
{
	bc_gpio_set(GPIO_PWM1, on);
	bc_gpio_set(GPIO_PWM5, on);
}

void ali_socket_ind_onoff_set(uint8_t on)
{
	bc_gpio_set(GPIO_PWM1, !on);
	bc_gpio_set(GPIO_PWM5, on);
}


void ali_socket_elec_relay_set(uint8_t on)
{
	M_PRINTF(L_APP, "ctrl to [%x]", on);
	bc_gpio_set(GPIO_PWM2, on);
	ali_socket_ind_onoff_set(on);
	
	p_ali_socket->eRelayS = on;

	//indicate status
	if(p_ali_socket->bindStatus){

		SYS_SetTimer(&p_ali_socket->btnGetTimer, (int)bc_mm_alis_get_random_time()/10, 
					TIMER_SINGLE, (Timer_Expire_CB)ali_socket_ind_sts_timer_cb);
	}

	//save status
	p_ali_socket->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	m_printf_hex(L_APP, "ali_socket_elec_relay_set save data", (uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);
}
void ali_socket_elec_relay_reverse(void)
{
	uint8_t on = p_ali_socket->eRelayS;
	on = !on;
	ali_socket_elec_relay_set(on);
}

uint8_t ali_socket_elec_relay_get(void)
{
	return p_ali_socket->eRelayS;
}
uint8_t ali_socket_elec_relay_init()
{
	
	if(p_ali_socket == NULL)
		return 2;
	ali_socket_elec_relay_set(p_ali_socket->eRelayS);
	return 0;
}

void ali_socket_key_init(void)
{
	GPIO_CFG cfg;
	cfg.pin_func = FUNC_INPUT;
	cfg.pin_cfg = GFG_PULLUP;
	bc_gpio_init(GPIO_PWM4, &cfg);
}
uint8_t ali_socket_key_get(void)
{
	return (uint8_t)bc_gpio_get(GPIO_PWM4);
}


