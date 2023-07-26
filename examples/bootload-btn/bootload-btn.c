#include "ch32v003fun.h"
#include <stdio.h>

uint32_t count;

// You can override the interrupt vector this way:
void InterruptVector()         __attribute__((naked)) __attribute((section(".init")));
void InterruptVector()
{
	asm volatile( "\n\
	.align  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.option pop");
}


void setupGPIO()
{
	// Enable GPIOD
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
	
	// GPIO D4 Input, Pull-up
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= GPIO_CNF_IN_PUPD<<(4*4);
	GPIOD->OUTDR |= (1<<4);

	// GPIO D0 Push-Pull, 10MHz Output
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);
}

void bootUserCode(int n)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	FLASH->BOOT_MODEKEYR = FLASH_KEY1;
	FLASH->BOOT_MODEKEYR = FLASH_KEY2;

	if( n )
	{
		FLASH->STATR = 0; // 1<<14 is zero, so, boot user code.
		FLASH->CTLR = CR_LOCK_Set;
		PFIC->SCTLR = 1<<31;	// software reset
		return;
	}

	FLASH->STATR |= 1<<14;
	FLASH->CTLR = CR_LOCK_Set;

}

int main()
{
	SystemInit();
	setupGPIO();

	bootUserCode( 0 );
	if( (GPIOD->INDR & (1<<4)) == 0 ) bootUserCode( 1 );

	// Make a clear blink signature.
	for( int i = 0; i < 5; i++ )
	{
		GPIOD->BSHR = 1;	// turn on PD0
		Delay_Ms( 50 );
		GPIOD->BSHR = (1<<16);	// turn off PD0
		Delay_Ms( 125 );
	}



	while(1);
}
