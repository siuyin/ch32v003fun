/* uartgpio.c demonstrates the use of the built-in UART
   to control GPIO.
  
   Connect an LED to PD0,
   push button PD5 to Gnd,
   PD5 (TX) -> your terminal RX,
   PD6 (RX) -> your terminal TX.
  
   Communications settings: 115200 8 N 1
  
   This code uses polling to check if a character has
   been received.
 */

#define SYSTEM_CORE_CLOCK 48000000
#define APB_CLOCK SYSTEM_CORE_CLOCK

#include "ch32v003fun.h"
#include <stdio.h>

void initUART() {
	SetupUART( UART_BRR );
	USART1->CTLR1 |= USART_CTLR1_RE; // Rx enable
}

void initGPIOD0PushPullD4InputPullUp() {
	// Enable GPIOD.
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;

	// GPIO D0 Push-Pull, 10MHz Output
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	// GPIO D4 Input Pull-up
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= GPIO_CNF_IN_PUPD<<(4*4);
	GPIOD->OUTDR |= 1<<4; // PD4 pull-up
}

int main()
{
	SystemInit48HSI();
	initUART();
	initGPIOD0PushPullD4InputPullUp();


	printf("UART is now controlling GPIO. Type h for help.\n");
	uint8_t ch;
	#define CHAR_RECVD ((USART1->STATR & USART_STATR_RXNE) != 0)
	#define LED_ON (GPIOD->BSHR = 1)
	#define LED_OFF (GPIOD->BSHR = 1<<16)
	#define BTN_PUSHED ((GPIOD->INDR&(1<<4)) == 0)
	while(1)
	{
		if (CHAR_RECVD) {
			ch = USART1->DATAR;
			printf("%c: ",ch);
			switch (ch)
			{
			case '1':
				LED_ON;
				printf("turning on LED\n");
				break;
			case '0':
				LED_OFF;
				printf("turning off LED\n");
				break;
			case '?':
			case 'h':
				printf("available commands: 1 = LED on, 0 = LED off\nPressing the push-button sends \"Button Pushed\" message.\n");
				break;
			default:
				printf("unknown command\n");
				break;
			}
		}

		if (BTN_PUSHED) {
			printf("Button Pushed\n");
		}
	}
}