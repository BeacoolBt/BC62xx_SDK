#ifndef __MM_ALIS_H_
#define __MM_ALIS_H_
#include "stdio.h"
#include "co_com.h"
#include "mm_com.h"
#include "mm_ali.h"
#include "ali_timer.h"


/**
 *@brief ali vendor received data callback function. 
 *param: model_lid	model indentifier id
 *param: opcode		@ref enum m_alis_3b_opcodes
 *param: data		received data buff start address
 *param: len		received data length
 *return: 0 success; 1 failed
 */
typedef void  (*bc_mm_ali_vendor_rx_cb)(const m_lid_t model_lid, const uint8_t opcode, const uint8_t* data, const uint16_t len);

uint8_t bc_mm_alis_set_onoff_status(uint8_t* data, bool on);

uint8_t bc_mm_alis_set_ltn_status(uint8_t* data, uint16_t ltn);

uint8_t bc_mm_alis_set_tmp_status(uint8_t* data, uint16_t tmp);

/**
 *@brief ali vendor data indicate. 
 *param: model_lid	model indentifier id
 *param: data		indicate data pointer
 *param: len		indicate data lenth
 *return: 0 success; 1 failed
 */
int  bc_mm_alis_send_indication(m_lid_t model_lid, uint8_t* data, int len);

/**
 *@brief ali vendor data send. 
 *param: model_lid	model indentifier id
 *param: opcode		@ref enum m_alis_3b_opcodes
 *param: data		indicate data pointer
 *param: len		indicate data lenth
 *return: 0 success; 1 failed
 */
int  bc_mm_ali_vendor_send(m_lid_t model_lid, uint8_t opcode, uint8_t* data, uint16_t len);

/**
 *@brief ali vendor get random time. 
 *param: none
 *return: random time value
 */
uint16_t bc_mm_alis_get_random_time(void);

/**
 *@brief ali vendor model init. When device control by Genie/App, rx_cb will be called;
 		When mesh timer set by App is expired, ctl will be called.
 *param: mdl		mesh mode struct pointer
 *param[in]:rx_cb	vendor model recv data callback.
 *param[in]:ctl		mesh timer expired callback function.
 *return: 0 success; 1 failed
 */
int bc_mm_alis_vendor_init(mesh_model_t* mdl, bc_mm_ali_vendor_rx_cb rx_cb,	ali_timer_ctrl_t* ctl);

/**
 *@brief ali vendor model deinit. 
 *param: mdl     mesh mode struct pointer
 *return: 0 success; 1 failed
 */
int bc_mm_alis_vendor_deinit(mesh_model_t* mdl);

#endif/*__MM_ALIS_H_*/

