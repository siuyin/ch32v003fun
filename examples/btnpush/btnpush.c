// Could be defined here, or in the processor defines.
#define SYSTEM_CORE_CLOCK 48000000

#include "ch32v003fun.h"
#include <stdio.h>

void initGPIOD0PushPullD4InputPullDown() {
	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
	
	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);
	
	// GPIO D4 Input Pull-up
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= GPIO_CNF_IN_PUPD<<(4*4);
	GPIOD->OUTDR |= 1<<4; // PD0 pull-up
}

void ledOn() {
	GPIOD->BSHR = 1<<0;
}

void ledOff() {
	GPIOD->BCR = 1;
}

uint8_t btnPushed() {
	if ((GPIOD->INDR&(1<<4)) == 0) return 1;
	return 0;
}
int main()
{
	SystemInit48HSI();

	initGPIOD0PushPullD4InputPullDown();

	while(1)
	{
		if (btnPushed()) {
			ledOn();
			continue;
		}
		ledOff();
	}
}
