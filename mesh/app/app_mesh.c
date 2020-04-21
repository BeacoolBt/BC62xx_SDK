#include "m_api.h"
#include "systick.h"

#include "app_mesh.h"
#include "co_com.h"

#include "meshTimer.h"
#include "bc62xx_gpio.h"

#include "mm_alis.h"
#include "ali_light.h"
#include "ali_socket.h"
#include "ali_heatingTable.h"


#define ALI_PROV_TIMEOUT_TIME (600000)//10mins 

#if (BLE_MESH)	
const char *app_mesh_prov_state_str[] ={
	"M_PROV_STARTED",
	"M_PROV_SUCCEED",
	"M_PROV_FAILED",
	"M_PROV_SUCCEED_APP",
};

static ali_app_t* ali_app;

const app_t ali_app_array[] = 
{
	//ali_light
	{	
		.factory_reset = ali_light_factory_reset,
		.prov_state_cb = ali_light_prov_state_cb,
		.prov_state_get = ali_light_prov_state_get,
		.app_init = ali_light_init,
		.app_deinit = ali_light_deinit,
		.hal_rx = ali_light_hal_rx,
		.tick_handler = NULL,
	},
	{	
		.factory_reset = ali_light_factory_reset,
		.prov_state_cb = ali_light_prov_state_cb,
		.prov_state_get = ali_light_prov_state_get,
		.app_init = ali_light_init,
		.app_deinit = ali_light_deinit,
		.hal_rx = NULL,
		.tick_handler = NULL,
	},
	//ali_socket = 
	{	
		.factory_reset = ali_socket_factory_reset,
		.prov_state_cb = ali_socket_prov_state_cb,
		.prov_state_get = ali_socket_prov_state_get,
		.app_init = ali_socket_init,
		.app_deinit = ali_socket_deinit,
		.hal_rx = NULL,
		.tick_handler = ali_socket_tick_handler,
	},
	//ali_HeatingTable = 
	{	
		.factory_reset = ali_heating_table_factory_reset,
		.prov_state_cb = ali_heating_table_prov_state_cb,
		.prov_state_get = ali_heating_table_prov_state_get,
		.app_init = ali_heating_table_init,
		.app_deinit = ali_heating_table_deinit,
		.hal_rx = ali_heating_table_hal_rx,
		.tick_handler = NULL,
	},
};
__STATIC void app_mesh_enabled_cb(uint16_t status)
{
	M_PRINTF(L_APP, "");
}

__STATIC void app_mesh_disabled_cb(uint16_t status)
{
	M_PRINTF(L_APP, "");
}
static void app_mesh_prov_timeout_timer_cb(void* param)
{
	bc_m_bcn_stop_tx_unprov_bcn();
}

__STATIC void app_mesh_prov_state_cb(uint8_t state, uint16_t status)
{
	
	M_PRINTF(L_APP, "state[%s]", app_mesh_prov_state_str[state]);
	if(state == M_PROV_STARTED){
		/*Start 10 mins prov timeout timer*/
		SYS_SetTimer(&ali_app->provTimeoutTimer, (int)ALI_PROV_TIMEOUT_TIME/10, 
					TIMER_SINGLE, (Timer_Expire_CB)app_mesh_prov_timeout_timer_cb);

		/*Clear saved provisioned parameters*/
		//bc_m_clear_config();
	}
	if(state == M_PROV_SUCCEED_APP){
		bc_m_proxy_ctrl(false);
		SYS_ReleaseTimer(&ali_app->provTimeoutTimer);
	}

	ali_app->app_t->prov_state_cb(state);
}
__STATIC void app_mesh_prov_param_req_cb(void)
{
	m_api_prov_param_cfm_t cfm;
	ali_uuid_t uuid;
	ali_config_uuid_read(&uuid);
	memcpy(cfm.dev_uuid ,&uuid, M_DEV_UUID_LEN);
	m_printf_hex(L_APP, "prov param req cb", cfm.dev_uuid, M_DEV_UUID_LEN);
	
	cfm.static_oob = M_PROV_STATIC_OOB_AVAILABLE;
	cfm.pub_key_oob = M_PROV_PUB_KEY_OOB_NOT_USED;
	cfm.out_oob_size = 1;
	cfm.in_oob_size = 0;
	cfm.out_oob_action = 0;
	cfm.in_oob_action = 0;
	cfm.nb_elt = 1;
	cfm.info = 0;

	bc_m_prov_param_rsp(&cfm);
}
__STATIC void app_mesh_prov_oob_auth_req_cb(uint8_t auth_method, uint16_t auth_action, uint8_t auth_size)
{
	uint8_t auth_data[16];
	
	M_PRINTF(L_APP,"auth_method:0x%02x,auth_action:0x%04x,auth_size:0x%02x", auth_method, auth_action, auth_size);

	ali_config_auth_val_read(auth_data);
									
	m_printf_hex(L_APP, "auth val request", auth_data, 16);
	
    bc_m_prov_oob_auth_rsp(1, auth_size, auth_data);
}
__STATIC void app_mesh_buf_block_freed_cb(uint8_t block_id)
{
	M_PRINTF(L_APP, "");
}
__STATIC void app_mesh_loaded_cb(uint16_t status)
{
	M_PRINTF(L_APP, "");
}

__STATIC void app_mesh_api_update_ind_cb(uint8_t type, uint16_t len, uint8_t *data)
{
	bc_m_param_save(type, len, data);
}

__STATIC void app_mesh_reset_ind_cb(void)
{
	M_PRINTF(L_APP, "");
	ali_app->app_t->factory_reset();
	bc_m_disable();
}

__STATIC void app_mesh_compo_data_req_cb(uint8_t page)
{
	M_PRINTF(L_APP, "");
	uint8_t compo_data[] = {0xfc, 0x02, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x01, 0x00, 0x01, 0x00, 0x05, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x13, 0x03, 0x13};
	uint8_t data_len;

	page = 0;

	if(page == 0x00) {
		data_len = sizeof(compo_data);
	}
	else {
		data_len = 1;
	}

	bc_m_compo_data_cfm(page, data_len, compo_data);
}

/// Mesh call-backs structure for Message API
__STATIC const m_api_cb_t app_mesh_cb = {
    .cb_enabled         = app_mesh_enabled_cb,
    .cb_disabled        = app_mesh_disabled_cb,
    .cb_prov_state      = app_mesh_prov_state_cb,
    .cb_prov_param_req  = app_mesh_prov_param_req_cb,
    .cb_prov_auth_req   = app_mesh_prov_oob_auth_req_cb,
    .cb_release         = app_mesh_buf_block_freed_cb,
    .cb_loaded          = app_mesh_loaded_cb,
 	.cb_update_ind		= app_mesh_api_update_ind_cb,
 	.cb_reset_ind		= app_mesh_reset_ind_cb,
 	.cb_compo_data_req	= app_mesh_compo_data_req_cb,
};
uint8_t app_mesh_init(void)
{
	/*Init mesh stack*/
	bc_m_mesh_init();

	/*Malloc memory for Genie product*/
	ali_app = ke_malloc(sizeof(ali_app_t), 0);
	/*Clear memroy*/
	memset(ali_app, 0, sizeof(ali_app_t));
	
	/*Get Genie device type*/
	uint8_t dType = ali_config_device_type_get();	
	M_PRINTF(L_APP, "device type[%d]", dType);
	/*Get device structer*/
	ali_app->app_t = &ali_app_array[dType];

	/*Init device*/
	ali_app->app_t->app_init(&ali_app->model, &ali_app->model_cnt);

	/*Set systick callback function with 1ms interval*/
	systick_set_tick_irq_handler(ali_app->app_t->tick_handler);

	return 0;
}

void app_mesh_deinit(void)
{
	ASSERT_ERR(ali_app && ali_app->model);
	/*Release device*/
	ali_app->app_t->app_deinit(&ali_app->model);
	/*Free memory*/
	ke_free(ali_app);
}
void app_mesh_start(void)
{
	uint16_t status = M_ERR_NO_ERROR;
	m_printf(L_APP, "app mesh start:\r\n");

	//set api callback function
	bc_m_set(&app_mesh_cb);
	// Set clock information
	status = bc_m_set_run_time(0, 0);
	ASSERT_ERR(status == M_ERR_NO_ERROR);

	//register all models
	bc_mm_register_models(ali_app->model, ali_app->model_cnt);

	//if proved ;load flash config data
	uint32_t len  = bc_m_param_data_len();
	if(len && ali_app->app_t->prov_state_get()){
		ASSERT_ERR(len);
		uint8_t *data = ke_malloc(len, 0);
		ASSERT_ERR(data);
		bc_m_param_get_data(len, data);

		status =  bc_m_load_stored_info(len, data);
		if(M_ERR_NO_ERROR != status){
			bc_m_clear_config();
		}
		ke_free(data);
		M_PRINTF(L_APP, "load stored config data status[%x]", status);
	}

	// Enable the stack
	bc_m_enable();
	m_printf(L_APP, "mesh stack enabled\r\n");
}


void app_mesh_hal_rx(const HAL_TYPE type, const uint8_t* data, uint8_t len)
{
	if(ali_app && ali_app->app_t && ali_app->app_t->hal_rx)
		ali_app->app_t->hal_rx(type, data, len);
}

uint8_t app_mesh_prov_state_get(void)
{
	if(ali_app && ali_app->app_t && ali_app->app_t->prov_state_get)
		return ali_app->app_t->prov_state_get();
	return 0;
}

#endif /*BLE_MESH*/
