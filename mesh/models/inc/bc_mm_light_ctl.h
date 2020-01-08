#ifndef __BC_MM_LIGHT_CTL_H__
#define __BC_MM_LIGHT_CTL_H__
#include "mm_com.h"
#include "co_com.h"
#include "m_api.h"
#include "bc_mm_light_ln.h"

/// ************************* Message IDs for Light CTL Model *************************


/// Lighting Server - Light CTL
#define MM_ID_LIGHTS_CTL        (0x1303)
/// Lighting Server - Light CTL Temperature
#define MM_ID_LIGHTS_CTLT       (0x1306)
/// Lighting Server - Light CTL Setup
#define MM_ID_LIGHTS_CTLS       (0x1304)

/// Light CTL Get
#define MM_MSG_LIGHT_CTL_GET                (0x5D82)
/// Light CTL Set
#define MM_MSG_LIGHT_CTL_SET                (0x5E82)
/// Light CTL Set Unacknowledged
#define MM_MSG_LIGHT_CTL_SET_UNACK          (0x5F82)
/// Light CTL Status
#define MM_MSG_LIGHT_CTL_STATUS             (0x6082)
/// Light CTL Temperature Get
#define MM_MSG_LIGHT_CTL_TEMP_GET           (0x6182)
/// Light CTL Temperature Set
#define MM_MSG_LIGHT_CTL_TEMP_SET           (0x6482)
/// Light CTL Temperature Set Unacknowledged
#define MM_MSG_LIGHT_CTL_TEMP_SET_UNACK     (0x6582)
/// Light CTL Temperature Status
#define MM_MSG_LIGHT_CTL_TEMP_STATUS        (0x6682)
/// Light CTL Temperature Range Get
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_GET     (0x6282)
/// Light CTL Temperature Range Status
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_STATUS  (0x6382)
/// Light CTL Default Get
#define MM_MSG_LIGHT_CTL_DFLT_GET           (0x6782)
/// Light CTL Default Status
#define MM_MSG_LIGHT_CTL_DFLT_STATUS        (0x6882)

/// ********************* Message IDs for Light CTL Setup Model ***********************

/// Light CTL Default Set
#define MM_MSG_LIGHT_CTL_DFLT_SET             (0x6982)
/// Light CTL Default Set Unacknowledged
#define MM_MSG_LIGHT_CTL_DFLT_SET_UNACK       (0x6A82)
/// Light CTL Temperature Range Set
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_SET       (0x6B82)
/// Light CTL Temperature Range Set Unacknowledged
#define MM_MSG_LIGHT_CTL_TEMP_RANGE_SET_UNACK (0x6C82)

#pragma pack(1)
typedef struct 
{
    uint16_t current_lightness;
    uint16_t current_temperature;
    uint16_t target_lightness;
    uint16_t target_temperature;
    uint8_t remain;
} light_stl_status_t;

typedef struct
{
    uint16_t lightness;
    uint16_t temperature;
    uint16_t delta_UV;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
}light_ctl_set_t;
#pragma pack()

typedef void (*bc_mm_light_ctl_sts_set)(uint16_t ctl);
typedef uint16_t (*bc_mm_light_ctl_sts_get)(void);

typedef struct
{
	bc_mm_light_ctl_sts_set ctl_set;
	bc_mm_light_ctl_sts_get ctl_get;
	bc_mm_light_ln_sts_set ln_set;
	bc_mm_light_ln_sts_get ln_get;
}light_ctl_ctrl_t;


void bc_mm_light_ctl_init(mesh_model_t* mdl, light_ctl_ctrl_t* ctrl);
void bc_mm_light_ctl_deinit(mesh_model_t* mdl);

#endif