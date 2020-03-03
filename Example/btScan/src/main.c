/*************************************************************************
*
* BC product
*
*************************************************************************/

#include <common.h>
#include "bc62xx_ble.h" 
#include "bc62xx_uart.h"
#include "systick.h"

void Bt_BleCallBack(uint8_t *buf, uint16_t len)
{

}

void Event_call_back(uint8_t da)
{
	switch(da){
		case 0x65:
			printf("BLE 0x65\r\n");
			//Do start inquiry
			HWRITE(mem_hci_cmd, 1);
			break;
		case IPC_EVT_LE_CONNECTED:{
			printf("BLE connect\r\n");
		}break;
			
		case IPC_EVT_LE_DISCONNECTED:{
			printf("BLE disconnect\r\n");
			HWRITE(mem_le_adv_enable, 0x01);
		}break;
		default:
			printf("enter event [0x%02x]\r\n", da);
			break;
	}
}
void Mesh_Adv_Cb(uint8_t* sour, uint16_t len)
{
	
}

void Inquiry_data_Cb(uint8_t* sour, uint16_t len)
{
	printf("RSSI %d MAC:", -sour[0]);
	for(int i = 1; i < len; i++){
		printf("%02x ", sour[i]);
	}
	printf("\r\n");
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

void main()
{	
	systick_init();
	UART_RemapToPrintf(UARTB, GPIO_UART_TX, GPIO_UART_RX);
	printf("-----------Bt scan Example-----------\r\n");
	
	tIPCControlBlock cb;
	memset(&cb, 0, sizeof(tIPCControlBlock));
	cb.blecb = Bt_BleCallBack;
	cb.evtcb = Event_call_back;
	cb.advcb = Mesh_Adv_Cb;
	//cb.readreqcb = ble_read_req_cb;
	cb.inquirycb = Inquiry_data_Cb;
	cb.readconnparamcb = ble_read_conn_param;
	
	IPC_Initialize(&cb);	//Register callback function.

	uint32_t tm = systick_get_ms();

	while(1){
		//ble process
		IPC_DealSingleStep();	
		if(systick_get_ms()-tm > 10){
			tm = systick_get_ms();
			HWRITE(mem_hci_cmd, 1);
		}
	}
}

