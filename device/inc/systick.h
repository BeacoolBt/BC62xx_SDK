#ifndef __SYSTICK_H
#define __SYSTICK_H
#include <stdbool.h>
#include "common.h"
#include "core_cm0.h"

typedef int32_t  vs32;
typedef  int16_t  vs16;
typedef  int8_t   vs8;

#define TICK_PER_SECOND 1000 
#define TICK_US	(1000)

#define IMU_LOOP_TIME		    2000	//��λΪuS
#define PID_INNER_LOOP_TIME		2000	//��λΪus
#define PID_OUTER_LOOP_TIME		5000	//��λΪus

#define GYRO_CF_TAU 8.5f

#define ACC_LPF_CUT 30.0f	//���ٶȵ�ͨ�˲�����ֹƵ��
#define GYRO_LPF_CUT 25.0f		//�����ǵ�ͨ�˲�����ֹƵ��

//#define GYRO_CF_TAU 1.2f
/*---------------------------------------------*/

/*-------------------�������ݷ��ͷ�ʽѡ��-----------------*/
//#define ANO_DT_USE_Bluetooth
//#define ANO_DT_USE_NRF24l01
/*--------------------------------------------------------*/



extern uint32_t tick_count;
void systick_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
uint32_t systick_get_us(void);
uint32_t systick_get_ms(void);

#endif
