#include <common.h>
#include "bc62xx_uart.h"
#include "bc62xx_ble.h" 
//#include "bc62xx_wdt.h"
#include "gatt_ota.h"
#include "portBle.h"

#include "co_com.h"
#include "ali_config.h"
#include "m_api.h"
#include "app_mesh.h"

#include "PriMesh.h"
#include "Timer.h"
#include "Application.h"
#include "Function.h"

void Bt_BleCallBack(uint8_t *buf, uint16_t len)
{
	uint16_t att_hdl = (uint16_t)*buf;
	switch(att_hdl){
		case 0x0009:{
			uint8_t val = (uint16_t)*(buf+2);
			if(val){
				printf("NOTIFY:\r\n");
			}
		}break;
		case GATT_PROV_WRITE_HANDLE:
		case GATT_PROXY_WRITE_HANDLE:{
			#if (BLE_MESH)
			if(OTANONE != gatt_ota_state_get())
				break;
			struct mesh_write_req_param para;
			para.con = 0;
			para.handle = att_hdl;
			para.offset = 0;
			para.length = len-2;
			para.value = buf+2;
			mesh_ble_event_set(MESH_BLE_CON_WRITE_REQ,&para);
			#endif
		}break;
		case OTA_WRITE_HANDLE:{
			gatt_ota_data_deal(buf+2, len - 2);
		}break;
		default:{
			
		}break;
	}
}

void Event_call_back(uint8_t da)
{
	unsigned char BleConnEvent[3] = {0x02,0x02,0x00};
	unsigned char BleDisconnEvent[3] = {0x02,0x05,0x00};
	switch(da){
		case 0x65:
			printf("BLE 0x65\r\n");
			break;
		case IPC_EVT_LE_CONNECTED:{
			printf("BLE connect\r\n");
			gatt_ota_start();
			#if (BLE_MESH)
			struct mesh_connected_param para;
			para.con = 0;
			mesh_ble_event_set(MESH_BLE_CON_CONNECTED,&para);
			#endif
		}break;
			
		case IPC_EVT_LE_DISCONNECTED:{
			printf("BLE disconnect\r\n");
			gatt_ota_stop();
			HWRITE(mem_le_adv_enable, 0x01);
			#if (BLE_MESH)	
			struct mesh_disconnected_param para;
			para.con = 0;
			para.reason = 0;
			mesh_ble_event_set(MESH_BLE_CON_DISCONNECTED,&para);
			#endif
		}break;
		default:
			printf("enter event [%d]\r\n", da);
			break;
	}
}
void Mesh_Adv_Cb(uint8_t* sour, uint16_t len)
{
#if (BLE_MESH)	
	if(OTANONE != gatt_ota_state_get())
		return;

	struct mesh_adv_report_param para;
	para.info = sour[0];
	para.rssi = sour[1];
	para.length = len-8;
	para.data = &sour[8];
	mesh_ble_event_set(MESH_BLE_SCAN_RX_CB,&para);
    
	/* œ»÷«¿∂—¿µ∆ */
    Data_t  xAdvData = {para.data, para.length};
    Data_t  xTypeData;
    uint8_t err_code = vFindADStructure(0xFF, &xAdvData, &xTypeData);  
    if(err_code == 1)
    {
        vAdvDataParse(xTypeData.pucData, xTypeData.usDlen);
    }
#endif
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
	UART_RemapToPrintf(UARTB, GPIO_UART_TX, GPIO_UART_RX);
	m_print("-----------%s[%s]-----------\r\n", MESH_VER, MESH_LVER);
	M_PRINTF(L_APP,"mesh stack init start");

	bool aliStatus = ali_config_data_init();
	if(!aliStatus){
		M_PRINTF(L_APP,"ali parameter error!");
		while(1);
	}
		
	uint8_t bt_addr[6];
	ali_config_mac_read(bt_addr, 0);
	mesh_mac_set(true, bt_addr);
	
    memcpy(pucMacAddr, bt_addr, 6);
	HWRITE(mem_tx_power,0x02);
	
	tIPCControlBlock cb;
	memset(&cb, 0, sizeof(tIPCControlBlock));
	cb.blecb = Bt_BleCallBack;
	cb.evtcb = Event_call_back;
	cb.advcb = Mesh_Adv_Cb;
	//cb.readreqcb = ble_read_req_cb;
	cb.readconnparamcb = ble_read_conn_param;
	
	IPC_Initialize(&cb);	//Register callback function.

#if (BLE_MESH)	
	if(aliStatus){
		app_mesh_init();
		app_mesh_start();
	}
#endif/*BLE_MESH*/
	//WDT_Enable();

	gatt_ota_init();

    vMeshInit();
    vTimerInit ();
	vApplicationInit();

	while(1)
	{
		#if (BLE_MESH)
		if(OTANONE == gatt_ota_state_get())
			bc_m_mesh_loop();
		#endif/*BLE_MESH*/
		//WDT_Kick();

		//ble process
		mesh_ble_process();	
		
	}
}

