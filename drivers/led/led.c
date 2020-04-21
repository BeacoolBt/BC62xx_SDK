#include "led.h"
#include "bc62xx_pwm.h"
#include "systick.h" 

#ifndef RELEASE
#define RELEASE
#endif

#ifndef BREATH_CTL_PEROID
#define BREATH_CTL_PEROID	(10)//10ms
#endif


//for gpio control:GPIO_SetOut(6, (bool)val);
static light_cw_t lcw;

static uint16_t ucPeriodCnt;
static uint16_t deltaLn;
static uint16_t deltaTp;

#if (LIGHT_CW_WITH_RGB)
static float krgb;
static uint16_t deltaR;
static uint16_t deltaG;
static uint16_t deltaB;
static uint16_t deltaLnRgb;
#endif

static float kScale;


/***********************************************************
MIN	800

MAX	20000
***********************************************************/

static void _cw_ctrl(light_cw_t* t);

static void _breath_timer_start(void)
{
	ucPeriodCnt = 0;
	uint32_t _time = systick_get_ms();
	while(1 == led_cw_do_breath()){
		while(systick_get_ms() - _time < BREATH_CTL_PEROID);
		_time = systick_get_ms();
	}
}

void led_cw_set_temperature(uint16_t tp, breath_t* breath)
{
#if (LIGHT_CW_WITH_RGB)
	//when mode changed from rgb to cw, do status change
	if(lcw.lmode != 0){
		lcw.lmode = 0;
		//do temperature change
		lcw.tp = tp;
		lcw.tpTarget = lcw.tp;
		if(breath){
			//switch on CW, lightness to RGB
			lcw.lnTarget = lcw.lnRgb;
			lcw.ln = 0;
			lcw.on = 0;
			lcw.onTarget = 1;
			deltaLn = lcw.lnTarget>=lcw.ln?lcw.lnTarget-lcw.ln:0;
			deltaLn = deltaLn/breath->pCnt;

			//switch off RGB, 
			lcw.lnRgbTarget = 0;
			//lcw.lnRgb = lcw.lnRgb;
			lcw.onRgbTarget = 0;
			lcw.onRgb = 1;
			deltaLnRgb = lcw.lnRgb>lcw.lnRgbTarget?lcw.lnRgb-lcw.lnRgbTarget:0;
			deltaLnRgb = deltaLnRgb/breath->pCnt;

			if(deltaLn || deltaLnRgb){
				lcw.breath = breath;
				_breath_timer_start();
				return;
			}
		}
		lcw.on = 1;
		lcw.onTarget = 1;
		lcw.onRgbTarget = 0;
		lcw.onRgb = 0;
		_cw_ctrl(&lcw);
		return;
	}
#endif
	if(breath){
		deltaTp = (tp > lcw.tp)?(tp - lcw.tp):(lcw.tp - tp);
		deltaTp= deltaTp/breath->pCnt;
		if(deltaTp){
			lcw.breath = breath;
			lcw.tpTarget = tp;
			_breath_timer_start();
			return;
		}
	}
	
	lcw.tp = tp;
	lcw.tpTarget = lcw.tp;
	_cw_ctrl(&lcw);
}

uint16_t led_cw_get_temperature(void)
{
	return lcw.tpTarget;
}

void led_cw_set_lightness(uint16_t ln, breath_t* breath)
{
#if (LIGHT_CW_WITH_RGB)
	if(lcw.lmode == 1){
		if(breath){
			deltaLnRgb = (ln > lcw.lnRgb)?(ln - lcw.lnRgb):(lcw.lnRgb - ln);
			deltaLnRgb = deltaLnRgb/breath->pCnt;
			if(deltaLnRgb){
				lcw.breath = breath;
				lcw.lnRgbTarget = ln;
				_breath_timer_start();
				return;
			}
		}
		lcw.lnRgb = ln;
		lcw.lnRgbTarget = lcw.lnRgb;
	}
	else
#endif
	{
		if(breath){
			deltaLn = (ln > lcw.ln)?(ln - lcw.ln):(lcw.ln - ln);
			deltaLn = deltaLn/breath->pCnt;
			if(deltaLn){
				lcw.breath = breath;
				lcw.lnTarget = ln;
				_breath_timer_start();
				return;
			}
		}
		lcw.ln = ln;
		lcw.lnTarget = lcw.ln;
	}
	_cw_ctrl(&lcw);
}
uint16_t led_cw_get_lightness(void)
{
#if (LIGHT_CW_WITH_RGB)
	if(lcw.lmode == 1)
		return lcw.lnRgbTarget;
#endif
	return lcw.lnTarget;
}

void led_cw_set_on_off(uint8_t on, breath_t* breath)
{
#if (LIGHT_CW_WITH_RGB)
	if(lcw.lmode == 1){
		if(breath && (on != lcw.onRgb)){//the target lightness need to be restore
			deltaLnRgb = lcw.lnRgb>=ALI_MIN_LN_VAL?lcw.lnRgb-ALI_MIN_LN_VAL:0;
			deltaLnRgb = deltaLnRgb/breath->pCnt;
			if(deltaLnRgb){
				lcw.lnRgbTarget = lcw.lnRgb;
				if(on){
					lcw.lnRgb = ALI_MIN_LN_VAL;
				}
				lcw.breath = breath;
				lcw.onRgbTarget = on;
				_breath_timer_start();
				return;
			}
		}
		lcw.onRgb = on;
		lcw.onRgbTarget = lcw.onRgb;
	}
	else
#endif
	{
		if(breath && (on != lcw.on)){//the target lightness need to be restore
			deltaLn = lcw.ln>=ALI_MIN_LN_VAL?lcw.ln-ALI_MIN_LN_VAL:0;
			deltaLn = deltaLn/breath->pCnt;
			if(deltaLn){
				lcw.lnTarget = lcw.ln;
				if(on){
					lcw.ln = ALI_MIN_LN_VAL;
				}
				lcw.breath = breath;
				lcw.onTarget = on;
				_breath_timer_start();
				return;
				}
		}
		lcw.on = on;
		lcw.onTarget = lcw.on;
	}

	_cw_ctrl(&lcw);
}
uint8_t led_cw_get_on_off(void)
{
#if (LIGHT_CW_WITH_RGB)
	if(lcw.lmode == 1)
		return lcw.onRgbTarget;
#endif
	return lcw.onTarget;
}
#if (LIGHT_CW_WITH_RGB)
int led_cwrgb_set_color(uint16_t r, uint16_t g, uint16_t b, breath_t* breath)
{
	//when mode changed from cw to rgb, quickly changed status
#if (LIGHT_CW_WITH_RGB)
	//when mode changed from rgb to cw, do status change
	if(lcw.lmode != 1){
		lcw.lmode = 1;
		//color changed
		lcw.rVal = r;
		lcw.gVal = g;
		lcw.bVal = b;
		lcw.rValTarg = lcw.rVal;
		lcw.gValTarg = lcw.gVal;
		lcw.bValTarg = lcw.bVal;
		if(breath){
			//switch on RGB, lightness to CW
			lcw.lnRgbTarget = lcw.ln;
			lcw.lnRgb = 0;
			lcw.onRgbTarget = 1;
			//lcw.onRgb = 0;
			deltaLnRgb = lcw.lnRgbTarget>lcw.lnRgb?lcw.lnRgbTarget-lcw.lnRgb:0;
			deltaLnRgb = deltaLnRgb/breath->pCnt;
			//switch off CW
			lcw.lnTarget = 0;
			//lcw.ln = ALI_MIN_LN_VAL;
			lcw.on = 1;
			lcw.onTarget = 0;
			deltaLn = lcw.ln>=lcw.lnTarget?lcw.ln-lcw.lnTarget:0;
			deltaLn = deltaLn/breath->pCnt;
			
			if(deltaLnRgb || deltaLn){
				lcw.breath = breath;
				_breath_timer_start();
				return 0;
			}			
		}
		lcw.on = 0;
		lcw.onTarget = 0;
		lcw.onRgbTarget = 1;
		lcw.onRgb = 1;
		_cw_ctrl(&lcw);
		return 0;
	}
#endif
	if(breath){//the target lightness need to be restore
		uint32_t temp;
		temp = r>lcw.rVal?r-lcw.rVal:lcw.rVal-r;
		deltaR = temp/breath->pCnt;
		temp = g>lcw.gVal?g-lcw.gVal:lcw.gVal-g;
		deltaG = temp/breath->pCnt;
		temp = b>lcw.bVal?b-lcw.bVal:lcw.bVal-b;
		deltaB = temp/breath->pCnt;
		if(deltaR || deltaG || deltaB){
			lcw.rValTarg = r;
			lcw.gValTarg = g;
			lcw.bValTarg = b;
			lcw.breath = breath;
			_breath_timer_start();
			return 0;
		}
	}

	lcw.rVal = r;
	lcw.rValTarg = lcw.rVal;
	lcw.gVal = g;
	lcw.gValTarg = lcw.gVal;
	lcw.bVal = b;
	lcw.bValTarg = lcw.bVal;
	_cw_ctrl(&lcw);
	return 0;
}

int led_cwrgb_get_color(uint16_t* r, uint16_t* g, uint16_t* b)
{
	*r = lcw.rValTarg;
	*g = lcw.gValTarg;
	*b = lcw.bValTarg;
	return 0;
}
#endif

void led_cw_init(light_cw_t* t)
{
	/*init gpio*/	
	GPIO_CFG cfg4 ={FUNC_PWM_OUT4, GFG_PULLUP};
	bc_gpio_init(ALI_PWM4_C, &cfg4);
	
	GPIO_CFG cfg5 ={FUNC_PWM_OUT5, GFG_PULLUP};
	bc_gpio_init(ALI_PWM5_W, &cfg5);
#if (LIGHT_CW_WITH_RGB)
	GPIO_CFG cfg1 ={FUNC_PWM_OUT1, GFG_PULLUP};
	bc_gpio_init(ALI_PWM1_R, &cfg1);
	
	GPIO_CFG cfg2 ={FUNC_PWM_OUT2, GFG_PULLUP};
	bc_gpio_init(ALI_PWM2_G, &cfg2);
	
	GPIO_CFG cfg3 ={FUNC_PWM_OUT3, GFG_PULLUP};
	bc_gpio_init(ALI_PWM3_B, &cfg3);
#endif
	
	struct PWM_CTRL_BITS bits = {0};
	PWM_Init(PWM_CH4, &bits);
	PWM_Init(PWM_CH5, &bits);
#if (LIGHT_CW_WITH_RGB)
	PWM_Init(PWM_CH1, &bits);
	PWM_Init(PWM_CH2, &bits);
	PWM_Init(PWM_CH3, &bits);
#endif
	if(t==NULL){
		lcw.ln = ALI_DEFAULT_LN_VAL;
		lcw.tp = ALI_DEFAULT_TP_VAL;
		lcw.on = ALI_DEFAULT_ON_VAL;
		lcw.freq = ALI_FREQUENCY_16K;
#if (LIGHT_CW_WITH_RGB)
		lcw.onRgb = ALI_CW_OFF;
		lcw.lnRgb = ALI_DEFAULT_LN_VAL;
		lcw.rVal = ALI_DEFAULT_R_VAL;
		lcw.gVal = ALI_DEFAULT_G_VAL;
		lcw.bVal = ALI_DEFAULT_B_VAL;
		lcw.lmode = 0;
#endif
	}
	else{
		lcw = *t;
	}
	lcw.lnTarget = lcw.ln;
	lcw.tpTarget = lcw.tp;
	lcw.onTarget = lcw.on;
#if (LIGHT_CW_WITH_RGB)
	lcw.lnRgbTarget = lcw.lnRgb;
	lcw.onRgbTarget = lcw.onRgb;
	lcw.rValTarg = lcw.rVal;
	lcw.gValTarg = lcw.gVal;
	lcw.bValTarg = lcw.bVal;
	krgb = (float)lcw.freq/65536/4096;
#ifndef RELEASE
	printf("krgb[%f]\r\n", krgb);
#endif
#endif
	kScale = (float)lcw.freq/65536/1920;
#ifndef RELEASE
	printf("kScale[%f]\r\n", kScale);
#endif
	_cw_ctrl(&lcw);
}

static void _cw_ctrl(light_cw_t* t)
{
	//control CW light
	if(t->on || t->onTarget){
		uint16_t c,w;
		c = kScale*(lcw.tp-800)*lcw.ln/10;
		w = kScale*(20000 -lcw.tp)*lcw.ln/10;
#ifndef RELEASE
		printf("on[%d] ln[%d] tp[%d] c[%d] w[%d]\r\n", t->on||t->onTarget, t->ln, t->tp, c, w);
#endif
		if(c){
			PWM_SetPnCnt(PWM_CH4, c, lcw.freq-c);//cold
			PWM_Start(PWM_CH4);
		}
		else{
			PWM_DeInit(PWM_CH4);
		}
		if(w){
			PWM_SetPnCnt(PWM_CH5, w, lcw.freq-w);//warm
			PWM_Start(PWM_CH5);
		}
		else{
			PWM_DeInit(PWM_CH5);
		}
	}
	else{
		PWM_DeInit(PWM_CH4);
		PWM_DeInit(PWM_CH5);
	}

	//control RGB light
#if (LIGHT_CW_WITH_RGB)
	if(t->onRgb || t->onRgbTarget){
#ifndef RELEASE
		printf("on[%x] r[%x] g[%x] b[%x]\r\n",t->onRgb||t->onRgbTarget, t->rVal, t->gVal, t->bVal);
#endif
		if(t->rVal){
			uint16_t r = (t->rVal>>4)*t->lnRgb*krgb;
			PWM_SetPnCnt(PWM_CH1, r, t->freq-r);//cold
			PWM_Start(PWM_CH1);
		}
		else{
			PWM_DeInit(PWM_CH1);
		}
		if(t->gVal){
			uint16_t g = (t->gVal>>4)*t->lnRgb*krgb;
			PWM_SetPnCnt(PWM_CH2, g, t->freq-g);//warm
			PWM_Start(PWM_CH2);
		}
		else{
			PWM_DeInit(PWM_CH2);
		}
		if(t->bVal){
			uint16_t b = (t->bVal>>4)*t->lnRgb*krgb;
			PWM_SetPnCnt(PWM_CH3, b, t->freq-b);//warm
			PWM_Start(PWM_CH3);
		}
		else{
			PWM_DeInit(PWM_CH3);
		}
	}
	else{//CW mode, close RGB
		PWM_DeInit(PWM_CH1);
		PWM_DeInit(PWM_CH2);
		PWM_DeInit(PWM_CH3);
	}
#endif
}

void led_cw_set_breath(breath_t* t)
{
	switch(t->mode){
		case BREATH_MODE_FLOW:{
		}break;
		case BREATH_MODE_H2L2H:{
			#if (LIGHT_CW_WITH_RGB)
			if(lcw.lmode == 1){
				lcw.onRgbTarget = 1;
				lcw.lnRgbTarget = lcw.lnRgb;
				deltaLnRgb = lcw.lnRgbTarget/t->pCnt;
				if(deltaLnRgb == 0)
					return;
			}
			else
			#endif
			{
				lcw.onTarget = 1;
				lcw.lnTarget = lcw.ln;
				deltaLn = lcw.lnTarget/t->pCnt;
				if(deltaLn == 0)
					return;
			}
		}break;
		default:{
			return;
		}break;
	}
	lcw.breath = t;
#ifndef RELEASE
	printf("mode[%x] rCnt[%x] pCnt[%x]", lcw.breath->mode, lcw.breath->rCnt,
											lcw.breath->pCnt);
#endif
	_breath_timer_start();
}

/**
 ****************************************************************************************
 * @brief led cw do breath function
 * @param None
 * @return 0 success
 			1 need continue do
 			2 failed
 ****************************************************************************************
 */
uint8_t  led_cw_do_breath(void)
{
	if(lcw.breath == NULL || (deltaLn == 0 && deltaTp == 0
#if (LIGHT_CW_WITH_RGB)
		&& deltaLnRgb == 0 && deltaB == 0 && deltaG == 0 && deltaR == 0
#endif
		)){
		//use target ctl
		return 2;
	}
	uint8_t _sts = 1;
#ifndef RELEASE
	printf("deltaLn[%x]deltaTp[%x]ucPeriodCnt[%x] mode[%x]\r\n", deltaLn, deltaTp, ucPeriodCnt, lcw.breath->mode);
#endif
	
	switch(lcw.breath->mode){
		case BREATH_MODE_FLOW:{
			if(ucPeriodCnt < lcw.breath->pCnt){
				if(lcw.onTarget > lcw.on 
					|| ((lcw.onTarget == lcw.on) 
					&& lcw.lnTarget > lcw.ln))
					lcw.ln += deltaLn;
				else 
					lcw.ln -= deltaLn;
#if (LIGHT_CW_WITH_RGB)
				if(lcw.onRgbTarget > lcw.onRgb 
					|| ((lcw.onRgbTarget == lcw.onRgb) 
					&& lcw.lnRgbTarget > lcw.lnRgb))
					lcw.lnRgb += deltaLnRgb;
				else 
					lcw.lnRgb -= deltaLnRgb;
#endif

				lcw.tp = lcw.tpTarget > lcw.tp?lcw.tp + deltaTp:lcw.tp - deltaTp;
#if (LIGHT_CW_WITH_RGB)
				lcw.rVal = lcw.rValTarg > lcw.rVal?lcw.rVal + deltaR:lcw.rVal - deltaR;
				lcw.gVal = lcw.gValTarg > lcw.gVal?lcw.gVal + deltaG:lcw.gVal - deltaG;
				lcw.bVal = lcw.bValTarg > lcw.bVal?lcw.bVal + deltaB:lcw.bVal - deltaB;
#endif
			}
			else{
				if(--lcw.breath->rCnt == 0){
					_sts = 0;
				}
				else{
					ucPeriodCnt = 0;
					lcw.ln = lcw.lnTarget;
					lcw.tp = lcw.tpTarget;
#if (LIGHT_CW_WITH_RGB)
					lcw.lnRgb = lcw.lnRgbTarget;
					lcw.rVal = lcw.rValTarg;
					lcw.gVal = lcw.gValTarg;
					lcw.bVal = lcw.bValTarg;
#endif
				}
			}
		}break;
		case BREATH_MODE_H2L2H:{
			if(ucPeriodCnt < lcw.breath->pCnt){
				lcw.ln -= deltaLn;
				lcw.tp -= deltaTp;
#if (LIGHT_CW_WITH_RGB)
				lcw.lnRgb -= deltaLnRgb;
				lcw.rVal -= deltaR;
				lcw.gVal -= deltaG;
				lcw.bVal -= deltaB;
#endif
			}
			else if(ucPeriodCnt < (lcw.breath->pCnt<<1)){
				
				lcw.ln += deltaLn;
				lcw.tp += deltaTp;
#if (LIGHT_CW_WITH_RGB)
				lcw.lnRgb += deltaLnRgb;
				lcw.rVal += deltaR;
				lcw.gVal += deltaG;
				lcw.bVal += deltaB;
#endif
			}
			else{
				if(--lcw.breath->rCnt == 0){
					_sts = 0;
				}
				else{
					ucPeriodCnt = 0;
					lcw.ln -= deltaLn;
					lcw.tp -= deltaTp;
#if (LIGHT_CW_WITH_RGB)
					lcw.lnRgb -= deltaLnRgb;
					lcw.rVal -= deltaR;
					lcw.gVal -= deltaG;
					lcw.bVal -= deltaB;
#endif
				}
			}
		}break;
		default:{
			_sts = 2;
		}break;
	}

	if(_sts == 0){
		lcw.on = lcw.onTarget;
		lcw.ln = lcw.lnTarget;
		lcw.tp = lcw.tpTarget;
#if (LIGHT_CW_WITH_RGB)
		lcw.onRgb = lcw.onRgbTarget;
		lcw.lnRgb = lcw.lnRgbTarget;
		lcw.rVal = lcw.rValTarg;
		lcw.gVal = lcw.gValTarg;
		lcw.bVal = lcw.bValTarg;
#endif
		lcw.breath = NULL;
		deltaTp = 0;
		deltaLn = 0;
#if (LIGHT_CW_WITH_RGB)
		deltaR = 0;
		deltaG = 0;
		deltaB = 0;
		deltaLnRgb = 0;
#endif
	}
	else{
		ucPeriodCnt++;
	}
	_cw_ctrl(&lcw);
	return _sts;
}

