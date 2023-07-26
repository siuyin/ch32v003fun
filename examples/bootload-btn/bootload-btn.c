/* bootload-btn.c is an example program running in the BOOT (0x1FFF F000) area.
 * Holding down the button connected to PD4, then pressing the reset button
 * or power-cycling the device will cause the user code in FLASH (0x0800 0000) to run.
 *
 * Please use the blink-btn example for the user program in FLASH.
 * This has PD4 configured as an input with weak pull-up so the compatible
 * with this BOOT program's GPIO assignment.
 *
 * Power-cycling the device will cause the BOOT program to run.
 * Pressing reset in BOOT will restart the BOOT program.
 * Pressing reset after the user program runs restarts the user program.
 */

#include "ch32v003fun.h"
#include <stdio.h>

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

void bootUserCode(int n)
{
	RCC->RSTSCKR |= RCC_SFTRSTF;
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	FLASH->BOOT_MODEKEYR = FLASH_KEY1;
	FLASH->BOOT_MODEKEYR = FLASH_KEY2;

	if( n )
	{
		FLASH->STATR &= ~(1<<14); // 1<<14 is zero, so, boot user code.
		FLASH->CTLR = CR_LOCK_Set;
		PFIC->SCTLR = 1<<31;	// software reset
		return;
	}

	FLASH->STATR |= 1<<14;
	FLASH->CTLR = CR_LOCK_Set;
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
