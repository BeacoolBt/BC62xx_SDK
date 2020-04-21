#ifndef M_CONFIG_
#define M_CONFIG_

/**
 ****************************************************************************************
 * @defgroup M_CONFIG Mesh configuration defines
 * @ingroup MESH
 * @brief  Mesh configuration defines
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */


/*
 * COMPILATION FLAGS
 ****************************************************************************************
 */

/// Flag indicating that Mesh Debug is enabled
#if (defined(CFG_BLE_MESH_DBG) && defined(CFG_BLE_MESH))
#define BLE_MESH_DBG 1
#else  // !(defined(CFG_BLE_MESH_DBG) && defined(CFG_BLE_MESH))
#define BLE_MESH_DBG 0
#endif // (defined(CFG_BLE_MESH_DBG) && defined(CFG_BLE_MESH))

/// Flag indicating that Mesh Message API is enabled
//#if (defined(CFG_BLE_MESH_MSG_API) && defined(CFG_BLE_MESH))
#define BLE_MESH_MSG_API 0
//#else  // !(defined(CFG_BLE_MESH_MSG_API) && defined(CFG_BLE_MESH))
//#define BLE_MESH_MSG_API 0
//#endif // (defined(CFG_BLE_MESH_MSG_API) && defined(CFG_BLE_MESH))

#if (defined(CFG_BLE_MESH_STORAGE_NONE))
#define BLE_MESH_STORAGE_NONE                (1)
#else
#define BLE_MESH_STORAGE_NONE                (0)
#endif //(BLE_MESH_STORAGE_NONE)

#if (defined(CFG_BLE_MESH_STORAGE_WVT))
#define BLE_MESH_STORAGE_WVT                 (1)
#else
#define BLE_MESH_STORAGE_WVT                 (0)
#endif //(BLE_MESH_STORAGE_WVT)

/// Support of GATT Proxy feature
#define CFG_BLE_MESH_GATT_PROXY
#if defined(CFG_BLE_MESH_GATT_PROXY)
#define BLE_MESH_GATT_PROXY                  (1)
#else
#define BLE_MESH_GATT_PROXY                  (0)
#endif //defined(CFG_BLE_MESH_GATT_PROXY)

/// Support of P feature
#define CFG_BLE_MESH_GATT_PROV
#if defined(CFG_BLE_MESH_GATT_PROV)
#define BLE_MESH_GATT_PROV                   (1)
#else
#define BLE_MESH_GATT_PROV                   (0)
#endif //defined(CFG_BLE_MESH_GATT_PROV)

/// Support of Relay feature
//#if defined(CFG_BLE_MESH_RELAY)
#define BLE_MESH_RELAY                       (1)
//#else
//#define BLE_MESH_RELAY                       (0)
//#endif //defined(CFG_BLE_MESH_RELAY)

/// Support of Friend feature
#if defined(CFG_BLE_MESH_FRIEND)
#define BLE_MESH_FRIEND                      (1)
#else
#define BLE_MESH_FRIEND                      (0)
#endif //defined(CFG_BLE_MESH_FRIEND)

/// Support of Low Power Node feature
#if defined(CFG_BLE_MESH_LPN)
#define BLE_MESH_LPN                         (1)
#else
#define BLE_MESH_LPN                         (0)
#endif //defined(CFG_BLE_MESH_LPN)

/// Used to know if GATT Bearer is present
#define BLE_MESH_GATT_BEARER (BLE_MESH_GATT_PROXY || BLE_MESH_GATT_PROV)

/// Supported feature mask
#define BLE_MESH_FEAT_MASK   (  (BLE_MESH_MSG_API    * M_FEAT_MSG_API_SUP        )\
                              | (BLE_MESH_RELAY      * M_FEAT_RELAY_NODE_SUP     )\
                              | (BLE_MESH_GATT_PROXY * M_FEAT_PROXY_NODE_SUP     )\
                              | (BLE_MESH_GATT_PROV  * M_FEAT_PB_GATT_SUP        )\
                              | (BLE_MESH_FRIEND     * M_FEAT_FRIEND_NODE_SUP    )\
                              | (BLE_MESH_LPN        * M_FEAT_LOW_POWER_NODE_SUP ))

/*
 * DEFINES
 ****************************************************************************************
 */

/// Default TTL to use for transmission  TODO Need to be managed by Foundation layer
#define M_TTL_DEFAULT                        (11)
/// Default relay to use for relay feature
#define M_RLY_DEFAULT						 (0x66)

/// Number of advertising transmission for Beacon
#define M_ADV_BCN_NB_TX                          (2)
/// Number of advertising transmission to perform
#define M_ADV_NB_TX                          (7)//[0,7]
/// Advertising interval - 20ms - 32 slots
#define M_ADV_INTERVAL                       (16)//(32)
/// Advertising connectable interval - 100ms - 32 slots TODO [FBE] check if specified by mesh
#define M_ADV_CON_INTERVAL                   (16)//32
/// Scanning interval - 30ms - 48 slots
#define M_ADV_SCAN_INTERVAL                  (16)//(48)

/// Maximum number of subnets the node can belong to
#define M_SUBNET_NB_MAX                      (2)////(5)

/// Maximum number of connectable bearer
#define BLE_CONNECTION_MAX					 (1)
#define M_BEARER_CON_MAX                     (BLE_CONNECTION_MAX)

/// Maximum number of bearers that can be added
#define M_BEARER_MAX                         (M_BEARER_CON_MAX + 1)
/// Maximum number of network interfaces (+ 1 for Local Interface)
#define M_LAY_NET_INTF_MAX                   (M_BEARER_MAX + 1)
/// Number of entries in the Network Message Cache
#define M_LAY_NET_MSG_CACHE_SIZE             (5)//zjl(10)
/// Number of entries in the Network Pre-filtering list
#define M_LAY_NET_PRE_FILT_SIZE              (4)

/// Number of buffer that can be used for transmission of generated lower transmit packet
#define M_LAY_LTRANS_NB_TX_BUFFER            (2)
/// Number of segmentation that can be performed in parallel
#define M_LAY_LTRANS_NB_SEGMENTATION         (2)
/// Number of reassembly that can be performed in parallel
#define M_LAY_LTRANS_NB_REASSEMBLY           (3)
/// Number of segment info in filtering list
#define M_LAY_LTRANS_NB_SEGMENT_FILTER       (6)
/// Number of time to send a segmented message which not target a unicast address
#define M_LAY_LTRANS_NB_SEG_PACKET_RETRANS   (3)

/// Transmission Pool size
#define M_LAY_ACCESS_TX_POOL_SIZE            (3)

/// Maximum number of buffer block that can be allocated by the Mesh Buffer Manager Toolbox
#define M_TB_BUF_BLOCK_MAX                   (5)
/// Size of environment part in buffer provided by the Mesh Buffer Manager Toolbox
/// Value must be a multiple of 4
#define M_TB_BUF_ENV_SIZE                    (32)
/// Size of data part of "long" buffers
/// Value must be a multiple of 4
#define M_TB_BUF_LONG_SIZE                   (152)
/// Size of data part of "small" buffers
/// Value must be a multiple of 4
#define M_TB_BUF_SMALL_SIZE                  (32)
#if (BLE_MESH_FRIEND)
/// Number of "small" buffer to allocate during mesh stack initialization
#define M_TB_BUF_INIT_LONG_NB                (4)
/// Number of "long" buffer to allocate during mesh stack initialization
#define M_TB_BUF_INIT_SMALL_NB               (20)
#else
/// Number of "small" buffer to allocate during mesh stack initialization
#define M_TB_BUF_INIT_LONG_NB                (2)
/// Number of "long" buffer to allocate during mesh stack initialization
#define M_TB_BUF_INIT_SMALL_NB               (10)
#endif //(BLE_MESH_FRIEND)
/// Maximum number of buffers that can be dynamically allocated
#define M_TB_BUF_DYN_MAX                     (2)

/// Number of model instances used by the device
/// Shall be at least 2 (Configuration Server model and Health Server model)
#define M_TB_MIO_MODEL_NB                    (10)
/// Number of addresses that can be put in the subscription list for a model instance
#define M_TB_MIO_SUBS_LIST_SIZE              (10)
/// Number of virtual addresses that can be stored
#define M_TB_MIO_VIRT_ADDR_LIST_SIZE         (5)

/// Maximum number of device keys supported by Mesh stack
#define M_TB_KEY_MAX_NB_DEV                  (1)
/// Maximum number of network keys supported by Mesh stack
#define M_TB_KEY_MAX_NB_NET                  (M_SUBNET_NB_MAX)
/// Maximum number of Application keys supported by Mesh stack
#define M_TB_KEY_MAX_NB_APP                  (10)//zjl(15)
/// Maximum number of Bound between Application and Model supported by Mesh stack
#define M_TB_KEY_MAX_NB_APP_MODEL_BIND       (20)//zjl(30)

/// Maximum number of connection Low Power Nodes when Friend Feature is used
#define M_FRIEND_LPN_MAX                     (5)
/// Maximum number of messages stored for a Low Power Node - Should be a power of 2
#define M_FRIEND_NB_STORED_MSG_MAX           (16)
/// Maximum number of address in Subscription List for a Low Power Node (max 255)
#define M_FRIEND_SUBS_LIST_MAX_LEN           (10)
/// Receive window in milliseconds
#define M_FRIEND_RECEIVE_WINDOW_MS           (3 * M_ADV_SCAN_INTERVAL * SLOT_SIZE / 1000)

/// Duration between two update of Connectable advertising data (1000ms)
#define M_PROXY_CON_ADV_UPDATE_DUR          (1000)//zjl(100)

/// Size of the Proxy filter list per active connections
#define M_PROXY_FILT_LIST_SIZE               (30)

#define MESH_MSG_MAX  6

#define BLE_MESH      1

#define APP_FOR_ALI

/// @} M_CONFIG

#endif /* M_CONFIG_ */
