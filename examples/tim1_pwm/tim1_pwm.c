/*
 * Example for using Advanced Control Timer (TIM1) for PWM generation
 * 03-28-2023 E. Brombaugh
 * 05-29-2023 SiuYin Loh
 */

// Could be defined here, or in the processor defines.
#define SYSTEM_CORE_CLOCK 48000000
#define APB_CLOCK SYSTEM_CORE_CLOCK

#include "ch32v003fun.h"
#include <stdio.h>

/*
 * initialize TIM1 for PWM
 */
void t1pwm_init( void )
{
	// Enable GPIOC, GPIOD and TIM1
	RCC->APB2PCENR |= 	RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC |
						RCC_APB2Periph_TIM1;
	
	// PD0 is T1CH1N, 10MHz Output alt func, push-pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*0);
	
	// PC4 is T1CH4, 10MHz Output alt func, push-pull
	GPIOC->CFGLR &= ~(0xf<<(4*4));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*4);
		
	// Reset TIM1 to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1; // writing a 1 will hold TIM1 in reset
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;// release from reset
	
	// CTLR1: default is up, events generated, edge align
	// SMCFGR: default clk input is CK_INT
	
	// Prescaler 
	TIM1->PSC = 0x0000;
	//TIM1->PSC = 0x3FFF;
	
	TIM1->CTLR1 |= TIM_ARPE; // auto reload preload enable
	// Auto Reload - sets period
	//TIM1->ATRLR = 255;
	TIM1->ATRLR = 255;
	
	// Reload immediately
	TIM1->SWEVGR |= TIM_UG;
	
	// Enable CH1N output, positive pol
	TIM1->CCER |= TIM_CC1NE | TIM_CC1NP;
	//TIM1->CCER |= TIM_CC1NE;
	//TIM1->CCER &= ~TIM_CC1NP; // update to reset state. negative polarity
	
	// Enable CH4 output, positive pol
	//TIM1->CCER |= TIM_CC4E | TIM_CC4P;
	TIM1->CCER |= TIM_CC4E; 
	
	// CH1 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
	/* In PWM Mode 1, the timer ouput is not asserted when:
	   In up-counter mode,
	   counter is less than the compare value.
	 */
	TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1; // mode 1 PWM
	//TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1 | TIM_OC1M_0; // mode 2 PWM
	
	// CH4 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
	//TIM1->CHCTLR2 |= TIM_OC4M_2 | TIM_OC4M_1; // mode 1 PWM
	TIM1->CHCTLR2 |= TIM_OC4M_2 | TIM_OC4M_1 | TIM_OC4M_0; // mode 2 PWM
	
	// Set the Capture Compare Register value to 50% initially
	TIM1->CH1CVR = 128;
	TIM1->CH4CVR = 128;
	
	// Enable TIM1 outputs
	TIM1->BDTR |= TIM_MOE;
	
	// Enable TIM1
	TIM1->CTLR1 |= TIM_CEN;
}

/*
 * set timer channel PW
 */
void t1pwm_setpw(uint8_t chl, uint16_t width)
{
	switch(chl&3)
	{
		case 0: TIM1->CH1CVR = width; break;
		case 1: TIM1->CH2CVR = width; break;
		case 2: TIM1->CH3CVR = width; break;
		case 3: TIM1->CH4CVR = width; break;
	}
}

/*
 * force output (used for testing / debug)
 */
void t1pwm_force(uint8_t chl, uint8_t val)
{
	uint16_t temp;
	
	chl &= 3;
	
	if(chl < 2)
	{
		temp = TIM1->CHCTLR1;
		temp &= ~(TIM_OC1M<<(8*chl));
		temp |= (TIM_OC1M_2 | (val?TIM_OC1M_0:0))<<(8*chl);
		TIM1->CHCTLR1 = temp;
	}
	else
	{
		chl &= 1;
		temp = TIM1->CHCTLR2;
		temp &= ~(TIM_OC1M<<(8*chl));
		temp |= (TIM_OC1M_2 | (val?TIM_OC1M_0:0))<<(8*chl);
		TIM1->CHCTLR2 = temp;
	}
}

/*
 * entry
 */
int main()
{
	uint8_t count = 0;
	
	SystemInit48HSI();

	// start serial @ default 115200bps
	SetupUART( UART_BRR );
	Delay_Ms( 100 );
	printf("\r\r\n\ntim1_pwm example\n\r");

	// init TIM1 for PWM
	printf("initializing tim1...");
	t1pwm_init();
	printf("done.\n\r");
		
	printf("looping...\n\r");
	//const uint8_t pwmWidth[10]={0,1,2,5,10,20,50,100,200,255}; // approx Log10 scale
	const uint8_t pwmWidth[10]={0,77,122,154,178,198,216,230,243,255}; // closer Log10 scale
	while(1)
	{
		t1pwm_setpw(0, pwmWidth[count]); // Chl 1
		t1pwm_setpw(3, pwmWidth[10-count]); // Chl 4
		count++;
		if (count >= 10) count=0;
		Delay_Ms( 65 );
	}
}
