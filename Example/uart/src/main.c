/*************************************************************************
*
* beacool uart example  
*
*************************************************************************/
#include "common.h"

#include "bc62xx_uart.h"

/*for genie module*/
#define TX_PIN 27
#define RX_PIN 28

int main()
{	
	/*init gpio*/	
	GPIO_CFG cfgTx ={FUNC_UARTB_TXD, GFG_PULLUP};
	bc_gpio_init(TX_PIN, &cfgTx);
	
	GPIO_CFG cfgRx ={FUNC_UARTB_RXD, GFG_PULLUP};
	bc_gpio_init(RX_PIN, &cfgRx);

	/*cofig uart parameter*/	
	USART_InitTypeDef Uart = {BAUD921600, DATA_BITS_8b, STOP_BITS_1, PARITY_EVEN,
								MODE_DUPLEX, FLOW_CTRL_NONE, 128, 128};
	USART_Init(UARTB, &Uart);

	uint8_t buff[50];
	/*main process*/
	while(1){
		if(USART_GetRxCount(UARTB) > 0){
			int len = USART_ReadDatatoBuff(UARTB, buff, 0);
			for(int i = 0; i < len; ++i)
				USART_SendData(UARTB, buff[i]);
		}
		for(int i = 0; i < 5000; ++i){
			hw_delay();
		}
	}
	return 0;
}

