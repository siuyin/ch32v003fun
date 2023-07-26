#include "ch32v003fun.h"
#include <stdio.h>

int main()
{
	SystemInit();

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD; 

	// GPIO D4 Input, Pull-up
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= GPIO_CNF_IN_PUPD<<(4*4);
	GPIOD->OUTDR |= (1<<4);

	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	while(1)
	{
		GPIOD->BSHR = 1;	 // Turn on GPIOs
		Delay_Ms( 250 );
		GPIOD->BSHR = (1<<16); // Turn off GPIODs
		Delay_Ms( 250 );

		while( (GPIOD->INDR & 1<<4) == 0) GPIOD->BSHR = 1<<16;
	}
}
