/** @file
 *
 * IPC Application for c51 devices
 *
 */
#include "bc62xx_ble.h"

//#define IPC_RX_BUFF_LEN		(30)
#define IPC_RX_BUFF_LEN		(200)
#define IPC_TX_BUFF_LEN		(30)


static tIPCControlBlock ipcCb;
static tIPCHandleCbArray ipcCbArray;

uint8_t ipcReadBuff[IPC_RX_BUFF_LEN];
uint8_t ipcSendBuff[IPC_TX_BUFF_LEN];
uint8_t gIPC_test;


void IpcDefaultCallBack(uint8_t len,uint8_t *dataPtr)
{
	gIPC_test++;
	return;
}

void IPC_Initialize(tIPCControlBlock *Cb)
{
	ipcCb = *Cb;
}

void IPC_init(tIPCHandleCbArray cbArrayPtr)
{
	ipcCbArray = cbArrayPtr;
	return;
}

static void ipcRx(uint8_t* Dest, uint16_t Len)
{
	uint32_t RxPtr = HR_REG_24BIT(reg_map(IPC_RX_READ_PTR));
	for (uint16_t i=0; i<Len; i++) {
		Dest[i] = HREAD(RxPtr);
		if(++RxPtr >= IPC_RX_END) 
			RxPtr = IPC_RX_HEAD;
	}
	
	HWRITE24BIT(IPC_RX_READ_PTR, RxPtr);
}

static void ipcTx(uint8_t* Src, uint8_t Len)
{
	int me;
	uint32_t TxPtr = HR_REG_24BIT(reg_map(IPC_TX_WRITE_PTR));
	for (uint16_t i=0; i<Len; i++) {
		HWRITE(TxPtr, Src[i]);
		if(++TxPtr >= IPC_TX_END) 
			TxPtr = IPC_TX_HEAD;
		me=100;
		while(me--);
	}
	HWRITE24BIT(IPC_TX_WRITE_PTR, TxPtr);
}

unsigned char IPC_TxBufferIsEnough(uint8_t Len)
{
	uint32_t TxPtr = HR_REG_24BIT(reg_map(IPC_TX_WRITE_PTR));
	uint32_t TxReadPtr = HR_REG_24BIT(reg_map(IPC_TX_READ_PTR));

	for (uint16_t i=0; i<Len; i++) {
		if(++TxPtr >= IPC_TX_END) 
			TxPtr = IPC_TX_HEAD;

		if(TxPtr == TxReadPtr)
			return 0;

	}
	return 1;
}

unsigned char IPC_WaitBufferEnough(uint8_t Len)
{
	int m;
	while(1){
		if(IPC_TxBufferIsEnough(Len))
		{
			return 1;
		}
		else
		{
			m = 1000;
			while(m--);
		}
	}
}

void IPC_TxCommon(uint8_t Type, uint8_t* Dt, uint8_t Len)
{
	ipcSendBuff[0] = Type;
	if (Len == 0 || Len > IPC_TX_BUFF_LEN-2)
		return;
	ipcSendBuff[1] = Len;
	memcpy(&ipcSendBuff[2], Dt, Len);
	ipcTx(ipcSendBuff, Len + 2);
}

void IPC_TxPacket(IPC_DATA_FORMAT *packet)
{
	if (packet->len == 0)
		return;

	ipcTx((unsigned char*)packet,  packet->len+ 2);
}

void IPC_TxControlCmd(uint8_t Cmd)
{
#define IPC_CMD_LEN 3
	uint8_t cmdBuff[IPC_CMD_LEN] = {IPC_CONTROL_CMD,0x01};
	cmdBuff[IPC_CMD_LEN - 1] = Cmd;
	ipcTx(cmdBuff, IPC_CMD_LEN);
}


uint8_t IPC_GetBBDIsconnectReason()
{
	return 0;
}

uint8_t IPC_CheckServerConenct()
{
	return 0;
}

uint8_t IPC_CheckReconnect()
{
	return 0;
}

void IPC_WaitLpm()
{
	int dayloop = 10000;
	while(dayloop--) hw_delay();
}

void IPC_AbandonLpm()
{
	hw_delay();
	//HW_REG_8BIT(IPC_M0_BUSY_FALG_ADDR, 0);
}


void IPC_DisableLpm()
{
	hw_delay();
	//HW_REG_8BIT(IPC_LPM_FLAG_ADDR, 0);
}

void IPC_EnableLpm()
{
	hw_delay();
	//HW_REG_8BIT(IPC_LPM_FLAG_ADDR, 1);
}


uint8_t IPC_IsTxBuffEmpty()
{
	uint32_t TxReadPtr = HR_REG_24BIT(reg_map(IPC_TX_READ_PTR));
	uint32_t TxWritePtr = HR_REG_24BIT(reg_map(IPC_TX_WRITE_PTR));
	if (TxWritePtr == TxReadPtr)
		return 1;
	return 0;
}

uint8_t IPC_IsRxBuffEmpty()
{
	uint32_t RxReadPtr = HR_REG_24BIT(reg_map(IPC_RX_READ_PTR));
	uint32_t RxWritePtr = HR_REG_24BIT(reg_map(IPC_RX_WRITE_PTR));
//	printf("RxReadPtr = %06x, RxWritePtr = %06x \n",RxReadPtr,RxWritePtr);
	if (RxReadPtr == RxWritePtr)
		return 1;
	return 0;
}

uint16_t a2dpLen;


void IPC_HandleRxPacket()
{
	uint8_t *pbuff = ipcReadBuff;
	if (IPC_IsRxBuffEmpty())
		return;

	ipcRx(ipcReadBuff, 2);
	if(*pbuff > IPC_TYPE_START && *pbuff < IPC_TYPE_NUM)
	{
		ipcRx(ipcReadBuff+2, *(pbuff+1));	
		//(*gIPCHandleCb[*pbuff])(*(pbuff+1),pbuff+2);
		(*(*ipcCbArray)[*pbuff])(*(pbuff+1),pbuff+2);
	}
	else
	{
		while(1);
	}

}

void IPC_DealSingleStep()
{
#define IPC_FRAME_HEAD_SIZE 3
	uint8_t *pbuff = ipcReadBuff;//大小为30个字节
	if (IPC_IsRxBuffEmpty())
		return;
			
	ipcRx(ipcReadBuff, IPC_FRAME_HEAD_SIZE);
 		
	if (*pbuff == IPC_CONTROL_EVT) {
		pbuff += 2;
		if(ipcCb.evtcb) ipcCb.evtcb(*pbuff);//处理收到的命令
	}
	else if(*pbuff ==   IPC_24G_DATA){
		pbuff++;
		ipcRx(ipcReadBuff+3, *(pbuff)-1);
		if(ipcCb.g24cb) ipcCb.g24cb((pbuff+1), *(pbuff));
		
	}
	else if (*pbuff == IPC_SPP_DATA) {
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE, *pbuff -1);
		if(ipcCb.sppcb) ipcCb.sppcb(pbuff+1, *pbuff);
	}	
	if (*pbuff == IPC_BLE_DATA) {
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE, *pbuff -1);				
		if(ipcCb.blecb) ipcCb.blecb(pbuff+1, *pbuff);//处理收到的数据
	}	
	else if (*pbuff == IPC_HID_DATA) {
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE, *pbuff -1);
		if(ipcCb.hidcb) ipcCb.hidcb(pbuff+1, *pbuff);
	}
	else if (*pbuff == IPC_MESH_DATA) {
 		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE, *pbuff -1);
		if(ipcCb.meshcb) ipcCb.meshcb(*(pbuff+1), pbuff+2, *pbuff-1);
	}
	else if (*pbuff == IPC_A2DP_DATA) {
		pbuff++;
		a2dpLen = pbuff[0]|pbuff[1]<<8;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE, a2dpLen);
		if(ipcCb.a2dpcb) ipcCb.a2dpcb(ipcReadBuff+IPC_FRAME_HEAD_SIZE,a2dpLen);
	}
	else if(*pbuff == IPC_MESH_ADV_DATA) {
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE,*pbuff -1);
		if(ipcCb.advcb) ipcCb.advcb(pbuff+1,*pbuff);
	}else if(*pbuff == IPC_READ_CONN_PARA) {
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE,*pbuff-1);
		if(ipcCb.readconnparamcb) ipcCb.readconnparamcb(pbuff+1,*pbuff);//
	}
	else if(*pbuff == IPC_BT_INQUIRY_RSP){
		pbuff++;
		ipcRx(ipcReadBuff+IPC_FRAME_HEAD_SIZE,*pbuff -1);
		if(ipcCb.inquirycb) ipcCb.inquirycb(pbuff+1,*pbuff);
	}
//	while(1);
}

void IPC_set_ack_flag()
{
  HWRITE(IPC_MCU_PHASE,IPC_MCU_PHASE1_ACK);
  HWRITE(IPC_MCU_STATE,IPC_MCU_STATE_RUNNING);
  while(HREAD(IPC_MCU_STATE))
    {
    HWRITE(0x4ff1,IPC_MCU_STATE_HIBERNATE);
    HWRITE(IPC_MCU_STATE,IPC_MCU_STATE_RUNNING);
    }
  while(HREAD(IPC_MCU_PHASE) != IPC_MCU_PHASE1_ACK)
    {
    HWRITE(IPC_MCU_PHASE,IPC_MCU_PHASE1_ACK);
    }
}

void IPC_set_nack_flag()
{
	HWRITE(IPC_MCU_PHASE,IPC_MCU_PHASE1_NACK);
	HWRITE(IPC_MCU_STATE,IPC_MCU_STATE_RUNNING);
}

void IPC_clear_flag()
{
	HWRITE(IPC_MCU_PHASE,IPC_MCU_PHASE_IDLE);
	HWRITE(IPC_MCU_STATE,IPC_MCU_STATE_RUNNING);
}

void IPC_wait_ack()
{
	int i;
	while(1)
	{
		i = 500;
		while(i--);
		switch (HREAD(IPC_MCU_PHASE))
		{
			case IPC_MCU_PHASE2_ACK:
				break;
			case IPC_MCU_PHASE_IDLE:
				return;
		}
	}
}
