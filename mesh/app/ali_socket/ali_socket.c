#include "ali_socket.h"

#include "mm_defines.h"
#include "meshTimer.h"

#include "bc_flash.h"
#include "bc_sys.h"
#include "bc62xx_gpio.h"
#include "systick.h"

/*Socket button pin*/
#define SOCKET_BTN_PIN		GPIO_PWM1
/*Socket red led pin*/
#define SOCKET_RED_PIN		GPIO_PWM4
/*Socket green led pin*/
#define SOCKET_GREEN_PIN	GPIO_PWM5
/*Socket control pin*/
#define SOCKET_CTRL_PIN		GPIO_PWM2

/*The pointer to socket data structer*/
static ali_socket_t* p_ali_socket = NULL;

static void ali_socket_ready_to_indicate(bool random);

static void ali_socket_ind_sts_timer_cb(int param);


/**
 *@brief Set socket control io value and do status indicate
 *param:on	control val
 *return: None
 */
void ali_socket_ctrl_set(uint8_t on)
{
	M_PRINTF(L_APP, "ctrl to [%x]", on);
	/*Control socket*/
	p_ali_socket->eRelayS = on;
	bc_gpio_set(SOCKET_CTRL_PIN, on);
	
	/*Control red indicate led*/
	bc_gpio_set(SOCKET_RED_PIN, on);

	//save status
	p_ali_socket->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);

	/*Indicate status with random delay*/
	ali_socket_ready_to_indicate(true);
}

/**
 *@brief Get socket control io value
 *param:None
 *return: [0] Low; [1] High
 */
static uint8_t ali_socket_ctrl_get(void)
{
	return p_ali_socket->eRelayS;
}

/**
 *@brief Init socket control io
 *param:None
 *return: [0] Success; [1] Fail
 */
static uint8_t ali_socket_ctrl_init(void)
{
	bc_gpio_set(SOCKET_CTRL_PIN, p_ali_socket->eRelayS);
	/*Control red indicate led*/
	bc_gpio_set(SOCKET_RED_PIN, p_ali_socket->eRelayS);
	return 0;
}

/**
 *@brief Init Button io pin with interrupt function
 *param:None
 *return:None
 */
static void ali_socket_btn_init(void)
{
	/*Button io init*/
	GPIO_CFG cfg;
	cfg.pin_func = FUNC_INPUT;
	cfg.pin_cfg = GFG_PULLUP;
	bc_gpio_init(SOCKET_BTN_PIN, &cfg);

	/*init key status*/
	p_ali_socket->skey_pre = BTN_INVALID_VAL;
	p_ali_socket->skey_next = BTN_INVALID_VAL;
}

static void ali_socket_ready_to_indicate(bool random)
{
	if(p_ali_socket == NULL)
		return;
	uint32_t delay_time = M_ALIS_INDICATION_PERIOD;
	if(random){
		delay_time = bc_mm_alis_get_random_time();
	}
	
	//if bound
	if(p_ali_socket->provState){
		SYS_SetTimer(&p_ali_socket->periodIndTimer, (int)delay_time/10, 
					TIMER_SINGLE, ali_socket_ind_sts_timer_cb);
	}
}

/**
 *@brief Do after proved
 *param:param	No use
 *return: None
 */
static void ali_socket_bind_timer_cb(void* param)
{
	uint16_t status;
	if(p_ali_socket->provState == 1){
		return;
	}
	p_ali_socket->provState = 1;

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


	/*Do 3 times green led flashing*/
	uint8_t i = SOCKET_QUICK_3_TIMES<<1;
	uint32_t tTime = mesh_timer_get_time();
	do{
		while(mesh_timer_get_time()-tTime < SOCKET_LED_INTERVAL){//delay 1s
			continue;
		}
		tTime = mesh_timer_get_time();
		bc_gpio_set(SOCKET_GREEN_PIN, i%2);
		//flash indicator led
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->0);
	/*close green led*/

	//disconnect and disable proxy when prov success
	extern void mesh_con_stop(uint16_t handle);
	mesh_con_stop(0);//disconncet ble
	bc_m_proxy_ctrl(false);
}

static void ali_socket_ind_sts_timer_cb(int param)
{
	/*Reset timer*/
	SYS_SetTimer(&p_ali_socket->periodIndTimer, M_ALIS_INDICATION_PERIOD/10, 
					TIMER_SINGLE, ali_socket_ind_sts_timer_cb);

	/*Assemble packet*/
	uint8_t data[3];
	int len = 0;
	len += bc_mm_alis_set_onoff_status(&data[0]+len, ali_socket_ctrl_get());
	m_printf_hex(L_APP, "ali_socket_ind_sts_timer_cb", data, len);
	/*Send indicate packet*/
	bc_mm_alis_send_indication(p_ali_socket->model[1].lid, data, len);
}

void ali_socket_prov_state_cb(uint8_t state)
{
	if(state == M_PROV_STARTED){
		//When prov start, open green led.
		bc_gpio_set(SOCKET_GREEN_PIN, 1);
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
	M_PRINTF(L_APP, "bound satus[%d]", p_ali_socket->provState);
	return p_ali_socket->provState;
}

void ali_socket_factory_reset()
{
	/*Do 5 times green led flashing*/
	uint8_t i = SOCKET_QUICK_5_TIMES<<1;
	uint32_t t = mesh_timer_get_time();
	/*Do led flashing*/
	do{
		/*Do delay*/
		while(mesh_timer_get_time()-t < SOCKET_LED_INTERVAL){
			continue;
		}
		t = mesh_timer_get_time();
		/*Control Green Led*/
		bc_gpio_set(SOCKET_GREEN_PIN, i%2);
		//flash indicator led
		M_PRINTF(L_APP, "led status = %d", i%2);
	}while(i-->0);

	/*Clear mesh parameter*/
	bc_m_clear_config();
	/*Clear socket parameter*/
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, FLASH_ALI_USED_PAGE_SIZE);
	/*Do reset*/
	bc_sys_reset();
}

/*This tick handler is triggered per 1ms*/
void ali_socket_tick_handler(void)
{
	/*Used for button check*/
	/*Read current value*/
	if(p_ali_socket == NULL)
		return;
	p_ali_socket->skey_next = bc_gpio_get(SOCKET_BTN_PIN);
	/*No press event happed*/
	if(p_ali_socket->skey_next == BTN_INVALID_VAL && p_ali_socket->skey_pre == BTN_INVALID_VAL){
		return;
	}
	/*Button is pressed at now*/
	else if(p_ali_socket->skey_next == BTN_VALID_VAL && p_ali_socket->skey_pre == BTN_INVALID_VAL){
		/*Record pressed time*/
		p_ali_socket->btnPresTime = systick_get_ms();
	}
	/*Button is pressing*/
	else if(p_ali_socket->skey_next == BTN_VALID_VAL && p_ali_socket->skey_pre == BTN_VALID_VAL){
		/*Check long press event*/
		if(systick_get_ms()- p_ali_socket->btnPresTime > SOCKET_RESET_TIME_CNT){
			/*delay 50ms for reset*/
			if(!SYS_TimerisExist(&p_ali_socket->delayTimer)){
				SYS_SetTimer(&p_ali_socket->delayTimer, (int)5, 
						TIMER_SINGLE, (Timer_Expire_CB)ali_socket_factory_reset);
				///*Do factory reset*/
				//ali_socket_factory_reset();
			}
		}
	}
	/*Button is pressed at now*/
	else{
		if(!SYS_TimerisExist(&p_ali_socket->delayTimer)){
			/*do socket control*/
			ali_socket_ctrl_set(!p_ali_socket->eRelayS);
		}
	}
	
	/*save current button value*/
	p_ali_socket->skey_pre = p_ali_socket->skey_next;
}

/**
 *@brief Set ali socket default parameter
 *param: None
 *return: None
 */
static void ali_socket_reload_defalut_param(void)
{
	memset((uint8_t*)p_ali_socket, 0, ALI_SOCKET_SAVE_LEN);
	/*Default socket status is closed*/
	//p_ali_socket->eRelayS = 0;
	
	p_ali_socket->dType = ali_config_device_type_get();
	p_ali_socket->saveCfgCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	bc_flash_erase(FLASH_ALI_START_ADDRRESS, 1);
	bc_flash_write(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);
}

/**
 *@brief init socket parameter
 *param: None
 *return: None
 */
static void ali_socket_param_init(void)
{
	memset(p_ali_socket, 0, sizeof(ali_socket_t));

	/*Read saved parameter*/
	bc_flash_read(FLASH_ALI_START_ADDRRESS, ALI_SOCKET_SAVE_LEN,(uint8_t*)p_ali_socket);
	/*Caculate the crc value*/
	uint8_t tCrc = bc_sys_check_sum((uint8_t*)p_ali_socket, ALI_SOCKET_SAVE_LEN-1);
	/*Match the crc value*/
	if(p_ali_socket->saveCfgCrc != tCrc){
		/*Do parameter reset*/
		ali_socket_reload_defalut_param();
		return;
	}
	/*Check saved device type is socket of not*/
	if(p_ali_socket->dType != ali_config_device_type_get()){//change device type
		M_PRINTF(L_APP, "device type is changed [%d] to [%d]", p_ali_socket->dType, ali_config_device_type_get());
		/*Do mesh Parameter clear*/
		bc_m_clear_config();
		/*Load default socket's parameter*/
		ali_socket_reload_defalut_param();
	}
}

uint8_t ali_socket_init(mesh_model_t** mdl, uint8_t* mcnt)
{
	/*Malloc memeory for socket*/
	p_ali_socket = ke_malloc(sizeof(ali_socket_t), 0);//get socket memory
	if(p_ali_socket == NULL)
		return 1;
	
	/*Init socket parameter*/
	ali_socket_param_init();

	/*Button init*/
	ali_socket_btn_init();
	
	/*Control socket*/
	ali_socket_ctrl_init();

	/*if proved, close Green led*/
	if(p_ali_socket->provState){
		bc_gpio_set(SOCKET_GREEN_PIN, 0);
	}
	
	/*Init generic onoff model*/
	p_ali_socket->oo_ctrl.get = ali_socket_ctrl_get;
	p_ali_socket->oo_ctrl.set = ali_socket_ctrl_set;
	bc_mm_gens_oo_init(&p_ali_socket->model[p_ali_socket->model_cnt++], &p_ali_socket->oo_ctrl);

	/*Init Ali vendor model*/
	bc_mm_alis_vendor_init(&p_ali_socket->model[p_ali_socket->model_cnt++], NULL, NULL);
	*mdl = p_ali_socket->model;
	*mcnt = p_ali_socket->model_cnt;

	/*Indicate status with random delay*/
	ali_socket_ready_to_indicate(true);
	return 0;
}

uint8_t ali_socket_deinit(mesh_model_t** mdl)
{
	if(p_ali_socket) return 1;
	/*Release generic onoff model*/
	bc_mm_alis_vendor_deinit(&p_ali_socket->model[--p_ali_socket->model_cnt]);
	/*Release Ali vendor model*/
	bc_mm_gens_oo_deinit(&p_ali_socket->model[--p_ali_socket->model_cnt]);
	/*Free socket memory*/
	ke_free(p_ali_socket);
	/*Reset pointer*/
	p_ali_socket = NULL;
	*mdl = NULL;
	return 0;
}

