/**
  *@file ipc.h
  *@brief ipc support for application.
  */
#ifndef _BC62XX_BLE_H_
#define _BC62XX_BLE_H_

#include "common.h"

#define BUG_FIX
#ifdef BUG_FIX
#define FIX_ENTER_LPM	0x01
#define FIX_ENTER_HIBERNATE	0x03

#endif

/**
  *@brief IPC unite bluetooth state.
  */
enum btState {
	STATE_BT_NULL,
	STATE_BT_CONNECTING,
	STATE_BT_PAIRING,
	STATE_BT_CONNECTED,
	STATE_BT_PAGE,
	STATE_BT_DISCV,
};

/**
  *@brief IPC unite BLE state.
  */
enum blestate {
	STATE_BLE_NULL,
	STATE_BLE_CONNECTING,
	STATE_BLE_PAIRING,
	STATE_BLE_CONNECTED,
	STATE_BLE_RECONNECTING,
	STATE_BLE_DISCV,
};

/**
  *@brief IPC unite 2.4g state.
  */
enum privprostate {
	STATE_24G_NULL,
	STATE_24G,
};

/**
  *@brief IPC unit mesh state.
  */
enum meshstate {
	STATE_NULL,
};
/**
  *@brief adv type.
  */
enum advType{
	ADV_TYPE_NOMAL,
	ADV_TYPE_DIRECT,
	ADV_TYPE_NOCONNECT,
};

/**
  *@brief scan type.
  */
enum scanType{
	SCAN_TYPE_PASSIVE,
	SCAN_TYPE_ACTIVE,
};


/**
  *@brief MCU state.
  */
#define IPC_MCU_STATE_RUNNING 0
#define IPC_MCU_STATE_HIBERNATE 1 
#define IPC_MCU_STATE_LMP 2

/**
  *@brief MCU phase type.
  */
#define IPC_MCU_PHASE_IDLE	0
#define IPC_MCU_PHASE1_NACK	1
#define IPC_MCU_PHASE1_ACK	2
#define IPC_MCU_PHASE2_NACK	3
#define IPC_MCU_PHASE2_ACK	4

/**
  *@brief IPC buffer address.
  */
#define IPC_TX_BUF_START_ADDR IPC_TO_BT_BUF_START_ADDR
#define IPC_TX_BUF_END_ADDR	IPC_TO_BT_BUF_END_ADDR
#define IPC_TX_WPTR_ADDR IPC_TO_BT_WPTR_ADDR
#define IPC_TX_RPTR_ADDR  IPC_TO_BT_RPTR_ADDR
#define IPC_RX_WPTR_ADDR IPC_TO_M0_WPTR_ADDR
#define IPC_RX_RPTR_ADDR  IPC_TO_M0_RPTR_ADDR
#define IPC_RX_BUF_START_ADDR IPC_TO_M0_BUF_START_ADDR
#define IPC_RX_BUF_END_ADDR	IPC_TO_M0_BUF_END_ADDR
#define IPC_MCU_PHASE mem_ipc_mcu_phase
#define IPC_MCU_STATE mem_ipc_mcu_state

/**
  *@brief IPC buffer length.
  */
#define IPC_TX_BUF_LEN (IPC_TX_BUF_END_ADDR-IPC_TX_BUF_END_ADDR)
#define IPC_RX_BUF_LEN (IPC_RX_BUF_END_ADDR-IPC_RX_BUF_END_ADDR)

/**
  *@brief IPC tx hardware address.
  */
#define IPC_TX_HEAD	IPC_TX_BUF_START_ADDR
#define IPC_TX_END	IPC_TX_BUF_END_ADDR
#define IPC_TX_WRITE_PTR	IPC_TX_WPTR_ADDR
#define IPC_TX_READ_PTR		IPC_TX_RPTR_ADDR

/**
  *@brief IPC rx hardware address.
  */
#define IPC_RX_HEAD	IPC_RX_BUF_START_ADDR
#define IPC_RX_END	IPC_RX_BUF_END_ADDR
#define IPC_RX_WRITE_PTR	IPC_RX_WPTR_ADDR
#define IPC_RX_READ_PTR		IPC_RX_RPTR_ADDR

/**
  *@brief IPC command type.
  */
#define IPC_CMD_START_DISCOVERY 		1
#define IPC_CMD_STOP_DISCOVERY 			2
#define IPC_CMD_RECONNECT 				3
#define IPC_CMD_DISCONNECT 				4
#define IPC_CMD_ENTER_SNIFF				5
#define IPC_CMD_SET_PIN_CODE 			10
#define IPC_CMD_START_INQUIRY 			11
#define IPC_CMD_STOP_INQUIRY 			12
#define IPC_CMD_START_ADV				13
#define IPC_CMD_STOP_ADV				14
#define IPC_CMD_LE_DISCONNECT			17
#define IPC_CMD_UPDATA_CONN				18
#define IPC_CMD_START_SCAN				23
#define IPC_CMD_STOP_SCAN				24
#define IPC_CMD_ENTER_HIBERNATE 		25
#define IPC_CMD_ROLE_SWITCH 				29
#define IPC_CMD_BB_RECONN_CANCEL 		30
#define IPC_CMD_MTU_EXCHANGE				33
#define IPC_CMD_STOP_24G				34
#define IPC_CMD_PAIR_24G				35
#define IPC_CMD_TEST_24G				36
#define IPC_CMD_TEST_MODE				37
#define IPC_CMD_UPDATE_SUPERVISION_TO	40
#define IPC_CMD_LE_SET_PINCODE 			41
#define IPC_CMD_SET_RECONNECT_INIT		42
#define IPC_CMD_START_ADV_REC			43
#define IPC_CMD_REC_PAGE 				44
#define IPC_CMD_LOAD_FLASH_PARAMS     	45

/**
  *@brief IPC event type.
  */
#define IPC_EVT_NULL					0x00
#define IPC_EVT_BB_CONNECTED			0x01
#define IPC_EVT_BB_DISCONNECTED			0x02
#define IPC_EVT_RECONN_STARTED			0x03
#define IPC_EVT_RECONN_FAILED			0x04
#define IPC_EVT_SETUP_COMPLETE			0x05
#define IPC_EVT_HID_CONNECTED			0x06
#define IPC_EVT_HID_DISCONNECTED		0x07
#define IPC_EVT_SPP_CONNECTED			0x08
#define IPC_EVT_SPP_DISCONNECTED		0x09
#define IPC_EVT_PINCODE_REQ				0x0A
#define IPC_EVT_HID_HANDSHAKE			0x12
#define IPC_EVT_RECONN_PAGE_TIMEOUT		0X13
#define IPC_EVT_LE_CONNECTED			0x14
#define IPC_EVT_LE_DISCONNECTED			0x15
#define IPC_EVT_LINKKEY_GENERATE		0x18
#define IPC_EVT_LE_ENC_INFO				0x29
#define IPC_EVT_SWITCH_FAIL_MASTER		0x2a
#define IPC_EVT_RESET					0x2f
#define IPC_EVT_WAKEUP				0x30
#define IPC_EVT_24G_PAIRING_COMPLETE	0x31
#define IPC_EVT_24G_ATTEMPT_FAIL		0x32
#define IPC_EVT_HIBERNATE_WAKE			0x33
#define IPC_EVT_LE_PROTOCOL_CONNECTED	0x34
#define IPC_EVT_LE_PINCODE_REQ			0x35
#define IPC_EVT_24G_CHEAK_DONGLE_EXIST	0x36
#define IPC_EVT_RECONNECT_STANDY		0x37
#define IPC_EVT_24G_ATTEMPT_SUCCESS		0x38

#define IPC_EVT_INDICATE_COMIFIRMATION		0X67
#define IPC_EVT_ACCEPT_UPDATA_CONN_PARAM	0x68 
#define IPC_EVT_REJECT_UPDATA_CONN_PARAM	0x69
#define IPC_EVT_SEND_NOTIFY_OK				0x70

/**
  *@brief IPC event type define.
  */
#define IPC_EVT_HFP_CONNECTED		0x60
#define IPC_EVT_HFP_DISCONNECTED	0x61
#define IPC_EVT_HFP_INDICATORS_UPDATE 0x62
#define IPC_EVT_HFP_UP_TO_MAX_VLM 0x63
#define IPC_EVT_HFP_DOWN_TO_MIN_VLM 0x64

/**
  *@brief IPC event type enum.
  */
typedef enum
{
 	HFP_EVT_PLUS_CIEV=1,
 	HFP_EVT_PLUS_CLIP,
 	HFP_EVT_RING,
 	HFP_EVT_PLUS_VGS,
 	HPF_EVT_PLUS_VGM,
 	HFP_EVT_CIND_INDICATOR,
}HFP_EVT_TYPE;

/**
  *@brief IPC protocol data.
  */
#define	IPC_CONTROL_CMD		0x01
#define	IPC_CONTROL_EVT		0x02
#define	IPC_HID_DATA		0x03
#define	IPC_SPP_DATA		0x04
#define	IPC_BLE_DATA		0x05
#define	IPC_24G_DATA		0x06
#define	IPC_MESH_DATA		0x07
#define	IPC_MESH_ADV_DATA 	0x08
#define	IPC_MESH_RSP_DATA 	0x09
#define	IPC_A2DP_DATA		 0x0a
#define	IPC_HFP_DATA		0x0b
#define IPC_READ_CONN_PARA	0x0c
#define IPC_BT_INQUIRY_RSP	0x0d



/**
  *@brief BLE format.
  */
typedef struct
{
	unsigned short mhandle;
	unsigned char data;
}IPC_BLE_FORMAT;

/**
  *@brief SPP format.
  */
typedef struct
{
	unsigned char data;
}IPC_SPP_FORMAT;

/**
  *@brief IPC data format.
  */
typedef struct
{
	unsigned char ipctype;
	unsigned char len;
	union
	{
		IPC_BLE_FORMAT uBleData;
	} ipcUnion;
}IPC_DATA_FORMAT;

/**
  *@brief IPC type enum.
  */
#define IPC_TYPE_START 0
#define IPC_TYPE_NUM 0xc

typedef void (*tIPCHandleCb)(uint8_t,uint8_t *); //定义函数指针tIPCHandleCb
typedef tIPCHandleCb (*tIPCHandleCbArray)[IPC_TYPE_NUM]; //定义函数指针数组tIPCHandleCbArray

/**
  *@brief IPC EVT callback function type.
  */
typedef void (*tIPCEventCb)(uint8_t);

/**
  *@brief IPC SPP callback function type.
  */
typedef void (*tIPCSppCb)(uint8_t*, uint16_t);

/**
  *@brief IPC BLE callback function type.
  */
typedef void (*tIPCBleCb)(uint8_t*, uint16_t);

/**
  *@brief IPC timer(1s) callback function type.
  */
typedef void (*tIPCTimer)(void);

/**
  *@brief IPC Hid callback function type.
  */
typedef void (*tIPCHidCb)(uint8_t*, uint16_t);

/**
  *@brief IPC Hid callback function type.
  */
typedef void (*tIPCMeshCb)(uint8_t, uint8_t*, uint16_t);

/**
  *@brief IPC adv callback function type.
  */
typedef void (*tIPCAdvCb)(uint8_t*, uint16_t);

/**
  *@brief IPC a2dp callback function type.
  */
typedef void (*tIPCa2dpCb)(uint8_t*, uint16_t);

/**
  *@brief IPC a2dp callback function type.
  */
typedef void (*tIPCReadConnParam)(uint8_t*, uint16_t);

/**
  *@brief IPC a2dp callback function type.
  */
typedef void (*tIPCReadConnParam)(uint8_t*, uint16_t);

/**
  *@brief IPC 2.4G callback function type.
  */
typedef void (*tIPC24gCb)(uint8_t*, uint8_t);

/**
  *@brief IPC Hid callback function type.
  */
typedef void (*tIPCInquiryCb)(uint8_t*, uint16_t);




/**
  *@brief IPC control block type.
  */
typedef struct IPCContolBlock {
	tIPCEventCb evtcb;		/*< ipc evt call back*/
	tIPCSppCb sppcb;		/*< ipc spp call back*/
	tIPCTimer timercb;		/*< ipc 1s timer call back*/
	tIPCTimer stimercb;		/*< ipc 100ms timer call back */
	tIPCBleCb blecb;		/*< ipc ble call back*/
	tIPCAdvCb advcb;		/*< ipc adv call back*/
	tIPCHidCb hidcb;		/*< ipc hid call back*/
	tIPCMeshCb meshcb;		/*< ipc mesh call back*/
	tIPCa2dpCb a2dpcb;		/*< ipc audio call back */
	tIPCReadConnParam readconnparamcb;	/*< ipc read connparam call back */
	tIPC24gCb g24cb;
	tIPCInquiryCb inquirycb;
}tIPCControlBlock;

/**
  *@brief Define 6-bit address.
  */
#define BT_ADDR_SIZE	6
typedef uint8_t tBTADDR[BT_ADDR_SIZE];

/**
  *@brief This function rigist IPC Control Block.
  *@param cb the IPC control block. 
  *@return None.
  */
void IPC_Initialize(tIPCControlBlock *Cb);

/**
  *@brief This function deal IPC Single process, callback the rigisted IPC control function.
  *@param None.
  *@return None.
  */
void IPC_DealSingleStep(void);

/**
  *@brief This function tx control IPC command.
  *@param cmd IPC command want to send.
  *@return None.
  */
void IPC_TxControlCmd(uint8_t Cmd);

/**
  *@brief This function tx mult IPC data.
  *@param cmd IPC command want to send.
  *@return None.
  */
void IPC_TxCommon(uint8_t Type, uint8_t* Dt, uint8_t Len);

/**
  *@brief This function tx HID IPC command,Cpu will block when TxBuffer is not Empty. 
  *@param dt IPC HID data want to send,len length of hid data. 
  *@return None.
  */
#define IPC_TxHidData(dt, len)		IPC_TxCommon(IPC_HID_DATA, (dt), (len))

/**
  *@brief This function tx 24G IPC command,Cpu will block when TxBuffer is not Empty. 
  *@param dt IPC HID data want to send,len length of hid data. 
  *@return None.
  */
#define IPC_Tx24GData(dt, len)		IPC_TxCommon(IPC_24G_DATA, (dt), (len))

/**
  *@brief This function tx SPP IPC command,Cpu will block when TxBuffer is not Empty.
  *@param dt IPC SPP data want to send,len length of SPP data. 
  *@return None.
  */
#define IPC_TxSppData(dt, len)		IPC_TxCommon(IPC_SPP_DATA, (dt), (len))

/**
  *@brief This function tx BLE IPC command,Cpu will block when TxBuffer is not Empty. 
  *@param dt IPC BLE data want to send,len length of BLE data.
  *@return None.
  */
#define IPC_TxBleData(dt, len)		 IPC_TxCommon(IPC_BLE_DATA, (dt), (len))

/**
  *@brief This function will do nothing wait enter lpm. 
  *@param None.
  *@return None.
  */
void IPC_WaitLpm(void);

/**
  *@brief This function will do nothing abandon lpm this time.
  *@param None.
  *@return None.
  */
void IPC_AbandonLpm(void);

/**
  *@brief This function check if tx buffer empty. 
  *@param None.
  *@return None.
  */
uint8_t IPC_IsTxBuffEmpty(void);

/**
  *@brief This function check if rx buffer empty.
  *@param None. 
  *@return None.
  */
uint8_t IPC_IsRxBuffEmpty();

/**
  *@brief This function will check sdp server connect.
  *@param None.
  *@return None.
  */
uint8_t IPC_CheckServerConenct(void);

/**
  *@brief This function will disable lpm function.
  *@param None.
  *@return None.
  */
void IPC_DisableLpm();

/**
  *@brief This function will enable lpm function.
  *@param None.
  *@return None.
  */
void IPC_EnableLpm();

/**
  *@brief This function prevents IPC txbuffer overflow.
  *@param The length of the data send.
  *@return 1 represents the completion of sending data,return 0 represents txbuffer is empty.
  */
unsigned char IPC_TxBufferIsEnough(uint8_t Len);

/**
  *@brief This function waits to be written tx buffer.
  *@param The length of the data send.
  *@return 1 represent the completion of sending data.
  */
unsigned char IPC_WaitBufferEnough(uint8_t Len);

/**
  *@brief IPC default callback function.
  *@param len 
  *@param dataPtr
  *@return None.
  */
void IpcDefaultCallBack(uint8_t len,uint8_t *dataPtr);

/**
  *@brief Accept packages from the handle.
  *@param None.
  *@return None.
  */
void IPC_HandleRxPacket();

/**
  *@brief IPC initialization.
  *@param cbArrayPtr callback function.
  *@return None.
  */
void IPC_init(tIPCHandleCbArray cbArrayPtr);

/**
  *@brief IPC transport packet.
  *@param packet IPC data.@ref IPC_DATA_FORMAT
  *@return None.
  */
void IPC_TxPacket(IPC_DATA_FORMAT *packet);

/**
  *@brief IPC set ACK flag.
  *@param None.
  *@return None.
  */
void IPC_set_ack_flag();

/**
  *@brief IPC set NACK flag.
  *@param None.
  *@return None.
  */
void IPC_set_nack_flag();

/**
  *@brief IPC wait ACK.
  *@param None.
  *@return None.
  */
void IPC_wait_ack();

/**
  *@brief IPC clear flag.
  *@param None.
  *@return None.
  */
void IPC_clear_flag();
void Ble_RecviceBleIpc();

#endif/*_BC62XX_BLE_H_*/
