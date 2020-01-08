#include "bc62xx_gpio.h"
#include "common.h"

#ifndef RELEASE 
#define IS_VALID_PIN(PIN)	((PIN == GPIO_UART_RX) || \
							(PIN == GPIO_UART_TX) || \
							(PIN == GPIO_ADC) || \
							(PIN == GPIO_PWM5) || \
							(PIN == GPIO_PWM4) || \
							(PIN == GPIO_PWM3) || \
							(PIN == GPIO_PWM2) || \
							(PIN == GPIO_PWM1))
#else
#define IS_VALID_PIN(PIN)
#endif

void bc_gpio_init(uint8_t pin, GPIO_CFG* t)
{
	ASSERT(t != NULL);
	ASSERT(IS_VALID_PIN(pin));
	HWRITE(CORE_GPIO_CONF + pin, *(uint8_t*)t);
}

void bc_gpio_deinit(uint8_t pin, GPIO_CFG* t)
{
	ASSERT(t != NULL);
	ASSERT(IS_VALID_PIN(pin));
}

void bc_gpio_set(uint8_t pin, bool val)
{
	ASSERT(IS_VALID_PIN(pin));
	//GPIO_SetOut(pin, val);
	hw_delay();
	if (val)
		HWRITE(CORE_GPIO_CONF + pin, GPCFG_OUTPUT_HIGH);
	else
		HWRITE(CORE_GPIO_CONF + pin, GPCFG_OUTPUT_LOW);
	hw_delay();
}

bool bc_gpio_get(uint8_t pin)
{
	ASSERT(IS_VALID_PIN(pin));
	hw_delay();

	register uint8_t queue = 0;
	register uint8_t group = 0;
	uint8_t st = 0;
	
	queue = (pin & 7);
	group = pin >> 3 & 3;
	st = HREAD(CORE_GPIO_IN+group);
	return (st & (1 << queue));
}

#if 0
void gpio6_handler()
{
	printf("gpio6_handler\r\n");
	disable_intr(GPIO6_intr);
}

void gpio_main()
{
	printf("main init\r\n");
	GPIO_SetInput(6, 0);
	enable_intr(GPIO6_intr);
}
#endif


