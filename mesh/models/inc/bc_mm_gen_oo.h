#ifndef __BC_MM_GEN_OO_H__
#define __BC_MM_GEN_OO_H__

#include "mm_com.h"
#include "co_com.h"
#include "m_api.h"

/// Generic Server - OnOff
#define MM_ID_GENS_OO           (0x1000)

/// ********************* Message IDs for Generic OnOff Model **************************

/// Generic OnOff Get
#define MM_MSG_GEN_OO_GET               (0x0182)
/// Generic OnOff Set
#define MM_MSG_GEN_OO_SET               (0x0282)
/// Generic OnOff Set Unacknowledged
#define MM_MSG_GEN_OO_SET_UNACK         (0x0382)
/// Generic OnOff Status
#define MM_MSG_GEN_OO_STATUS            (0x0482)

typedef void (*bc_mm_oo_sts_set)(uint8_t on);
typedef uint8_t (*bc_mm_oo_sts_get)(void);

typedef struct
{
	bc_mm_oo_sts_set set;
	bc_mm_oo_sts_get get;
}onff_ctrl_t;

#pragma pack(1)
typedef struct 
{
    uint8_t present_onoff;
    uint8_t target_onoff;
    uint8_t remain;
}gen_oo_status_t;

typedef struct
{
    uint8_t onoff;
    uint8_t tid;
    uint8_t ttl;
    uint8_t delay;
}gen_oo_set_t;
#pragma pack()

/**
 *@brief init generic onoff model. 
 *param: mdl		the pointer of mesh_model_t
 *param: ctrl		the pointer of onff_ctrl_t
 *return: 0 success; 1 failed
 */
int bc_mm_gens_oo_init(mesh_model_t* mdl, onff_ctrl_t* ctrl);

/**
 *@brief deinit generic oooff model.   
 *param: mdl		the pointer of mesh_model_t
 *return: 0 success; 1 failed
 */
int bc_mm_gens_oo_deinit(mesh_model_t* mdl);

#endif/*__BC_MM_GEN_OO_H__*/