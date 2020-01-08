/*************************************************************************
*
* BC product
*
*************************************************************************/

#include <common.h>
#include "bc62xx_ble.h" 
#include "bc62xx_uart.h"

#pragma pack(1)
typedef struct
{
	uint8_t sucFlag[3];
	uint8_t sucLength;
	uint8_t sucType;
	uint16_t susCpyId;
	uint16_t susBcnType;
	uint8_t sucUuid[16];
	uint16_t susMajor;
	uint16_t susMinor;
	uint8_t sucMPower;
}ibeacon_t;
#pragma pack(0)

void Bt_BleCallBack(uint8_t *buf, uint16_t len)
{

}

void Event_call_back(uint8_t da)
{
	switch(da){
		case 0x65:
			printf("BLE 0x65\r\n");
			break;
		case IPC_EVT_LE_CONNECTED:{
			printf("BLE connect\r\n");
		}break;
			
		case IPC_EVT_LE_DISCONNECTED:{
			printf("BLE disconnect\r\n");
			HWRITE(mem_le_adv_enable, 0x01);
		}break;
		default:
			printf("enter event [%d]\r\n", da);
			break;
	}
}
void Mesh_Adv_Cb(uint8_t* sour, uint16_t len)
{

}

void ble_read_req_cb(uint8_t* buf, uint16_t len)
{
	printf("%s:", __FUNCTION__);
	printf("hadler[%x]\r\n", (uint16_t)*buf);
}

void ble_read_conn_param(uint8_t* buf, uint16_t len)
{
	printf("%s:", __FUNCTION__);
	for(int i = 0; i < len; ++i){
		printf("%02x ", buf[i]);
	}
	printf("\r\n");
}
const uint8_t ucUuid[] = {0xB9 ,0x40, 0x7F, 0x30, 0xF5, 0xF8, 0x46, 0x6E, 0xAF, 0xF9, 0x25, 0x55, 0x6B, 0x57, 0xFE, 0x6D};
const uint8_t ucFlags[] = {0x02, 0x01, 0x06};
void main()
{	
		
	UART_RemapToPrintf(UARTB, GPIO_UART_TX, GPIO_UART_RX);
	printf("-----------IBEACON Example-----------\r\n");

	//tBTADDR _ucAddr = {0xFF, 0x01, 0x02, 0x03, 0x04, 0x05};
	//BLE_SetMacAddr(_ucAddr);
#if 1

	HWRITE(mem_tx_power,0x02);
	
	tIPCControlBlock cb;
	memset(&cb, 0, sizeof(tIPCControlBlock));
	cb.blecb = Bt_BleCallBack;
	cb.evtcb = Event_call_back;
	cb.advcb = Mesh_Adv_Cb;
	//cb.readreqcb = ble_read_req_cb;
	cb.readconnparamcb = ble_read_conn_param;
	
	IPC_Initialize(&cb);	//Register callback function.

	/*ADV Parameter init*/
	HWRITE(mem_le_adv_type, ADV_TYPE_NOMAL);
	HW_REG_16BIT(reg_map(mem_le_adv_interval_max), 1600);	//itl : 1s; unit: 0.625ms
	
	/*Set IBEACON Information*/
	static ibeacon_t stIBeacon;
	memcpy(stIBeacon.sucFlag, ucFlags, sizeof(ucFlags));
	stIBeacon.sucLength = 0x1A;
	stIBeacon.sucType = 0xff;
	stIBeacon.susCpyId = 0x004c;
	stIBeacon.susBcnType = 0x1502;
	memcpy(stIBeacon.sucUuid, ucUuid, sizeof(ucUuid));
	stIBeacon.susMajor = 0x0101;
	stIBeacon.susMinor = 0x0202;
	stIBeacon.sucMPower = 0xc5;
	
	memcpy((unsigned char*)reg_map(mem_le_adv_data),&stIBeacon,sizeof(stIBeacon));
	HWRITE(mem_le_adv_data_len,sizeof(stIBeacon));

	for(int i = 0; i < sizeof(stIBeacon); ++i){
		printf("%02x ", ((uint8_t*)&stIBeacon)[i]);
	}
	printf("\r\n");

	/*Start ADV*/
	HWRITE(mem_le_adv_enable, 0x01);
#endif	


	printf("-----------Start IBEACON-----------\r\n");
	//WDT_Enable();

	while(1){
		//WDT_Kick();

		//ble process
		IPC_DealSingleStep();	
	}
}

