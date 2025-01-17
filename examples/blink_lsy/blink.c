#include "ch32v003fun.h"
#include <stdio.h>

// use defines to make more meaningful names for our GPIO pins
#define PIN_1 PD0

int main()
{
	SystemInit();

	// Enable GPIOs
	funGpioInitAll();

	funPinMode( PIN_1, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while ( 1 )
	{
		funDigitalWrite( PIN_1, FUN_HIGH );
		Delay_Ms( 250 );
		funDigitalWrite( PIN_1, FUN_LOW );
		Delay_Ms( 250 );
	}
}
