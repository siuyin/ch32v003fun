#include "ch32v003fun.h"
#include <stdio.h>

int main()
{
	SystemInit();

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC;

	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	// GPIO D4 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4);

	// GPIO C0 Push-Pull
	GPIOC->CFGLR &= ~(0xf<<(4*0));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);
}

void toggleGPIOsMs(uint16_t dly) {
	GPIOD->BSHR = 1 | (1<<4);	 // Bit Set High Reset: Turn on GPIOs
	GPIOC->BSHR = 1;
	Delay_Ms( dly );
	GPIOD->BSHR = (1<<16) | (1<<(16+4)); // Turn off GPIODs
	GPIOC->BSHR = (1<<16);
	Delay_Ms( dly );
}


int main()
{
	SystemInit48HSI();

	initGPIOD0D4C0PushPull();

	while(1)
	{
		toggleGPIOsMs(100);
	}
}
