// Demonstrates remapping of USART1 TX (PD5) to PD6.
// Based on examples/usartdemo.c .


#include "ch32v003fun.h"
#include <stdio.h>

uint32_t count;

int main()
{
	SystemInit();

	// Enable GPIOD.
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;

	// GPIO D0 Push-Pull, 10MHz Output
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	// make PD6 TX instead of default PD5
	GPIOD->CFGLR &= ~(0xf<<4*6);
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6);
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
	AFIO->PCFR1 |= 1<<21;	// USART1_RM1

	while(1)
	{
		GPIOD->BSHR = 1;	 // Turn on GPIOD0
		Delay_Ms( 50 );
		GPIOD->BSHR = 1<<16; // Turn off GPIOD0
		Delay_Ms( 50 );
		printf( "Count: %lu\n", count++ );
	}
}
