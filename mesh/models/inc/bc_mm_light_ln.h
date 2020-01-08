#ifndef __BC_MM_LIGHT_LN_H__
#define __BC_MM_LIGHT_LN_H__
#include "mm_com.h"
#include "co_com.h"
#include "m_api.h"

/// Lighting Server - Light Lightness
#define MM_ID_LIGHTS_LN         (0x1300)
/// Lighting Server - Light Lightness Setup
#define MM_ID_LIGHTS_LNS        (0x1301)

/// ********************** Message IDs for Light Lightness Model **********************

/// Light Lightness Get
#define MM_MSG_LIGHT_LN_GET                 (0x4B82)
/// Light Lightness Set
#define MM_MSG_LIGHT_LN_SET                 (0x4C82)
/// Light Lightness Set Unacknowledged
#define MM_MSG_LIGHT_LN_SET_UNACK           (0x4D82)
/// Light Lightness Status
#define MM_MSG_LIGHT_LN_STATUS              (0x4E82)
/// Light Lightness Linear Get
#define MM_MSG_LIGHT_LN_LINEAR_GET          (0x4F82)
/// Light Lightness Linear Set
#define MM_MSG_LIGHT_LN_LINEAR_SET          (0x5082)
/// Light Lightness Linear Set Unacknowledged
#define MM_MSG_LIGHT_LN_LINEAR_SET_UNACK    (0x5182)
/// Light Lightness Linear Status
#define MM_MSG_LIGHT_LN_LINEAR_STATUS       (0x5282)
/// Light Lightness Last Get
#define MM_MSG_LIGHT_LN_LAST_GET            (0x5382)
/// Light Lightness Last Status
#define MM_MSG_LIGHT_LN_LAST_STATUS         (0x5482)
/// Light Lightness Default Get
#define MM_MSG_LIGHT_LN_DFLT_GET            (0x5582)
/// Light Lightness Default Status
#define MM_MSG_LIGHT_LN_DFLT_STATUS         (0x5682)
/// Light Lightness Range Get
#define MM_MSG_LIGHT_LN_RANGE_GET           (0x5782)
/// Light Lightness Range Status
#define MM_MSG_LIGHT_LN_RANGE_STATUS        (0x5882)

/// ***************** Message IDs for Light Lightness Setup Model *********************

/// Light Lightness Default Set
#define MM_MSG_LIGHT_LN_DFLT_SET            (0x5982)
/// Light Lightness Default Set Unacknowledged
#define MM_MSG_LIGHT_LN_DFLT_SET_UNACK      (0x5A82)
/// Light Lightness Range Set
#define MM_MSG_LIGHT_LN_RANGE_SET           (0x5B82)
/// Light Lightness Range Set Unacknowledged
#define MM_MSG_LIGHT_LN_RANGE_SET_UNACK     (0x5C82)

typedef void (*bc_mm_light_ln_sts_set)(uint16_t ln);
typedef uint16_t (*bc_mm_light_ln_sts_get)(void);

typedef struct
{
	bc_mm_light_ln_sts_set set;
	bc_mm_light_ln_sts_get get;
}light_ln_ctrl_t;

#pragma pack(1)
typedef struct 
{
    uint16_t current_level;
    uint16_t target_level;
    uint8_t remain;
}light_ln_status_t;

typedef struct
{
    uint16_t level;
    uint8_t tid;
    uint8_t ttl;
    uint8_t delay;
}light_ln_set_t;
#pragma pack()

void bc_mm_light_ln_init(mesh_model_t* mdl,light_ln_ctrl_t* ctrl);

void bc_mm_light_ln_deinit(mesh_model_t* mdl);

#endif/*__BC_MM_LIGHT_LN_H__*/