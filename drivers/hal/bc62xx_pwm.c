#include "bc62xx_pwm.h"

union PWM_CTRL_REG {
	uint8_t all;
	struct PWM_CTRL_BITS bits;
};

typedef struct {
	uint16_t P0PCNT;
	uint16_t P0NCNT;
	__IO union PWM_CTRL_REG PWM0;
	uint16_t P1PCNT;
	uint16_t P1NCNT;
	__IO union PWM_CTRL_REG PWM1;
	uint16_t P2PCNT;
	uint16_t P2NCNT;
	__IO union PWM_CTRL_REG PWM2;
	uint16_t P3PCNT;
	uint16_t P3NCNT;
	__IO union PWM_CTRL_REG PWM3;
	uint16_t P4PCNT;
	uint16_t P4NCNT;
	__IO union PWM_CTRL_REG PWM4;
	uint16_t P5PCNT;
	uint16_t P5NCNT;
	__IO union PWM_CTRL_REG PWM5;
	uint16_t P6PCNT;
	uint16_t P6NCNT;
	__IO union PWM_CTRL_REG PWM6;
	uint16_t P7PCNT;
	uint16_t P7NCNT;
	__IO union PWM_CTRL_REG PWM7;
}__packed tPWM_CTRL_REG_GRP;

void PWM_Init(uint8_t channel, struct PWM_CTRL_BITS *bits)
{
	tPWM_CTRL_REG_GRP *pwm = (tPWM_CTRL_REG_GRP *)(reg_map(PWM_BASE));
	//pwm0
	if (channel&PWM_CH0)	pwm ->PWM0.bits = *bits;
	//pwm1
	if (channel&PWM_CH1)	pwm ->PWM1.bits = *bits;
	//pwm2
	if (channel&PWM_CH2)	pwm ->PWM2.bits = *bits;
	//pwm3
	if (channel&PWM_CH3)	pwm ->PWM3.bits = *bits;
	//pwm4
	if (channel&PWM_CH4)	pwm ->PWM4.bits = *bits;
	//pwm5
	if (channel&PWM_CH5)	pwm ->PWM5.bits = *bits;
	//pwm6
	if (channel&PWM_CH6)	pwm ->PWM6.bits = *bits;
	//pwm7
	if (channel&PWM_CH7)	pwm ->PWM7.bits = *bits;
}

void PWM_DeInit(uint8_t channel)
{
	tPWM_CTRL_REG_GRP *pwm = (tPWM_CTRL_REG_GRP *)(reg_map(PWM_BASE));
	//pwm0
	if (channel&PWM_CH0)	pwm ->PWM0.bits.ENABLE = PWM_DISABLE;
	//pwm1
	if (channel&PWM_CH1)	pwm ->PWM1.bits.ENABLE = PWM_DISABLE;
	//pwm2
	if (channel&PWM_CH2)	pwm ->PWM2.bits.ENABLE = PWM_DISABLE;
	//pwm3
	if (channel&PWM_CH3)	pwm ->PWM3.bits.ENABLE = PWM_DISABLE;
	//pwm4
	if (channel&PWM_CH4)	pwm ->PWM4.bits.ENABLE = PWM_DISABLE;
	//pwm5
	if (channel&PWM_CH5)	pwm ->PWM5.bits.ENABLE = PWM_DISABLE;
	//pwm6
	if (channel&PWM_CH6)	pwm ->PWM6.bits.ENABLE = PWM_DISABLE;
	//pwm7
	if (channel&PWM_CH7)	pwm ->PWM7.bits.ENABLE = PWM_DISABLE;
}

void PWM_Start(uint8_t channel)
{
	tPWM_CTRL_REG_GRP *pwm = (tPWM_CTRL_REG_GRP *)(reg_map(PWM_BASE));
	//pwm0
	if (channel&PWM_CH0)	pwm ->PWM0.bits.ENABLE = PWM_ENABLE;
	//pwm1
	if (channel&PWM_CH1)	pwm ->PWM1.bits.ENABLE = PWM_ENABLE;
	//pwm2
	if (channel&PWM_CH2)	pwm ->PWM2.bits.ENABLE = PWM_ENABLE;
	//pwm3
	if (channel&PWM_CH3)	pwm ->PWM3.bits.ENABLE = PWM_ENABLE;
	//pwm4
	if (channel&PWM_CH4)	pwm ->PWM4.bits.ENABLE = PWM_ENABLE;
	//pwm5
	if (channel&PWM_CH5)	pwm ->PWM5.bits.ENABLE = PWM_ENABLE;
	//pwm6
	if (channel&PWM_CH6)	pwm ->PWM6.bits.ENABLE = PWM_ENABLE;
	//pwm7
	if (channel&PWM_CH7)	pwm ->PWM7.bits.ENABLE = PWM_ENABLE;
}


void PWM_SetPnCnt(uint8_t channel, uint16_t pcnt, uint16_t ncnt)
{
	//tPWM_CTRL_REG_GRP *pwm = (tPWM_CTRL_REG_GRP *)(reg_map(PWM_BASE));

	
	if (channel&PWM_CH0) {
		HW_REG_16BIT(reg_map(CORE_PWM0_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM0_PCNT), pcnt);
	}

	if (channel&PWM_CH1) {
		HW_REG_16BIT(reg_map(CORE_PWM1_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM1_PCNT), pcnt);
	}

	if (channel&PWM_CH2) {
		HW_REG_16BIT(reg_map(CORE_PWM2_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM2_PCNT), pcnt);
	}

	if (channel&PWM_CH3) {
		HW_REG_16BIT(reg_map(CORE_PWM3_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM3_PCNT), pcnt);
	}

	if (channel&PWM_CH4) {
		HW_REG_16BIT(reg_map(CORE_PWM4_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM4_PCNT), pcnt);
	}

	if (channel&PWM_CH5) {
		HW_REG_16BIT(reg_map(CORE_PWM5_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM5_PCNT), pcnt);
	}

	if (channel&PWM_CH6) {
		HW_REG_16BIT(reg_map(CORE_PWM6_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM6_PCNT), pcnt);
	}

	if (channel&PWM_CH7) {
		HW_REG_16BIT(reg_map(CORE_PWM7_NCNT), ncnt);
		HW_REG_16BIT(reg_map(CORE_PWM7_PCNT), pcnt);
	}

}


