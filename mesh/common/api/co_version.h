#ifndef _CO_VERSION_H_
#define _CO_VERSION_H_
/**
 ****************************************************************************************
 * @defgroup CO_VERSION Version Defines
 * @ingroup COMMON
 *
 * @brief Bluetooth Controller Version definitions.
 *
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_HW_50_ISO == 0)        // BLE 5.0 HW
/// BCBT SW Major Version
#define BCBT_SW_VERSION_MAJOR                   (BC_BT50_VERSION)
/// BCBT SW Minor Version
#define BCBT_SW_VERSION_MINOR                   0
/// BCBT SW Build Version
#define BCBT_SW_VERSION_BUILD                   17
/// BCBT SW Major Version
#define BCBT_SW_VERSION_SUB_BUILD               0

#else // (BLE_HW_50_ISO == 1)       // BLE 5.0 + ISO HW
/// BCBT SW Major Version
#define BCBT_SW_VERSION_MAJOR                   (BC_BT50_VERSION)
/// BCBT SW Minor Version
#define BCBT_SW_VERSION_MINOR                   0xA
/// BCBT SW Build Version
#define BCBT_SW_VERSION_BUILD                   6
/// BCBT SW Major Version
#define BCBT_SW_VERSION_SUB_BUILD               0
#endif // (BLE_HW_50_ISO == 0)



/// @} CO_VERSION


#endif // _CO_VERSION_H_
