#ifndef M_VERSION_
#define M_VERSION_

/**
 ****************************************************************************************
 * @defgroup MESH Mesh stack
 * @ingroup ROOT
 * @brief  Mesh Stack
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_version.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Mesh Specification version
#define MESH_VERSION_MAJOR                   (1)

/// Mesh Integration version (BLE core version)
#define MESH_VERSION_MINOR                   (BCBT_SW_VERSION_MAJOR)

/// Mesh SW version
#define MESH_VERSION_BUILD                   (0)

/// Mesh Sub SW version - if needed
#define MESH_VERSION_SUB_BUILD               (0)

/// @} MESH

#endif /* M_VERSION_ */
