/** 
  *@file adc.h
  *@brief adc support for application.
  */
#ifndef _BC62XX_ADC_H_
#define _BC62XX_ADC_H_
#include "common.h"

/**
  *@brief ADC configure.
  */
#define DRV_ADC_M0RAM_ENABLE	0x40
#define DRV_ADC_FILTER_DISABLE	0x20
#define DRV_ADC_ENABLE			0x80
#define DRV_ADC_8K_FILTer		0x10
#define DRV_ADC_CLEAR_RSSI	0x08
#define DRV_ADC_AVG_TIMER_RSSI  0x06
#define DRV_ADC_RSSI_CLOCK	 0x01

/**
  *@brief ADC gpio.
  */
#define DRV_ADC_CHANNEL_SIG_GPIO6	0x0001
#define DRV_ADC_CHANNEL_SIG_GPIO7	0x0002
#define DRV_ADC_CHANNEL_SIG_GPIO8	0x0004
#define DRV_ADC_CHANNEL_SIG_GPIO9	0x0008
#define DRV_ADC_CHANNEL_SIG_GPIO10	0x0010
#define DRV_ADC_CHANNEL_SIG_GPIO11	0x0020
#define DRV_ADC_CHANNEL_SIG_GPIO12	0x0040
#define DRV_ADC_CHANNEL_SIG_GPIO13	0x0080
#define  DRV_ADC_CHANNEL_DIFF_GPIO67	0x0100
#define  DRV_ADC_CHANNEL_DIFF_GPIO89	0x0200
#define  DRV_ADC_CHANNEL_DIFF_GPIO1011	0x0400
#define  DRV_ADC_CHANNEL_DIFF_GPIO1213	0x0800

/**
  *@brief ADC channel.
  */
#define DRV_ADC_PGA_0 0x00
#define DRV_ADC_PGA_1 0x01
#define DRV_ADC_PGA_2 0x02
#define DRV_ADC_PGA_3 0x03
#define DRV_ADC_PGA_4 0x04
#define DRV_ADC_PGA_5 0x05
#define DRV_ADC_PGA_6 0x06
#define DRV_ADC_PGA_7 0x07

/**
  *@brief ADC calibration value.
  */
#define DRV_ADC_BIAS_15V 0x00
#define DRV_ADC_BIAS_18V 0x10
#define DRV_ADC_BIAS_20V 0x20
#define DRV_ADC_BIAS_25V 0x30

/**
  *@brief ADC driver Configure definition.
  */
typedef struct
{
	unsigned char *mDmaBuf;	/*!< adc DMA start address */
	int16_t mDmaBufLen;		/*!< adc DMA buffer length */
	uint16_t mSampleDelay;	/*!< adc channel switch delay counter, based on 3M */
	uint16_t mChannelCfg;	/*!< adc channel switch */
	uint8_t mAdcConfig;		/*!< adc ctrl byte */
} DRV_ADC_CONFIG;

/**
  *@brief ADC initialization.
  *@param None.
  *@return None.
  */
void Dvr_adc_init();

/**
  *@brief ADC initialization.
  *@param pAdcConfig ADC initialize configure.@ref DRV_ADC_CONFIG
  *@return None.
  */
void Drv_adcdma_init(DRV_ADC_CONFIG *pAdcConfig);

/**
  *@brief Enable adc.
  *@param None.
  *@return None.
  */
void Drv_adc_enable();

/**
  *@brief Disable adc.
  *@param None.
  *@return None.
  */
void Drv_adc_disable();

/**
  *@brief Setting pga.
  *@param None.
  *@return None.
  */
void Drv_adc_set_pga();

/**
  *@brief Setting bias.
  *@param None.
  *@return None.  
  */
void Dvr_adc_set_bias();

/**
  *@brief ADC setting delay time.
  *@param None.
  *@return None.
  */
void Drv_adc_sampleDelay();

/**
  *@brief Get current ADC DMA write pointer.
  *@param None.
  *@return Current ADC DMA write pointer. 
  */
unsigned short Drv_adc_getWptr();

#endif/*_BC62XX_ADC_H_*/

