#ifndef __SYSTICK_H
#define __SYSTICK_H
#include <stdbool.h>
#include "common.h"
#include "core_cm0.h"

typedef int32_t  vs32;
typedef  int16_t  vs16;
typedef  int8_t   vs8;
/*callback function when systick handler is arrived*/
typedef void (*systick_irq_handler)(void);


#define TICK_PER_SECOND			1000 
#define TICK_US					(1000)

#define IMU_LOOP_TIME			2000
#define PID_INNER_LOOP_TIME		2000
#define PID_OUTER_LOOP_TIME		5000

#define GYRO_CF_TAU				8.5f

#define ACC_LPF_CUT				30.0f
#define GYRO_LPF_CUT			25.0f

/**
 *@brief Init system tick with 1ms interval.
 *param: None.
 *return: None.
 */
void systick_init(void);

/**
 *@brief Do delay, unit:us.
 *param: us	delay times.
 *return: None.
 */
void delay_us(uint32_t us);

/**
 *@brief Do delay, unit:ms.
 *param: ms	delay times.
 *return: None.
 */
void delay_ms(uint32_t ms);

/**
 *@brief Get system tick, unit:us.
 *param: None.
 *return: Current systick value.
 */
uint32_t systick_get_us(void);

/**
 *@brief Get system tick, unit:ms.
 *param: None.
 *return: Current systick value.
 */
uint32_t systick_get_ms(void);

/**
 *@brief Set timer irq handler, that will be call when systick is arrived.
 *param: func	systick_irq_handler function.
 *return: None.
 */
void systick_set_timer_irq_handler(systick_irq_handler func);

/**
 *@brief Set tick irq handler, that will be call when systick is arrived.
 *param: func	systick_irq_handler function.
 *return: None.
 */
void systick_set_tick_irq_handler(systick_irq_handler func);

#endif
