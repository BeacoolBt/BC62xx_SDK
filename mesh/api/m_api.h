#ifndef M_API_
#define M_API_

/**
 ****************************************************************************************
 * @defgroup M_API Mesh Native Model Program Interface
 * @ingroup MESH
 * @brief  Mesh Native Model Interface
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

//#include "arch.h"
#include "m_defines.h"
#include "m_config.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// Mesh Supported features
enum m_api_feat
{
    /// Relay Node
    M_FEAT_RELAY_NODE_SUP       = (1 << 0),
    /// Proxy Node
    M_FEAT_PROXY_NODE_SUP       = (1 << 1),
    /// Friend Node
    M_FEAT_FRIEND_NODE_SUP      = (1 << 2),
    /// Low Power Node
    M_FEAT_LOW_POWER_NODE_SUP   = (1 << 3),

    /// Message API supported
    M_FEAT_MSG_API_SUP          = (1 << 14),
    /// Provisioning over GATT
    M_FEAT_PB_GATT_SUP          = (1 << 15),
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Mesh start-up configuration
typedef struct m_cfg
{
    /// Mask of supported features (@see enum m_api_feat)
    uint16_t features;
    /// 16-bit company identifier assigned by the Bluetooth SIG
    uint16_t cid;
    /// 16-bit vendor-assigned product identifier
    uint16_t pid;
    /// 16-bit vendor-assigned product version identifier
    uint16_t vid;
    /// Localization descriptor
    uint16_t loc;
    /// Number of pages in the Composition Data
    uint8_t  nb_cdata_page;
} m_cfg_t ;

/**
 ****************************************************************************************
 * @brief Mesh Buffer, the value of the pointer must not be changed.
 *
 * It must be:
 * - Allocated through bc_m_buf_alloc()
 * - Updated with bc_m_buf_data_get() / bc_m_buf_data_len_get()
 * - Released with bc_m_buf_release()
 ****************************************************************************************
 */
typedef void m_api_buf_t;

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU for specific model identifier
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] opcode       Operation code
 * @param[in] p_buf        Pointer to the buffer containing the message PDU. - No need to release buffer.
 * @param[in] app_key_lid  Application Key Local identifier (Required for a response)
 * @param[in] src          Source address of the message (Required for a response)
 * @param[in] rssi         Measured RSSI level for the received PDU.
 * @param[in] not_relayed  True if message have been received by an immediate peer; False, it can have been relayed
 ****************************************************************************************
 */
typedef void (*m_api_model_rx_cb)(m_lid_t model_lid, uint32_t opcode, m_api_buf_t* p_buf, m_lid_t  app_key_lid, uint16_t src,
                                  int8_t rssi, bool not_relayed);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU to check that model can handle it
 *
 * @note bc_m_model_opcode_status function must be used to provide information about opcode support
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] opcode    Operation code to check
 ****************************************************************************************
 */
typedef void (*m_api_model_opcode_check_cb)(m_lid_t model_lid, uint32_t opcode);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a new publish period value.
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] period_ms    Publish period in milliseconds
 ****************************************************************************************
 */
typedef void (*m_api_model_publish_period_cb)(m_lid_t model_lid, uint32_t period_ms);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once PDU has been sent.
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] tx_hdl    Handle value configured by model when message has been requested to be sent
 * @param[in] p_buf     Pointer to the buffer containing the transmitted PDU. - Buffer must be released by model.
 * @param[in] status    Transmission status.
 ****************************************************************************************
 */
typedef void (*m_api_model_sent_cb)(m_lid_t model_lid, uint8_t tx_hdl, m_api_buf_t* p_buf, uint16_t status);

/**
 ****************************************************************************************
 * @brief Callback executed when simple execution performed
 *
 * @param[in] status    Execution status.
 ****************************************************************************************
 */
typedef void (*m_api_end_cb)(uint16_t status);

/**
 ****************************************************************************************
 * @brief Callback executed when a buffer block is properly released.
 *
 * @param[in] block_id  Buffer Block identifier
 ****************************************************************************************
 */
typedef void (*m_api_buf_block_released_cb)(uint8_t block_id);

/**
 ****************************************************************************************
 * Callback used to inform about a modification of the provisioning module state
 *
 * @param state    State of the provisioner   (@see enum m_prov_state)
 * @param status   Relevant only for provisioning failed (failed reason)
 ****************************************************************************************
 */
typedef void (*m_api_prov_state_cb)(uint8_t state, uint16_t status);

/**
 ****************************************************************************************
 * Callback used to inform that provisioning parameters are required
 ****************************************************************************************
 */
typedef void (*m_api_prov_param_req_cb)(void);

/**
 ****************************************************************************************
 * Callback used to inform that Out Of Band Authentication Data is required for provisioning
 *
 * @note Authentication data must be provided using @see bc_m_prov_oob_auth_rsp() function
 *
 *
 * @param auth_method  Authentication method (@see enum m_prov_auth_method)
 * @param auth_action  Authentication Action:
 *                     - M_PROV_AUTH_NO_OOB     = Prohibited
 *                     - M_PROV_AUTH_STATIC_OOB = 16 bytes LSB static out of band data required
 *                     - M_PROV_AUTH_OUTPUT_OOB = @see enum m_prov_out_oob, 1 bit set.
 *                     - M_PROV_AUTH_INPUT_OOB  = @see enum m_prov_in_oob, 1 bit set.
 * @param auth_size    expected authentication maximum data size
 ****************************************************************************************
 */
typedef void (*m_api_prov_oob_auth_req_cb)(uint8_t auth_method, uint16_t auth_action, uint8_t auth_size);

/**
 ****************************************************************************************
 * Callback used to inform that stored information have been loaded
 ****************************************************************************************
 */
typedef void (*m_api_stored_info_loaded_cb)(uint16_t status);
#if (BLE_MESH_MSG_API == 0)
/**
 ****************************************************************************************
 /// Callback used to inform that state date need to store
 ****************************************************************************************
 */
typedef void (*m_api_update_ind_cb)(uint8_t type, uint16_t len, uint8_t *data);

/**
 ****************************************************************************************
 /// Callback used to inform that node reset
 ****************************************************************************************
 */
typedef void (*m_api_reset_ind_cb)(void);
/**
 ****************************************************************************************
 /// Callback used to request the data of page
 ****************************************************************************************
 */
typedef void (*m_api_compo_data_req_cb)(uint8_t page);
#endif
/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Mesh call-backs structure for native API
typedef struct m_api_cb
{
    /// Callback executed at end of mesh enable request
    m_api_end_cb                   cb_enabled;
    /// Callback executed at end of mesh disable request
    m_api_end_cb                   cb_disabled;
    /// Callback used to inform application about provisioning state
    m_api_prov_state_cb            cb_prov_state;
    /// Callback used to request provisioning parameters to the application
    m_api_prov_param_req_cb        cb_prov_param_req;
    /// Callback used to request out of band authentication data
    m_api_prov_oob_auth_req_cb     cb_prov_auth_req;
    /// Callback used to inform that a buffer block is properly released
    m_api_buf_block_released_cb    cb_release;
    /// Callback used to inform that stored information have been loaded
    m_api_stored_info_loaded_cb    cb_loaded;
#if (BLE_MESH_MSG_API == 0)
	/// Callback used to inform that state date need to store
    m_api_update_ind_cb    			cb_update_ind;
	/// Callback used to inform that node reset
	m_api_reset_ind_cb				cb_reset_ind;
	m_api_compo_data_req_cb			cb_compo_data_req;

#endif
} m_api_cb_t;

/// Model call-backs structure for native API
typedef struct m_api_model_cb
{
    /// Reception of a buffer for model
    m_api_model_rx_cb             cb_rx;
    /// Callback executed when a PDU is properly sent
    m_api_model_sent_cb           cb_sent;
    /// Check if model can handle operation code
    m_api_model_opcode_check_cb   cb_opcode_check;
    /// Callback function called when a new publish period is received
    m_api_model_publish_period_cb cb_publish_period;
} m_api_model_cb_t;

/*
 * FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Define the set of callback to communicate with mesh native API
 *
 * @param[in] p_cb_api Native application callback set use to communicate with a native API
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_set(const m_api_cb_t* p_cb_api);

/**
 ****************************************************************************************
 * @brief Enable Mesh profile
 *
 * @note cb_enabled() of m_api_cb_t called at end of enable execution
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_enable(void);

/**
 ****************************************************************************************
 * @brief Disable Mesh profile
 *
 * @note cb_disabled() of m_api_cb_t called at end of disable execution
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_disable(void);

/**
 ****************************************************************************************
 * @brief Get device run time
 *
 * @param[out] p_clock_ms       Pointer to variable that will contain the current clock in milliseconds.
 * @param[out] p_nb_wrap        Pointer to variable that will contain the number of wrap.
 ****************************************************************************************
 */
void bc_m_get_run_time(uint32_t *p_clock_ms, uint16_t *p_nb_wrap);

/**
 ****************************************************************************************
 * @brief Set device run time
 *
 * @param[out] clock_ms       Current clock in milliseconds.
 * @param[out] nb_wrap        Number of wraps.
 ****************************************************************************************
 */
uint16_t bc_m_set_run_time(uint32_t clock_ms, uint16_t nb_wrap);

/**
 ****************************************************************************************
 * @brief Allocate block of buffers
 *
 * @param[out] p_block_id  Pointer to the variable that will contain the index of the allocated block.
 * @param[in]  nb_bufs     Number of buffers contained in the block.
 * @param[in]  small       Indicate if block contains small or long buffers.
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_buf_alloc_block(uint8_t *p_block_id, uint8_t nb_bufs, bool small);

/**
 ****************************************************************************************
 * @brief Free block of buffers
 *
 * @note cb_release() of m_api_cb_t called at end of disable execution
 *
 * @param[in] block_id   Index of the allocated block.
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_buf_free_block(uint8_t block_id);

/**
 ****************************************************************************************
 * @brief Allocate buffers command
 *
 * @param[out] pp_buf    Pointer to the buffer structure allocated
 * @param[in]  size      size of data required for the buffer
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_buf_alloc(m_api_buf_t** pp_buf, uint16_t size);

/**
 ****************************************************************************************
 * @brief Release allocate buffers. The buffer is free as soon as all reference to buffer are released.
 *
 * @param[in] p_buf    Pointer to the buffer structure
 *
 * @return Execution Status code
 ****************************************************************************************
 */
uint16_t bc_m_buf_release(m_api_buf_t* p_buf);

/**
 ****************************************************************************************
 * @brief Retrieve start data pointer of buffer structure
 *
 * @param[in] p_buf    Pointer to the buffer structure
 *
 * @return pointer to the data buffer, NULL if an error occurs.
 ****************************************************************************************
 */
uint8_t* bc_m_buf_data_get(m_api_buf_t* p_buf);

/**
 ****************************************************************************************
 * @brief Retrieve length of data in buffer structure
 *
 * @param[in] p_buf    Pointer to the buffer structure
 *
 * @return data buffer length, 0 if an error occurs.
 ****************************************************************************************
 */
uint16_t bc_m_buf_data_len_get(m_api_buf_t* p_buf);

/**
 ****************************************************************************************
 * @brief Register a model
 *
 * @param[in] model_id          Model ID.
 * @param[in] addr_offset       Offset from primary element address.
 * @param[in] vendor            Vendor Model ID or SIG Model ID.
 * @param[in] p_cb              Pointer to callback functions defined by the model
 * @param[out] p_model_lid      Pointer to the variable that will contain the allocated Model LID.
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_register_model(uint32_t model_id, uint8_t addr_offset, bool vendor,
                              const m_api_model_cb_t *p_cb, m_lid_t *p_model_lid);

/**
 ****************************************************************************************
 * @brief Load stored information for storage manager
 *
 * @param[in] length    Length of stored information report
 * @param[in] p_data    Pointer to stored information report
 ****************************************************************************************
 */
uint16_t bc_m_load_stored_info(uint16_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Set IV update mode and ignore 96-hour limit
 *
 * @param[in] update    True if transition to IV Update in Progress state is required, False if
 * require to transit to Normal state
 ****************************************************************************************
 */
void bc_m_iv_upd_test_mode(bool update);

/**
 ****************************************************************************************
 * @brief Let the model publish a message over mesh network
 *
 * @note Message status will be reported with model callback (@see m_api_model_sent_cb)
 *
 * @param[in] model_id     Model ID.
 * @param[in] opcode       Operation code of the message
 * @param[in] tx_hdl       Handle value used by model to retrieve which message has been sent
 * @param[in] p_buf        Pointer to the buffer structure that contains message to publish
 * @param[in] trans_mic_64 1 = Segmented PDU force transport MIC to 64 bits ; 0 = 32 bits transport MIC
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_model_publish(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, m_api_buf_t *p_buf, bool trans_mic_64);

/**
 ****************************************************************************************
 * @brief Let the model publish a message over mesh network
 *
 * @note Message status will be reported with model callback (@see m_api_model_sent_cb)
 *
 * @param[in] model_id     Model ID.
 * @param[in] opcode       Operation code of the message
 * @param[in] tx_hdl       Handle value used by model to retrieve which message has been sent
 * @param[in] p_buf        Pointer to the buffer structure that contains message to publish
 * @param[in] key_lid      Key information.
 *                         If key_lid & 0x80 != 0, key_lid & 0x7F = network key local index
 *                         else key_lid & 0x7F = application key local index.
 * @param[in] dst          Unicast destination address of the message (source address parameter of received request message)
 * @param[in] trans_mic_64 For a segmented PDU force transport mic to 64 bits
 * @param[in] not_relay    True, send message to an immediate peer; False, accept message to be relayed
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_model_rsp_send(m_lid_t model_lid, uint32_t opcode, uint8_t tx_hdl, m_api_buf_t *p_buf,
                              m_lid_t key_lid, uint16_t dst, bool trans_mic_64, bool not_relay);

/**
 ****************************************************************************************
 * @brief Reply to the Model operation code support (@see m_api_model_opcode_check_cb)
 *
 * @param[in] model_id  Model ID.
 * @param[in] opcode    Operation code checked
 * @param[in] status    M_ERR_NO_ERROR if operation supported by model, other error code to reject
 *
 * @return Execution status code
 ****************************************************************************************
 */
void bc_m_model_opcode_status(m_lid_t model_lid, uint32_t opcode, uint16_t status);

/**
 ****************************************************************************************
 * @brief Indicate an update of attention state to the application. It can be sent either
 * because attention timer has expired, or because attention state has been modified by
 * the provisioner.
 *
 * @param[in] attention_state       New attention state value
 ****************************************************************************************
 */
void bc_m_send_attention_update_ind(uint8_t attention_state);

/**
 ****************************************************************************************
 * @brief Request content of a page of composition data to the application.
 *
 * @param[in] page       Page number
 ****************************************************************************************
 */
void bc_m_send_compo_data_req_ind(uint8_t page);

/**
 ****************************************************************************************
 * @brief Request stored information to the application.
 ****************************************************************************************
 */
void bc_m_send_stored_info_req_ind(void);

/**
 ****************************************************************************************
 * @brief Inform the application about a configuration update.
 *
 * @param upd_type      Update type
 * @param length        Entry length
 * @param p_data        Pointer to entry data
 ****************************************************************************************
 */
void bc_m_send_update_ind(uint8_t upd_type, uint16_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Inform the application about received node reset request from the provisioner
 ****************************************************************************************
 */
void bc_m_send_node_reset_ind(void);

/**
 ****************************************************************************************
 * @brief Request to get the current Registered Fault state identified by Company ID for primary
 * element.
 *
 * @param[in] comp_id       Company ID
 ****************************************************************************************
 */
void bc_m_send_fault_get_req_ind(uint16_t comp_id);

/**
 ****************************************************************************************
 * @brief Request to start a test procedure of primary element.
 *
 * @param[in] comp_id       Company ID
 * @param[in] test_id       Test ID
 * @param[in] cfm_needed    Indicate if MESH_FAULT_CFM message is expected from the application.
 ****************************************************************************************
 */
void bc_m_send_fault_test_req_ind(uint16_t comp_id, uint8_t test_id, bool cfm_needed);

/**
 ****************************************************************************************
 * @brief Inform that clear of Registered Fault state identified by Company ID has been received for
 * primary element
 *
 * @param[in] comp_id       Company ID
 ****************************************************************************************
 */
void bc_m_send_fault_clear_ind(uint16_t comp_id);

/**
 ****************************************************************************************
 * @brief Send MESH_FAULT_PERIOD_IND message to the application
 *
 * @param[in] period_ms       Publication period in milliseconds
 * @param[in] period_ms       Publication period in milliseconds when one or several fault are known
 ****************************************************************************************
 */
void bc_m_send_fault_period_ind(uint32_t period_ms, uint32_t period_fault_ms);

#if (BLE_MESH_LPN)
/**
 ****************************************************************************************
 * @brief Request to send a MESH_API_LPN_STATUS_IND message to the upper application.
 *
 * @param[in] status        Friendship status
 * @param[in] friend_addr   Address of Friend node
 ****************************************************************************************
 */
void bc_m_send_lpn_status_ind(uint16_t status, uint16_t friend_addr);

/**
 ****************************************************************************************
 * @brief Request to send a MESH_API_LPN_OFFER_IND message to the upper application.
 *
 * @param[in] friend_addr       Address of node that sent the Friend Offer message
 * @param[in] rx_window         RX window supported by the Friend node
 * @param[in] queue_size        Queue size supported by the Friend node
 * @param[in] subs_list_size    Size of subscription list supported by the Friend node
 * @param[in] rssi              RSSI measured upon reception of Friend Offer message
 ****************************************************************************************
 */
void bc_m_send_lpn_offer_ind(uint16_t friend_addr, uint8_t rx_window,
                              uint8_t queue_size, uint8_t subs_list_size, int8_t rssi);
#endif //(BLE_MESH_LPN)

/**
 ****************************************************************************************
 * @brief Provide composition data
 *
 * @param[in] page      Page of composition data
 * @param[in] length    Page length
 * @param[in] p_data    Pointer to page content
 ****************************************************************************************
 */
void bc_m_compo_data_cfm(uint8_t page, uint8_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief Provide fault status for primary element
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to the fault array
 ****************************************************************************************
 */
void bc_m_health_status_send(uint16_t comp_id, uint8_t test_id, uint8_t length,
                              uint8_t *p_fault_array);

/**
 ****************************************************************************************
 * @brief Provide fault status for primary element
 *
 * @param[in] comp_id           Company ID
 * @param[in] test_id           Test ID
 * @param[in] length            Length of fault array
 * @param[in] p_fault_array     Pointer to the fault array
 ****************************************************************************************
 */
void bc_m_health_cfm(bool accept, uint16_t comp_id, uint8_t test_id, uint8_t length,
                      uint8_t *p_fault_array);

// -----------------------------------------------------------------------------
//                    Provisioning API
// -----------------------------------------------------------------------------

/**
 ****************************************************************************************
 * @brief Provide provisioning parameters to the provisioning module
 *
 * @param[in] p_param      Provisioning parameters
 ****************************************************************************************
 */
void bc_m_prov_param_rsp(void *p_param);

/**
 ****************************************************************************************
 * @brief Provide authentication data to the provisioning module
 *
 * @param[in] accept      True, Accept pairing request, False reject
 * @param[in] auth_size   Authentication data size (<= requested size else pairing automatically rejected)
 * @param[in] p_auth_data Authentication data (LSB for a number or array of bytes)
 ****************************************************************************************
 */
void bc_m_prov_oob_auth_rsp(bool accept, uint8_t auth_size, const uint8_t* p_auth_data);

/**
 ****************************************************************************************
 * @brief Get the local public key for out of band transmission of local public key
 *
 * @param[out] p_pub_key_x   X Coordinate of public Key (32 bytes LSB)
 * @param[out] p_pub_key_y   Y Coordinate of public Key (32 bytes LSB)
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_prov_pub_key_read(uint8_t* p_pub_key_x, uint8_t* p_pub_key_y);


#if (BLE_MESH_LPN)

// -----------------------------------------------------------------------------
//                    Low Power Node API
// -----------------------------------------------------------------------------

/**
 ****************************************************************************************
 * @brief Enable Low Power Node feature and start looking for an available Friend node in
 * the neighborhood.
 *
 * @param[in] poll_timeout          Initial value of PollTimeout timer
 * @param[in] poll_intv_ms          Poll interval in milliseconds
 * @param[in] rx_delay              Requested receive delay
 * @param[in] rssi_factor           RSSI factor
 * @param[in] rx_window_factor      Receive window factor
 * @param[in] min_queue_size_log    Requested minimum number of messages that the Friend node
 * can store in its Friend Queue.
 ****************************************************************************************
 */
uint16_t bc_m_lpn_start(uint32_t poll_timeout, uint32_t poll_intv_ms, uint8_t rx_delay,
                         uint8_t rssi_factor, uint8_t rx_window_factor, uint8_t min_queue_size_log);

/**
 ****************************************************************************************
 * @brief Disable Low Power Node feature.
 ****************************************************************************************
 */
uint16_t bc_m_lpn_stop(void);

/**
 ****************************************************************************************
 * @brief Select a friend after reception of one or several Friend Offer messages.
 *
 * @param[in] friend_addr       Address of the selected Friend node.
 ****************************************************************************************
 */
uint16_t bc_m_lpn_select_friend(uint16_t friend_addr);
#endif //(BLE_MESH_LPN)

#if (BLE_MESH_GATT_PROXY)

// -----------------------------------------------------------------------------
//                    GATT Proxy API
// -----------------------------------------------------------------------------

/**
 ****************************************************************************************
 * @brief Control if Proxy service should start / stop advertising it's capabilities
 *
 * @param[in] enable  True to enable advertising for 60s, False to stop advertising
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_proxy_ctrl(bool enable);
#endif // (BLE_MESH_GATT_PROXY)

/**
 ****************************************************************************************
 * @brief mesh stack init
 *
 * @param none
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_mesh_init(void);
/**
 ****************************************************************************************
 * @brief mesh stack deinit
 *
 * @param none
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_mesh_deinit(void);

/**
 ****************************************************************************************
 * @brief mesh stack process mainloop
 *
 * @param none
 *
 * @return Execution status code
 ****************************************************************************************
 */
uint16_t bc_m_mesh_loop();

/**
 ****************************************************************************************
 * @brief mesh stack clear configuration data
 *
 * @param none
 *
 * @return none
 ****************************************************************************************
 */
void bc_m_clear_config(void);

/******************************for stored config parameter********************/
/**
 ****************************************************************************************
 * @brief save config parameter
 *
 * @param[in] upd_type        update data type of config
 * @param[in] length          data length
 * @param[in] p_data          data
 * 
 * @return none
 ****************************************************************************************
 */
void bc_m_param_save(uint8_t upd_type, uint16_t length, uint8_t *p_data);

/**
 ****************************************************************************************
 * @brief save config parameter
 *
 * @param[in] none
 * 
 * @return the total length of config parameter data
 ****************************************************************************************
 */
uint32_t bc_m_param_data_len(void);

/**
 ****************************************************************************************
 * @brief save config parameter
 *
 * @param[in] buff length
 * @param[in] buff pointer address
 * 
 * @return the total length of config parameter data
 ****************************************************************************************
 */
uint32_t bc_m_param_get_data(uint32_t len, uint8_t* data);

//for ali
/**
 ****************************************************************************************
 * @brief ali save seq iv parameter; 
 ************************************
 */
void bc_m_ali_save_seq_iv(void);

/**
 ****************************************************************************************
 * @brief stop unproved beacon data
 ****************************************************************************************
 */
void bc_m_bcn_stop_tx_unprov_bcn(void);

/**
 ****************************************************************************************
 * @brief Find an application key from application key index
 *
 * @param[in]  app_key_id      Application key index
 * @param[out] p_app_key_lid   Pointer used to return application key local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t bc_m_key_app_find(uint16_t app_key_id, m_lid_t* p_app_key_lid);

/**
 ****************************************************************************************
 * @brief Indicate that an application key has been bound with a given model.
 *
 * @param[in] model_lid        Model local index.
 ****************************************************************************************
 */
void bc_m_mio_bind(m_lid_t model_lid);
/**
 ****************************************************************************************
 * @brief Create a bind between a specific model and an application key
 *
 * @param[in] app_key_lid Application key local identifier
 * @param[in] model_lid   Model local identifier
 *
 * @return Execution status (@see enum m_error)
 ****************************************************************************************
 */
uint16_t bc_m_key_model_bind(m_lid_t app_key_lid, m_lid_t model_lid);

/**
 ****************************************************************************************
 * @brief Add an address in the subscription list of a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] addr             Address to insert.
 * @param[in] p_label_uuid     Pointer to Label UUID used to generate the address in case it is a
 * virtual address.
 *
 * @return M_ERR_NO_ERROR if address has been inserted.
 *         M_ERR_COMMAND_DISALLOWED if address cannot be inserted.
 ****************************************************************************************
 */
uint16_t bc_m_mio_add_subscription(m_lid_t model_lid, uint16_t addr);

/**
 ****************************************************************************************
 * @brief Set publication parameters for a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] p_params         Pointer to a buffer containing the publication parameters.
 *
 * @return M_ERR_NO_ERROR if publication parameters have been set.
 *         M_ERR_INVALID_PARAM if publication parameters are not valid.
 ****************************************************************************************
 */
uint16_t bc_m_mio_set_publi_param(m_lid_t model_lid, uint16_t addr, uint8_t *p_label_uuid,
                                  uint8_t app_key_lid, uint8_t ttl, uint8_t period, uint8_t retx_params,
                                  uint8_t friend_cred);





/// @} M_API

#endif /* M_API_ */
