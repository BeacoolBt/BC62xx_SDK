/*************************************************************************
*
* beacool uart example  
*
*************************************************************************/
#include "common.h"

#include "bc62xx_uart.h"
#include "ProtocolRln.h"

/*for genie module*/
#define TX_PIN 27
#define RX_PIN 28

static void _rln_cb(const uint8_t* data, uint8_t len)
{
	//if(data == NULL || len == 0)
	//	return;
	/*echo for debug*/
	protocol_send(data,len);
}

static void _send_cb(const uint8_t* data, uint8_t len)
{
	for(int i = 0; i < len; ++i)
		USART_SendData(UARTB, (uint16_t)data[i]);
}

/**TEST DATA
 *3a ff 05 01 02 03 04 05 14
 */
int main()
{
#if 1
	/*init gpio*/	
	GPIO_CFG cfgTx ={FUNC_UARTB_TXD, GFG_PULLUP};
	bc_gpio_init(TX_PIN, &cfgTx);
	
	GPIO_CFG cfgRx ={FUNC_UARTB_RXD, GFG_PULLUP};
	bc_gpio_init(RX_PIN, &cfgRx);

	/*cofig uart parameter*/	
	USART_InitTypeDef Uart = {BAUD115200, DATA_BITS_8b, STOP_BITS_1, PARITY_EVEN,
								MODE_DUPLEX, FLOW_CTRL_NONE, 128, 128};
	USART_Init(UARTB, &Uart);
#else
	UART_RemapToPrintf(UARTB, TX_PIN, RX_PIN);
#endif
	uint8_t buff[50];

	protocol_init(_rln_cb, _send_cb);
	
	/*main process*/
	while(1){
		if(USART_GetRxCount(UARTB) > 0){
			int len = USART_ReadDatatoBuff(UARTB, buff, 0);
			for(int i = 0; i < len; ++i)
				USART_SendData(UARTB, buff[i]);
			protocol_recv(buff, len);
		}
		for(int i = 0; i < 5000; ++i){
			hw_delay();
		}
	}
	return 0;
}

