#ifndef __PORT_H__
#define __PORT_H__
#include "common.h"

#include "co_list.h"
#include "m_al.h"



#define RAM_START_ADDR 0x10010500
#define RAM_LENGTH 0x3500
#define S_ADDR(a) (RAM_START_ADDR+(((uint32_t)a)%RAM_LENGTH))

/* Used to know if a message is not present in kernel queue */
#define KE_MSG_NOT_IN_QUEUE     ((struct co_list_hdr *) 0xFFFFFFFF)

#define MESH_MSG_DEFAULT_HANDLER	0xFFFF
/* Default Message handler code to handle several message type in same handler. */
enum msg_id
{
	MESH_USE_ENC_BLOCK_IND = 0,
	MESH_EXT_ADV_REPORT_IND,
	MESH_READ_REQ_IND,
	MESH_WRITE_REQ_IND,
	MESH_MSG_ID_MAX = MESH_MSG_MAX,
};

/// Format of an event callback function
typedef void (*p_callback_t)(void);

/// Event types definition
/*@TRACE*/
enum KE_EVENT_TYPE
{
	KE_EVENT_BLE_MESH_DJOB ,
	KE_EVENT_KE_MESSAGE,        //!< KE_EVENT_KE_MESSAGE
	KE_EVENT_KE_TIMER,          //!< KE_EVENT_KE_TIMER
	KE_EVENT_MAX,               //!< KE_EVENT_MAX
};
/// Status of ke_task API functions
enum KE_EVENT_STATUS
{
    KE_EVENT_OK = 0,
    KE_EVENT_FAIL,
    KE_EVENT_UNKNOWN,
    KE_EVENT_CAPA_EXCEEDED,
    KE_EVENT_ALREADY_EXISTS,
};
/// Task State
typedef uint8_t ke_state_t;
/// Message Identifier. The number of messages is limited to 0xFFFF.
/// The message ID is divided in two parts:
/// bits[15~8]: task index (no more than 255 tasks support)
/// bits[7~0]: message index(no more than 255 messages per task)
typedef uint16_t ke_msg_id_t;

/// Task Identifier. Composed by the task type and the task index.
typedef uint16_t ke_task_id_t;


/// Format of a task message handler function
typedef int (*ke_msg_func_t)(ke_msg_id_t const msgid, void const *param,
                             ke_task_id_t const dest_id, ke_task_id_t const src_id);

/// Element of a message handler table.
struct ke_msg_handler
{
    /// Id of the handled message.
    ke_msg_id_t id;
    /// Pointer to the handler function for the msgid above.
    ke_msg_func_t func;
};

/// Task descriptor grouping all information required by the kernel for the scheduling.
struct ke_task_desc
{
    /// Pointer to the message handler table
    const struct ke_msg_handler* msg_handler_tab;
    /// Pointer to the state table (one element for each instance).
    ke_state_t* state;
    /// Maximum index of supported instances of the task.
    uint16_t idx_max;
    /// Number of messages handled
    uint16_t msg_cnt;
};
/// Profile Environment Data
typedef struct prf_env
{
    /// Application Task Number - if MSB bit set, Multi-Instantiated task
    ke_task_id_t app_task;
    /// Profile Task  Number    - if MSB bit set, Multi-Instantiated task
    ke_task_id_t prf_task;
} prf_env_t;

/// Profile task environment variable definition to dynamically allocate a Task.
typedef struct prf_task_env
{
    /// Profile Task description
    struct ke_task_desc desc;
    /// pointer to the allocated memory used by profile during runtime.
    prf_env_t*          env;
    /// Profile Task Number
    ke_task_id_t        task;
    /// Profile Task Identifier
    ke_task_id_t        id;
}prf_task_env_t;


/// Status returned by a task when handling a message
/*@TRACE*/
enum ke_msg_status_tag
{
    KE_MSG_CONSUMED = 0, ///< consumed, msg and ext are freed by the kernel
    KE_MSG_NO_FREE,      ///< consumed, nothing is freed by the kernel
    KE_MSG_SAVED,        ///< not consumed, will be pushed in the saved queue
};

/// Message structure.
struct ke_msg
{
    struct co_list_hdr hdr;     ///< List header for chaining

    ke_msg_id_t     id;         ///< Message id.
    ke_task_id_t    dest_id;    ///< Destination kernel identifier.
    ke_task_id_t    src_id;     ///< Source kernel identifier.
    uint16_t        param_len;  ///< Parameter embedded struct length.
    uint32_t        param[__ARRAY_EMPTY];   ///< Parameter embedded struct. Must be word-aligned.
};


/// Kernel memory heaps types.
/*@TRACE*/
enum KE_MEM_HEAP
{
    /// Memory allocated for environment variables
    KE_MEM_ENV,
//    #if (BLE_HOST_PRESENT)
    /// Memory allocated for Attribute database
    KE_MEM_ATT_DB,
//    #endif // (BLE_HOST_PRESENT)
    /// Memory allocated for kernel messages
    KE_MEM_KE_MSG,
    /// Non Retention memory block
    KE_MEM_NON_RETENTION,
    KE_MEM_BLOCK_MAX,
};

void *ke_malloc(uint32_t size, uint8_t type);

void ke_free(void* mem_ptr);

/*
//message type
*/
void ke_msg_free(struct ke_msg const *msg);

void ke_msg_send(void const *param_ptr);

void ke_event_init(void);

uint8_t ke_event_callback_set(uint8_t event_type, void (*p_callback)(void));

void ke_event_set(uint8_t event_type);

void ke_event_clear(uint8_t event_type);

//uint32_t ke_event_get_all(void);

void ke_event_schedule(void);

/**
 ****************************************************************************************
 * @brief Convert a parameter pointer to a message pointer
 *
 * @param[in]  param_ptr Pointer to the parameter member of a ke_msg
 *                       Usually retrieved by a ke_msg_alloc()
 *
 * @return The pointer to the ke_msg
 ****************************************************************************************
 */
__INLINE_S__ struct ke_msg * ke_param2msg(void const *param_ptr)
{
   return (struct ke_msg*) (((uint8_t*)param_ptr) - offsetof(struct ke_msg, param));
}

/**
 ****************************************************************************************
 * @brief Convert a message pointer to a parameter pointer
 *
 * @param[in]  msg Pointer to the ke_msg.
 *
 * @return The pointer to the param member
 ****************************************************************************************
 */
__INLINE_S__ void * ke_msg2param(struct ke_msg const *msg)
{
    return (void*) (((uint8_t*) msg) + offsetof(struct ke_msg, param));
}

#endif/*__PORT_H__*/
