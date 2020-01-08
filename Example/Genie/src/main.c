/*************************************************************************
*
* Yichip  
*
*************************************************************************/

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

#include "ProtocolRln.h"


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
		#if (BLE_MESH)
		case GATT_PROV_START_HANDLE+2:
		case GATT_PROXY_START_HANDLE+2:{
			m_printf_hex(L_APP, "DATA_IN RECV DATA", buf, len);
			struct mesh_write_req_param para;
			para.con = 0;
			para.handle = att_hdl;
			para.offset = 0;
			para.length = len-2;
			para.value = buf+2;
			mesh_ble_event_set(MESH_BLE_CON_WRITE_REQ,&para);
		}break;
		case GATT_PROV_START_HANDLE+5:
		case GATT_PROXY_START_HANDLE+5:{
			m_printf_hex(L_APP, "DATA_OUT_CCC RECV DATA", buf, len);
			struct mesh_write_req_param para;
			para.con = 0;
			para.handle = att_hdl;
			para.offset = 0;
			para.length = len-2;
			para.value = buf+2;
			mesh_ble_event_set(MESH_BLE_CON_WRITE_REQ,&para);
			//uint8_t ucT[40];
			///*for test*/mesh_con_notify(GATT_PROV_START_HANDLE+4,ucT, sizeof(ucT));
		}break;
		#endif
		case OTA_WRITE_HANDLE:{
			gatt_ota_data_deal(buf+2, len - 2);
		}break;
		default:{
			
		}break;
	}
}

void Event_call_back(uint8_t da)
{
	switch(da){
		case 0x65:
			printf("BLE 0x65\r\n");
			break;
		case IPC_EVT_LE_CONNECTED:{
			printf("BLE connect\r\n");
			//close scan & adv
			mesh_connect_do();
			#if (BLE_MESH)
			struct mesh_connected_param para;
			para.con = 0;
			mesh_ble_event_set(MESH_BLE_CON_CONNECTED,&para);
			#endif
		}break;
			
		case IPC_EVT_LE_DISCONNECTED:{
			printf("BLE disconnect\r\n");
			//restore scan&adv
			mesh_disconnect_do();
			gatt_ota_stop();
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
#ifdef CONFIG_FACTORY_TEST
		//for test
		app_mesh_hal_rx(HAL_TST, &sour[8], len-8);
#endif

#if (BLE_MESH)	
	if(OTANONE != gatt_ota_state_get())
		return;

	struct mesh_adv_report_param para;
	para.info = sour[0];
	para.rssi = sour[1];
	para.length = len-8;
	para.data = &sour[8];
	mesh_ble_event_set(MESH_BLE_SCAN_RX_CB,&para);
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

static void _rln_cb(const uint8_t* data, uint8_t len)
{
	app_mesh_hal_rx(HAL_UART, data, len);
}

static void _send_cb(const uint8_t* data, uint8_t len)
{
	for(int i = 0; i < len; ++i)
		USART_SendData(UARTB, (uint16_t)data[i]);
}
#ifdef RELEASE
static void _uartb_init(void)
{
	/*init gpio*/	
	GPIO_CFG cfgTx ={FUNC_UARTB_TXD, GFG_PULLUP};
	bc_gpio_init(GPIO_UART_TX, &cfgTx);
	
	GPIO_CFG cfgRx ={FUNC_UARTB_RXD, GFG_PULLUP};
	bc_gpio_init(GPIO_UART_RX, &cfgRx);

	/*cofig uart parameter*/	
	USART_InitTypeDef Uart = {BAUD9600, DATA_BITS_8b, STOP_BITS_1, PARITY_EVEN,
								MODE_DUPLEX, FLOW_CTRL_NONE, 128, 128};
	USART_Init(UARTB, &Uart);
}
#endif
int main()
{
#ifdef RELEASE
	_uartb_init();
#else
	UART_RemapToPrintf(UARTB, GPIO_UART_TX, GPIO_UART_RX);
#endif
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

	protocol_init(_rln_cb, _send_cb);

	while(1){
		#if (BLE_MESH)
		if(OTANONE == gatt_ota_state_get())
			bc_m_mesh_loop();
		#endif/*BLE_MESH*/
		//WDT_Kick();

		//ble process
		mesh_ble_process();
#ifdef RELEASE
		if(USART_GetRxCount(UARTB) > 0){
			uint8_t buff[50];
			int len = USART_ReadDatatoBuff(UARTB, buff, 0);
			protocol_recv(buff, len);
		}
#endif
	}
	return 0;
}

