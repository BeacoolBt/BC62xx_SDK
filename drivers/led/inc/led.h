#ifndef __LED_H__
#define __LED_H__
#include "stdio.h"
#include "bc62xx_gpio.h"

#define ALI_UART_RX	GPIO_UART_RX
#define ALI_UART_TX	GPIO_UART_TX
#define ALI_ADC		GPIO_ADC
#define ALI_PWM5_W	GPIO_PWM5
#define ALI_PWM4_C	GPIO_PWM4
#define ALI_PWM3_B	GPIO_PWM3
#define ALI_PWM2_G	GPIO_PWM2
#define ALI_PWM1_R	GPIO_PWM1
#ifndef ALI_MIN_LN_VAL
#define ALI_MIN_LN_VAL		(0x28f)
//#define ALI_MIN_LN_VAL		(0x28f*5)
#endif
#define ALI_MAX_LN_VAL		(100*0x28f)
#define ALI_DEFAULT_LN_VAL	ALI_MAX_LN_VAL

#define ALI_MIN_TP_VAL		(800)
#define ALI_MAX_TP_VAL		(100*192+800)
#define ALI_DEFAULT_TP_VAL	(50*192+800)

#define ALI_DEFAULT_R_VAL	(65535)
#define ALI_DEFAULT_G_VAL	(65535)
#define ALI_DEFAULT_B_VAL	(65535)

#define ALI_CW_OFF 0
#define ALI_CW_ON 1
#define ALI_DEFAULT_ON_VAL	ALI_CW_ON

#define ALI_FREQUENCY_1K	24000
#define ALI_FREQUENCY_2K	12000
#define ALI_FREQUENCY_3K	8000
#define ALI_FREQUENCY_4K	6000
#define ALI_FREQUENCY_5K	4800
#define ALI_FREQUENCY_6K	4000
#define ALI_FREQUENCY_8K	3000
#define ALI_FREQUENCY_10K	2400
#define ALI_FREQUENCY_12K	2000
#define ALI_FREQUENCY_16K	1500
#ifndef ALI_CURRENT_FREQ
#define ALI_CURRENT_FREQ	ALI_FREQUENCY_2K
#endif

#define BREATH_MODE_FLOW		(0)
#define BREATH_MODE_H2L2H		(1)


#define BREATH_TIME_PERIOD	(40)//40ms

#ifndef LIGHT_CW_WITH_RGB
#define LIGHT_CW_WITH_RGB	(0)
#endif

typedef struct{
	uint8_t mode:3;//0:L2H; 1:L2H2L
	uint8_t rCnt:5;// repeat times
	uint16_t pCnt;// BREATH_TIME_PERIOD*pCnt = period
}breath_t;	

typedef struct
{
	breath_t* breath;//for brearh lamp
	uint16_t freq;//pwm frequency
	/*For CW light control*/
	uint8_t cPin;
	uint8_t wPin;
	uint8_t on;//on/off	
	uint8_t onTarget;//on/off	
	uint16_t ln;
	uint16_t lnTarget;
	uint16_t tp;
	uint16_t tpTarget;
	/*For RGB light control*/
#if (LIGHT_CW_WITH_RGB)
	uint8_t onRgb;//on/off	
	uint8_t onRgbTarget;//on/off	
	uint16_t lnRgb;
	uint16_t lnRgbTarget;
	uint16_t rVal;
	uint16_t rValTarg;
	uint16_t gVal;
	uint16_t gValTarg;
	uint16_t bVal;
	uint16_t bValTarg;
	uint8_t lmode;//0 for cw; 1 for rgb
#endif
}light_cw_t;

/*
 *FOR WARM/COLD
 */

/**
 ****************************************************************************************
 * @brief Set temperature for cw light
 *
 * @param[in] temperature val, range in [800,20000]
 *
 * @return None
 ****************************************************************************************
 */
void led_cw_set_temperature(uint16_t tp, breath_t* breath);

/**
 ****************************************************************************************
 * @brief get temperature from cw light
 *
 * @param None
 *
 * @return temperature val
 ****************************************************************************************
 */
uint16_t led_cw_get_temperature(void);

/**
 ****************************************************************************************
 * @brief Set lightness for cw light
 *
 * @param[in] lightness val, range in [0x28f,0x28f00]
 *
 * @return None
 ****************************************************************************************
 */
void led_cw_set_lightness(uint16_t ln, breath_t* breath);

/**
 ****************************************************************************************
 * @brief get lightness from cw light
 *
 * @param None
 *
 * @return lightness val
 ****************************************************************************************
 */
uint16_t led_cw_get_lightness(void);

/**
 ****************************************************************************************
 * @brief trun on/off the cw light
 *
 * @param[in] on on/off val, 0 off, 1 on
 *
 * @return None
 ****************************************************************************************
 */
void led_cw_set_on_off(uint8_t on, breath_t* breath);

/**
 ****************************************************************************************
 * @brief get on/off status from cw light
 *
 * @param None
 *
 * @return on/off val
 ****************************************************************************************
 */
uint8_t led_cw_get_on_off(void);

#if (LIGHT_CW_WITH_RGB)
/**
 ****************************************************************************************
 * @brief Set color for rgb light
 *
 * @param[i] r val, range in [0,0xffff]
 * @param[i] g val, range in [0,0xffff]
 * @param[i] b val, range in [0,0xffff]
 *
 * @return None
 ****************************************************************************************
 */
int led_cwrgb_set_color(uint16_t r, uint16_t g, uint16_t b, breath_t* breath);

/**
 ****************************************************************************************
 * @brief get color from rgb light
 *
 * @param[o] r val, range in [0,0xffff]
 * @param[o] g val, range in [0,0xffff]
 * @param[o] b val, range in [0,0xffff]
 *
 * @return None
 ****************************************************************************************
 */
int led_cwrgb_get_color(uint16_t* r, uint16_t* g, uint16_t* b);
#endif

/**
 ****************************************************************************************
 * @brief init cw light
 *
 * @param[in] light_cw_t
 *
 * @return None
 ****************************************************************************************
 */
void led_cw_init(light_cw_t* t);

/**
 ****************************************************************************************
 * @brief set cw breath parameter 
 * Default warm pin->WALI_PWM5_W
 * Default Cold pin->GPIO_PWM4
 *
 * @param[in] breath_t
 *
 * @return None
 ****************************************************************************************
 */
void led_cw_set_breath(breath_t* t);

/**
 ****************************************************************************************
 * @brief led cw do breath function
 * @param None
 * @return 0 success
 			1 need continue do
 			2 failed
 ****************************************************************************************
 */
uint8_t  led_cw_do_breath(void);

#endif/*__LED_H__*/

