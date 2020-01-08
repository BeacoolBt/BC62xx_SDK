#ifndef __BC62XX_GPIO_H__
#define __BC62XX_GPIO_H__
/*
 * GPIO28/29 20k
 * 其它gpio 上下拉 10K电阻
 */

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#define GPIO_UART_RX	28//GPIO28
#define GPIO_UART_TX	27//GPIO27
#define GPIO_ADC		25//GPIO25
#define GPIO_PWM5		24//GPIO24
#define GPIO_PWM4		23//GPIO23
#define GPIO_PWM3		19//GPIO19
#define GPIO_PWM2		14//GPIO14
#define GPIO_PWM1		6//GPIO6
	
typedef enum 
{
	GFG_NONE = 0,
	GFG_PULLUP ,
	GFG_PULLDOWN,
	GFG_ANALOG,
}GPIO_PULL;

enum 
{
	FUNC_INPUT = 0,
	FUNC_QSPI_NCS = 2,
	FUNC_QSPI_SCK = 3,
	FUNC_QSPI_IO0 = 4,
	FUNC_QSPI_IO1 = 5,
	FUNC_QSPI_IO2 = 6,
	FUNC_QSPI_IO3 = 7,
	FUNC_UART_TXD = 8,
	FUNC_UART_RXD = 9,
	FUNC_UART_RTS = 10,
	FUNC_UART_CTS = 11,
	FUNC_UARTB_TXD = 12,
	FUNC_UARTB_RXD = 13,
	FUNC_UARTB_RTS = 14,
	FUNC_UARTB_CTS = 15,
	FUNC_PWM_OUT0 = 16,
	FUNC_PWM_OUT1,
	FUNC_PWM_OUT2,
	FUNC_PWM_OUT3,
	FUNC_PWM_OUT4,
	FUNC_PWM_OUT5,
	FUNC_PWM_OUT6,
	FUNC_PWM_OUT7,
	FUNC_I2S_DOUT,
	FUNC_I2S_LRCKOUT,
	FUNC_I2S_CLKOUT,
	FUNC_I2S_DIN = 28,
	FUNC_I2S_LRCKIN,
	FUNC_I2S_CLKIN,
	FUNC_SPID_MISO,
	FUNC_SPID_NCS,
	FUNC_SPID_SCK,
	FUNC_SPID_MOSI,
	FUNC_SPID_SDIO,
	FUNC_JTAG_SWCLK,
	FUNC_JTAG_SWDAT,
	FUNC_QDEC_X0,
	FUNC_QDEC_X1,
	FUNC_QDEC_Y0,
	FUNC_QDEC_Y1,
	FUNC_QDEC_Z0,
	FUNC_QDEC_Z1,
	FUNC_IIC_SDA,
	FUNC_IIC_SCL,
	FUNC_DAC_OUTP,
	FUNC_DAC_OUTN,
	FUNC_SWCLK = 60,
	FUNC_SWDAT,
	FUNC_OUTPUT_LOW,
	FUNC_OUTPUT_HIGH,
}GPIO_FUNC;


typedef struct
{
	uint8_t pin_func:6;
	uint8_t pin_cfg:2;//0 output; 1 pull up; 2 pull down; 3 analog func
}GPIO_CFG;

void bc_gpio_init(uint8_t pin, GPIO_CFG* t);

void bc_gpio_deinit(uint8_t pin, GPIO_CFG* t);

void bc_gpio_set(uint8_t pin, bool val);

bool bc_gpio_get(uint8_t pin);

#endif/*__BC62XX_GPIO_H__*/


