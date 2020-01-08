/** 
  *@file pwm.h
  *@brief PWM support for application.
  */
#ifndef _bc62XX_PWM_H_
#define _bc62XX_PWM_H_

#include "common.h"
#include "bc62xx_gpio.h"

/**
  *@brief PWM register base.
  */
#define PWM_BASE CORE_PWM0_PCNT

/**
  *@brief PWM configure parameter.
  */
struct PWM_CTRL_BITS{
	uint8_t VAL:3;			/*!< pwm val */
	uint8_t SYNC:1;			/*!< sync with pwm0 */
	uint8_t HIGHF:1;		/*!< high level first */
	uint8_t ENABLE:1;		/*!< pwm enable */
};

/**
  *@brief Sync.
  */
#define PWM_SYNC_DEFAULT 0
#define PWM_SYNC_ENABLE 1

/**
  *@brief High first.
  */
#define PWM_HIGHF_ENABLE 1
#define PWM_HIGHF_DISABLE 0

/**
  *@brief PWM enable.
  */
#define PWM_ENABLE 1
#define PWM_DISABLE 0

/**
  *@brief PWM channel.
  */
#define PWM_CH0 1
#define PWM_CH1 (1<<1)
#define PWM_CH2 (1<<2)
#define PWM_CH3 (1<<3)
#define PWM_CH4 (1<<4)
#define PWM_CH5 (1<<5)
#define PWM_CH6 (1<<6)
#define PWM_CH7 (1<<7)
#define PWM_CH_COUNT	8

/**
  *@brief PWM initialization.
  *@param channel the channel that outputs the PWM.
  *@param bits PWM configure structure.@ref PWM_CTRL_BITS
  *@return None.
  */
void PWM_Init(uint8_t channel, struct PWM_CTRL_BITS *bits);

/**
  *@brief cancel PWM initialization.
  *@param channel the channel that outputs the PWM.
  *@return None.
  */
void PWM_DeInit(uint8_t channel);

/**
  *@brief Open PWM output.
  *@param channel the channel that outputs the PWM.
  *@return None.
  */
void PWM_Start(uint8_t channel);

/**
  *@brief PWM initialization.
  *@param channel the channel that outputs the PWM.
  *@param pcnt positive pulse width.
  *@param ncnt negative pulse width. 
  *@return None.
  */
void PWM_SetPnCnt(uint8_t channel, uint16_t pcnt, uint16_t ncnt);

#endif/*_bc62XX_PWM_H_*/

