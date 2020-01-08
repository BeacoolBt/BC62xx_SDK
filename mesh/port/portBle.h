#ifndef __PORTBLE_H__
#define __PORTBLE_H__

#include "m_al.h"
#include "gap.h"
#include "att.h"
#include "port.h"


/// Advertising report information
#define ADV_IND			0
#define ADV_DIRECT_IND  1
#define ADV_NONCONN_IND 2
#define SCAN_REQ		3
#define SCAN_RSP		4
#define CONNECT_REQ		5
#define ADV_SCAN_IND	6

#define GATT_PROV_START_HANDLE	(0x001B)
#define GATT_PROV_WRITE_HANDLE	(0x001D)

#define GATT_PROXY_START_HANDLE	(0x0021)
#define GATT_PROXY_WRITE_HANDLE	(0x0023)

enum gapm_adv_report_info
{
    /// Report Type
    GAPM_REPORT_INFO_REPORT_TYPE_MASK    = 0x07,
    /// Report is complete
    GAPM_REPORT_INFO_COMPLETE_BIT        = (1 << 3),
    /// Connectable advertising
    GAPM_REPORT_INFO_CONN_ADV_BIT        = (1 << 4),
    /// Scannable advertising
    GAPM_REPORT_INFO_SCAN_ADV_BIT        = (1 << 5),
    /// Directed advertising
    GAPM_REPORT_INFO_DIR_ADV_BIT         = (1 << 6),
};

/// Advertising report type
enum gapm_adv_report_type
{
    /// Extended advertising report
    GAPM_REPORT_TYPE_ADV_EXT = 0,
    /// Legacy advertising report
    GAPM_REPORT_TYPE_ADV_LEG,
    /// Extended scan response report
    GAPM_REPORT_TYPE_SCAN_RSP_EXT,
    /// Legacy scan response report
    GAPM_REPORT_TYPE_SCAN_RSP_LEG,
    /// Periodic advertising report
    GAPM_REPORT_TYPE_PER_ADV,
};

/// Indicate reception of advertising, scan response or periodic advertising data
/*@TRACE*/
struct gapm_ext_adv_report_ind
{
    /// Activity identifier
    //uint8_t actv_idx;
    /// Bit field providing information about the received report (@see enum gapm_adv_report_info)
    uint8_t info;
    /// Transmitter device address
    //struct gap_bdaddr trans_addr;
    /// Target address (in case of a directed advertising report)
    //struct gap_bdaddr target_addr;
    /// TX power (in dBm)
    //int8_t tx_pwr;
    /// RSSI (between -127 and +20 dBm)
    int8_t rssi;
    /// Primary PHY on which advertising report has been received
    //uint8_t phy_prim;
    /// Secondary PHY on which advertising report has been received
    //uint8_t phy_second;
    /// Advertising SID
    /// Valid only for periodic advertising report
    //uint8_t adv_sid;
    /// Periodic advertising interval (in unit of 1.25ms, min is 7.5ms)
    /// Valid only for periodic advertising report
    //uint16_t period_adv_intv;
    /// Report length
    uint16_t length;
    /// Report
    uint8_t data[__ARRAY_EMPTY];
};


/// Inform that attribute value is requested by lower layers.
/*@TRACE*/
struct gattc_read_req_ind
{
    /// Handle of the attribute that has to be read
    uint16_t handle;
};
/// Confirm Read Request requested by GATT to profile
/*@TRACE*/
struct gattc_read_cfm
{
    /// Handle of the attribute read
    uint16_t handle;
    /// Data length read
    uint16_t length;
    /// Status of read command execution by upper layers
    uint8_t status;
    /// attribute data value
    uint8_t  value[__ARRAY_EMPTY];
};

/// Inform that a modification of database has been requested by peer device.
struct gattc_write_req_ind
{
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// offset at which the data has to be written
    uint16_t offset;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
};
/// Confirm modification of database from upper layer when requested by peer device.
struct gattc_write_cfm
{
    /// Handle of the attribute written
    uint16_t handle;
    /// Status of write command execution by upper layers
    uint8_t status;
};

/// Send an event to peer device
struct gattc_send_evt_cmd
{
    /// Request type (notification / indication)
    uint8_t operation;
    /// operation sequence number
    uint16_t seq_num;
    /// characteristic handle
    uint16_t handle;
    /// length of packet to send
    uint16_t length;
    /// data value
    uint8_t  value[__ARRAY_EMPTY];
};

/// Possible values for setting client configuration characteristics
enum prf_cli_conf
{
    /// Stop notification/indication
    PRF_CLI_STOP_NTFIND = 0x0000,
    /// Start notification
    PRF_CLI_START_NTF,
    /// Start indication
    PRF_CLI_START_IND,
};

/// Internal 16bits UUID service description
struct attm_desc
{
    /// 16 bits UUID LSB First
    uint16_t uuid;
    /// Attribute Permissions (@see enum attm_perm_mask)
    uint16_t perm;
    /// Attribute Extended Permissions (@see enum attm_value_perm_mask)
    uint16_t ext_perm;
    /// Attribute Max Size
    /// note: for characteristic declaration contains handle offset
    /// note: for included service, contains target service handle
    uint16_t max_size;
};

typedef enum _MESH_BLE_EVENT{
	/*adversting started*/
	MESH_BLE_ADV_STARTED = 0,
	/*adversting stopted*/
	MESH_BLE_ADV_STOPTED,
	/*adversting scanning started*/
	MESH_BLE_SCAN_STARTED,
	/*adversting scanning data indication; parameter @ref struct mesh_adv_report_param*/
	MESH_BLE_SCAN_RX_CB,
	/*adversting scanning stopted*/
	MESH_BLE_SCAN_STOPTED,
	/*ble gattc connected; parameter @ref struct mesh_connected_param*/
	MESH_BLE_CON_CONNECTED,
	/*ble gattc disconnected; parameter @ref struct mesh_disconnected_param*/
	MESH_BLE_CON_DISCONNECTED,
	/*ble gattc read request; parameter @ref struct mesh_read_req_param*/
	MESH_BLE_CON_READ_REQ,
	/*ble gattc write request; parameter @ref struct mesh_write_req_param*/
	MESH_BLE_CON_WRITE_REQ
}MESH_BLE_EVENT;

/// Indicate reception of advertising, scan response or periodic advertising data
/*@TRACE*/
struct mesh_adv_report_param
{
    /// Bit field providing information about the received report (@see enum gapm_adv_report_info)
    uint8_t info;
    /// RSSI (between -127 and +20 dBm)
    int8_t rssi;
    /// Report length
    uint16_t length;
    /// Report
    uint8_t* data;
};

/// Indicate gattc connected parameter
struct mesh_connected_param
{
    /// connected handle
    uint16_t con;
};

/// Indicate gattc disconnected parameter
struct mesh_disconnected_param
{
    /// connected handle to be disconnect
    uint16_t con;
	/// disconnected reason
	uint8_t reason;
};

/// Indicate gattc read request parameter
struct mesh_read_req_param
{
	/// connected handle
    uint16_t con;
    /// Handle of the attribute that has to be written
    uint16_t handle;
};

/// Inform that a modification of database has been requested by peer device.
struct mesh_write_req_param
{
	/// connected handle
    uint16_t con;
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// offset at which the data has to be written
    uint16_t offset;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t*  value;
};

/**
 ****************************************************************************************
 * @brief define ble event callback
 *
 * @param ev		BLE event, see as: MESH_BLE_EVENT
 * @param param		event related parameter
 * return
 * [0]process success, [1]process failed or not valid data
 ****************************************************************************************
 */
typedef uint32_t (*mesh_ble_event_callback)(MESH_BLE_EVENT ev, void* param);

/**
 ****************************************************************************************
 * @brief ble stack start and set event callback function
 *
 * @param[in] cb	callback function
 * return
 * [0]ble stack start success, [1]staet failed
 ****************************************************************************************
 */
uint32_t mesh_ble_start(mesh_ble_event_callback cb);
/**
 ****************************************************************************************
 * @brief process ble event
 ****************************************************************************************
 */
void mesh_ble_process(void);

/**
 ****************************************************************************************
 * @brief stop ble stack and unset event callback function
 ****************************************************************************************
 */
void mesh_ble_stop();

/**
 ****************************************************************************************
 * @brief set event and fill the parameter
 ****************************************************************************************
 */
uint32_t mesh_ble_event_set(MESH_BLE_EVENT ev, void* param);

/*
 ****************************************************************************************
 * Adv functions
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief start non-connectable and non-scannable undirected advertising events
 *
 * @param[in] itl	Packet transmission interval (in slots)
 * @param[in] nb	Number of packet transmission to perform
 * @param[in] len	Length of data to transmit
 * @param[in] data	Pointer to the data to transmit
 ****************************************************************************************
 */
void mesh_adv_data_send(uint16_t itl, uint8_t nb, uint16_t len, const uint8_t* data);

/**
 ****************************************************************************************
 * @brief start connectable and scannable undirected advertising events
 *
 * @param[in] itl	Packet transmission interval (in slots)
 * @param[in] nb	Number of packet transmission to perform
 * @param[in] len	Length of data to transmit
 * @param[in] data	Pointer to the data to transmit
 ****************************************************************************************
 */
void mesh_con_adv_data_send(uint16_t itl, uint16_t len, const uint8_t* data);

/**
 ****************************************************************************************
 * @brief stop connectable adversting
 ****************************************************************************************
 */
void mesh_con_adv_stop(void);

/**
 ****************************************************************************************
 * @brief disconnect ble connection
 *
 * @param[in] handle	need to closed connected handle 
 ****************************************************************************************
 */
void mesh_con_stop(uint16_t handle);

/*
 ****************************************************************************************
 * Scan functions
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief scan start function
 *
 * @param[in] itl	Packet transmission interval (in slots)
 ****************************************************************************************
 */
void mesh_adv_scan_start(uint16_t itl);

/**
 ****************************************************************************************
 * @brief scan stop function
 ****************************************************************************************
 */
void mesh_adv_scan_stop(void);

void mesh_connect_do();

void mesh_disconnect_do();

/*
 ****************************************************************************************
 * Con functions
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief notify data function
 *
 * @param[in] handle: characteristic handle
 * @param[in] data: value
 * @param[in] len: length of packet to send
 * return
 * length of notified data 
 ****************************************************************************************
 */
uint32_t mesh_con_notify(uint16_t handle, uint8_t* data, uint32_t len);

/**
 ****************************************************************************************
 * @brief read confirm function
 *
 * @param[in] handle: characteristic handle
 * @param[in] data: value
 * @param[in] len: length of packet to send
 * return 
 * length of read contirm data
 ****************************************************************************************
 */
uint32_t mesh_con_read_cfm(uint16_t handle, uint8_t* data, uint32_t len);

/**
 ****************************************************************************************
 * @brief write response function
 *
 * @param[in] handle: characteristic handle
 * return 
 * [0] write success; [1] write failed
 ****************************************************************************************
 */
uint8_t mesh_con_write_rsp(uint16_t handle);

/**
 ****************************************************************************************
 * @brief Get mtu size
 *
 * @param[in] connection handle
 * return 
 * mtu size
 ****************************************************************************************
 */
uint32_t gattc_get_mtu(uint8_t conidx);

/*
 ****************************************************************************************
 * Service functions
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Function use to verify if several services can be allocated on a contiguous
 * handle range. If this command succeed, it means that service allocation will succeed.
 *
 * If start_hdl = 0, it return handle using first available handle (start_hdl is
 * modified); else it verifies if start handle given can be used to allocates handle range.
 *
 * @param[in|out] start_hdl     Service start handle.
 * @param[in]     nb_att        Number of handle to allocate (containing service handles)
 *
 * @return Command status code:
 *  - @ref ATT_ERR_NO_ERROR: If service allocation succeeds.
 *  - @ref ATT_ERR_INVALID_HANDLE: If start_hdl given in parameter or UUIDs value invalid
 ****************************************************************************************
 */
uint8_t attm_reserve_handle_range(uint16_t* start_hdl, uint8_t nb_att);

#if 1//(BLE_ATTS)
/**
 ****************************************************************************************
 * @brief Function use to ease service database creation.
 * Use @see attmdb_add_service function of attmdb module to create service database,
 * then use @see attmdb_add_attribute function of attmdb module to create attributes
 * according to database description array given in parameter.
 *
 * @note: database description array shall be const to reduce memory consumption (only ROM)
 * @note: It supports only 16 bits UUIDs
 *
 * @note: If shdl = 0, it return handle using first available handle (shdl is
 * modified); else it verifies if start handle given can be used to allocates handle range.
 *
 * @param[in|out] shdl          Service start handle.
 * @param[in]     uuid          Service UUID
 * @param[in|out] cfg_flag      Configuration Flag, each bit matches with an attribute of
 *                              att_db (Max: 32 attributes); if the bit is set to 1, the
 *                              attribute will be added in the service.
 * @param[in]     max_nb_att    Number of attributes in the service
 * @param[in|out] att_tbl       Array which will be fulfilled with the difference between
 *                              each characteristic handle and the service start handle.
 *                              This array is useful if several characteristics are optional
 *                              within the service, can be set to NULL if not needed.
 * @param[in]     dest_id       Task ID linked to the service. This task will be notified
 *                              each time the service content is modified by a peer device.
 * @param[in|out] att_db        Table containing all attributes information
 * @param[in]     svc_perm      Service permission (@see enum attm_svc_perm_mask)
 *
 * @return Command status code:
 *  - @ref ATT_ERR_NO_ERROR: If database creation succeeds.
 *  - @ref ATT_ERR_INVALID_HANDLE: If start_hdl given in parameter + nb of attribute override
 *                            some existing services handles.
 *  - @ref ATT_ERR_INSUFF_RESOURCE: There is not enough memory to allocate service buffer.
 *                           or of new attribute cannot be added because all expected
 *                           attributes already added or buffer overflow detected during
 *                           allocation
 ****************************************************************************************
 */
uint8_t attm_svc_create_db(uint16_t *shdl, uint16_t uuid, uint8_t *cfg_flag, uint8_t max_nb_att,
                           uint8_t *att_tbl, ke_task_id_t const dest_id,
                           const struct attm_desc *att_db, uint8_t svc_perm);

#endif

/**
 ****************************************************************************************
 * @brief Control visibility of a service from peer device. service present in database
 *        but cannot be access by a peer device.
 *
 * @param[in] handle Service handle.
 * @param[in] hide   True to hide the service, False to restore visibility
 *
 * @return Command status code:
 *  - @ref ATT_ERR_NO_ERROR: If service allocation succeeds.
 *  - @ref ATT_ERR_INVALID_HANDLE: If start_hdl given in parameter or UUIDs value invalid
 ****************************************************************************************
 */
uint8_t attmdb_svc_visibility_set(uint16_t handle, bool hide);

/**
 ****************************************************************************************
 * @brief mac addres set
 *
 * @param[in] reversal	the mac bytes order.
 * @param[in] mac		six bytes mac data pointer
 *
 * @return status:
 *  - 0: If set success.
 *  - 1: If ser failed.
 ****************************************************************************************
 */
uint8_t mesh_mac_set(bool reversal, uint8_t* mac);

/**
 ****************************************************************************************
 * @brief mac addres get
 *
 * @param[in] reversal	the mac bytes order.
 * @param[in] mac		six bytes mac data pointer
 *
 * @return status:
 *  - 0: If get success.
 *  - 1: If ger failed.
 ****************************************************************************************
 */
uint8_t mesh_mac_get(bool reversal, uint8_t* mac);

/**
 ****************************************************************************************
 * @brief flash read
 *
 * @param[in] addr		start addr.
 * @param[in] len		length of read data.
 * @param[out] data		read data buffer pointer.
 *
 * @return read data length:
 ****************************************************************************************
 */
uint32_t mesh_flash_read(uint32_t addr, uint32_t len, uint8_t* data);

/**
 ****************************************************************************************
 * @brief flash erase and write
 *
 * @param[in] addr		start addr.
 * @param[in] len		length of write data.
 * @param[out] data		witer data buffer pointer.
 *
 * @return write data length:
 ****************************************************************************************
 */
uint32_t mesh_flash_write(uint32_t addr, uint32_t len, uint8_t* data);
/**
 ****************************************************************************************
 * @brief flash write
 *
 * @param[in] addr		start addr.
 * @param[in] len		length of write data.
 * @param[out] data		witer data buffer pointer.
 *
 * @return write data length:
 ****************************************************************************************
 */

uint32_t mesh_flash_write_nerase(uint32_t addr, uint32_t len, uint8_t* data);

/**
 ****************************************************************************************
 * @brief flash erase size 4K
 *
 * @param[in] addr		start addr.
 *
 * @return write data length:
 ****************************************************************************************
 */
uint32_t mesh_flash_erase(uint32_t addr);

#endif/*__PORTBLE_H__*/
