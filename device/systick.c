#include "systick.h" 
#include "bc62xx_timer.h"

uint32_t tick_count = 0;

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
void SysTick_Handler(void) 
{
	tick_count++;
	SYStick_handle();
}