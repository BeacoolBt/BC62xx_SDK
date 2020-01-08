#include "bc62xx_adc.h"

//static DRV_ADC_CONFIG *gpAdcConfig;
unsigned char gAdcCtrl;

void Drv_adcdma_init(DRV_ADC_CONFIG *pAdcConfig)
{
	unsigned short buffenEnd;

	int i,j;	
	buffenEnd = ((unsigned short)((uint32_t)pAdcConfig->mDmaBuf + pAdcConfig->mDmaBufLen) & 0xFFFF);
	//memset(gpAdcConfig->mDmaBuf, 0, gpAdcConfig->mDmaBufLen);

	//HWRITE(0x896c,0x2d);
	//HWRITE(0x896e,0x94);
	
	//HWRITE(0x8978,0x80);

//mic bias
	HWRITE(0x8978,0x80);
	HWRITE(0x8979,0xE0);
	HWRITE(0x897a,0x68);
	HWRITE(0x897b,0xa8);
	HWRITE(0x897c,0x55);
	HWRITE(0x897d,0x18);
	HWRITE(0x897e,0xF3);
	HWRITE(0x897f,0xE1);
	HWRITE(0x8980,0xFF);
	HWRITE(0x8981,0x30);

	HWRITE(0x897f,0xE3);
	HWRITE(0x897f,0xE1);
	HWRITE(0x897e,0xf3);	
	HWRITE(0x897e,0xE3);
	for (j =100;j>0;j--)
	{
		i = 10000;
		while(i--);
	}
	HWRITE(0x897e,0xf3);

	

//VDD
/**
	HWRITE(0x8978,0x80);
	HWRITE(0x8979,0xE0);
	HWRITE(0x897a,0x68);
	HWRITE(0x897b,0xa8);
	HWRITE(0x897c,0x55);
	HWRITE(0x897d,0x18);
	HWRITE(0x897e,0xF3);
	HWRITE(0x897f,0x1f);
	HWRITE(0x8980,0x00);
	HWRITE(0x8981,0x30);

	HWRITE(0x897f,0x1f);
	HWRITE(0x897f,0x1d);
	HWRITE(0x897e,0xf3);	
	HWRITE(0x897e,0xE3);
	for (j =100;j>0;j--)
	{
		i = 10000;
		while(i--);
	}
	HWRITE(0x897e,0xf3);
**/
	gAdcCtrl = pAdcConfig->mAdcConfig;

	//HWRITE(0x8038,0x08);
	
	//#define CORE_MIC_HPF   						0x8114
	//#define CORE_MIC_HPF_CTR					0x8115
	HWRITE(CORE_MIC_HPF_CTR,0x10);



	HWRITEW(CORE_ADCD_SADDR, pAdcConfig->mDmaBuf);
	HWRITEW(CORE_ADCD_EADDR, buffenEnd-1);
	//HWRITEW(CORE_ADCD_SADDR, 0x300);
	//HWRITEW(CORE_ADCD_EADDR, 0x6FF);
	//HWRITEW(CORE_ADCD_DELAY, pAdcConfig->mSampleDelay);
	//HWRITEW(CORE_ADCD_DELAY, 80);

	return;
}


void Dvr_adc_init() 
{
	return;
}

void Drv_adc_enable(void)
{
	//HWRITE(CORE_ADCD_CTRL, DRV_ADC_ENABLE | gAdcCtrl);
	HWRITE(CORE_ADCD_CTRL, DRV_ADC_ENABLE | gAdcCtrl);
	return;
}

unsigned short Drv_adc_getWptr()
{
	return HREADW(CORE_ADCD_ADDR);	
}

void Drv_adc_disable(void)
{
	HWRITE(CORE_ADCD_CTRL, gAdcCtrl);
	return;
}

void Drv_adc_set_pga() 
{
	return;
}

void Dvr_adc_set_bias() 
{
	return;
}

void Drv_adc_sampleDelay() 
{
	return;
}

void Drv_adc_data_point() 
{
	return;
}

/**
void DRV_adc_checkPacket()
{
	int audioLen,framelen,frame_len;
	int delaycount;

	int audioReadPtr = HREADW(CORE_ADCD_ADDR) ;
	
	if (audioReadPtr >= TO_16BIT_ADDR(gpAdcConfig->mReadPtr))
	{
		while((audioReadPtr -  TO_16BIT_ADDR(gpAdcConfig->mReadPtr)) >= MIC_ONE_PACKET_LEN)
		{
			{
				DEBUG_PRINT2(gpAdcConfig->mReadPtr,MIC_ONE_PACKET_LEN);
			}

			gpAdcConfig->mReadPtr+=MIC_ONE_PACKET_LEN;
		}
	}
	else
	{
		while(TO_16BIT_ADDR(gpAdcConfig->mReadPtr) != TO_16BIT_ADDR(gpAdcConfig->mReadBufPtr))
		{
			{
				DEBUG_PRINT2(gpAdcConfig->mReadPtr,MIC_ONE_PACKET_LEN);
			}

			{
				gpAdcConfig->mReadPtr+=MIC_ONE_PACKET_LEN;
				if (gpAdcConfig->mReadPtr == gpAdcConfig->mEndPtr)
					gpAdcConfig->mReadPtr = gpAdcConfig->mReadBufPtr;
			}
		}
	}
}
**/


