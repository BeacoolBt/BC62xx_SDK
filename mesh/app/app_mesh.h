#ifndef _APP_MESH_H_
#define _APP_MESH_H_

#include <stdint.h>
#include "ali_config.h"
#include "mm_com.h"
#include "bc62xx_timer.h"

#define APP_MESH_MODELS_COUNT           4

typedef enum{
	HAL_TST = 0,//this index is used for product test
	HAL_UART,
	HAL_24G,
	HAL_BLE,
	HAL_MAX,
}HAL_TYPE;

//*******below is lib_code*********
typedef void (*ali_app_factory_reset)(void);
typedef void (*ali_app_prov_state_cb)(uint8_t state);
typedef uint8_t (*ali_app_prov_state_get)(void);
typedef uint8_t (*ali_app_init)(mesh_model_t** mdl, uint8_t* mcnt);
typedef uint8_t (*ali_app_deinit)(mesh_model_t** mdl);
typedef void (*ali_app_hal_rx)(const HAL_TYPE type, const uint8_t *data , uint8_t len);
/*define systick function for application */
typedef void (*ali_app_tick_handler)(void);

typedef struct
{
	/*Do factory reset*/
	ali_app_factory_reset factory_reset;
	/*When providion status changed, use the function to indicate application*/
	ali_app_prov_state_cb prov_state_cb;
	/*Get application saved provinsion status*/
	ali_app_prov_state_get prov_state_get;
	/*Application init*/
	ali_app_init app_init;
	/*Application deinit*/
	ali_app_deinit app_deinit;
	/*When received data from hardware, transmit date to application*/
	ali_app_hal_rx hal_rx;
	/*when systick is arrived, call this function*/
	ali_app_tick_handler tick_handler;
}app_t;


typedef struct{
	uint8_t model_cnt;//model counter
	uint8_t reg_cnt;
	mesh_model_t* model;//model information
	SYS_TIMER_TYPE provTimeoutTimer;//indicate timer
	const app_t* app_t;
}ali_app_t;

uint8_t app_mesh_init(void);

void app_mesh_deinit(void);

void app_mesh_start(void);

void app_mesh_hal_rx(const HAL_TYPE type, const uint8_t* data, uint8_t len);

uint8_t app_mesh_prov_state_get(void);

#endif
