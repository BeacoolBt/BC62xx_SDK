#ifndef __MM_CON_H__
#define __MM_CON_H__

#include <stdint.h>
#include "m_api_msg.h"

#define MAX_ELEMENTS_NUM (4) //for config

typedef uint32_t (*bc_m_model_opcode_check)(uint32_t opcode);
typedef void (*bc_m_model_msg_deal)(m_api_model_msg_ind_t const* p_msg);

typedef struct 
{
    uint32_t id;
    uint8_t offset;
    uint8_t lid;
	uint8_t vendor;
	bc_m_model_opcode_check op_check_cb;
	bc_m_model_msg_deal		msg_deal_cb;
}mesh_model_t;

extern uint8_t emmTid;

/**
 ****************************************************************************************
 * @brief Check tid is valid
 *
 * @param[in] tid		model recvived tid value.
 *
 * @return true: valid.
 *         false: invalid.
 ****************************************************************************************
 */
bool bc_mm_check_tid(uint8_t tid);

uint8_t bc_mm_com_get_tx_hdl(void);

/**
 ****************************************************************************************
 * @brief register all models.
 *
 * @param[in] model_t		mesh_model_t array pointer.
 * @param[in] cnt			mesh_model_t array counter 
 *
 * @return 0: all model registered.
 *         1: register failed.
 ****************************************************************************************
 */
uint8_t bc_mm_register_models(mesh_model_t* model_t, uint8_t cnt);

/**
 ****************************************************************************************
 * @brief cancel regestered models.
 *
 * @param None
 * @return 0 Success.
 *         1 Failed.
 ****************************************************************************************
 */
uint8_t bc_mm_unregister_models(void);

#endif

