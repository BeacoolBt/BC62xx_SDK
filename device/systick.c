#include "systick.h" 

static volatile uint32_t tick_count = 0;

void systick_init(void)
{
	SysTick_Config(SystemCoreClock/TICK_PER_SECOND);
}

// Return system uptime in microseconds (rollover in 70minutes)
uint32_t systick_get_us(void) 
{
	register uint32_t ms, cycle_cnt;
	do {
		ms = tick_count;
		cycle_cnt = SysTick->VAL;
	} while (ms != tick_count);

	return ms * TICK_US + (SysTick->LOAD - cycle_cnt) * TICK_US / SysTick->LOAD;
}

uint32_t systick_get_ms(void)
{
	return tick_count;
}

void delay_us(uint32_t us)
{
    uint32_t now = systick_get_us();
    while (systick_get_us() - now < us);
}

void delay_ms(uint32_t ms)
{
	uint32_t now = tick_count;
	while (tick_count - now < ms);
}

/*timer irq callback function*/
static systick_irq_handler timer_irq_handler;
void systick_set_timer_irq_handler(systick_irq_handler func)
{
	timer_irq_handler = func;
}
/*systick irq callback function*/
static systick_irq_handler tick_irq_handler;
void systick_set_tick_irq_handler(systick_irq_handler func)
{
	tick_irq_handler = func;
}

void SysTick_Handler(void) 
{
	tick_count++;
	/*Call timer irq handler*/
	if(timer_irq_handler)
		timer_irq_handler();

	/*Call tick irq handler*/
	if(tick_irq_handler)
		tick_irq_handler();

}