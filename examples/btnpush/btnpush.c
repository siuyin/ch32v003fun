// Could be defined here, or in the processor defines.
#define SYSTEM_CORE_CLOCK 48000000

#include "ch32v003fun.h"
#include <stdio.h>
#include <stdbool.h>

void initGPIOD0PushPullD4InputPullDown() {
	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
	
	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);
	
	// GPIO D4 Input Pull-up
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= GPIO_CNF_IN_PUPD<<(4*4);
	GPIOD->OUTDR |= 1<<4; // PD4 pull-up
}

static bool ledLit;
void ledOn() {
	GPIOD->BSHR = 1<<0;
	ledLit=true;
}

void ledOff() {
	GPIOD->BCR = 1;
	ledLit=false;
}

enum btnState {
	maybePushed,
	pushed,
	maybeReleased,
	released,
};

bool btnPushed(uint8_t n) {
	if ((GPIOD->INDR&(1<<n)) != 0) return false;

	// debounce mechanical push button
	enum btnState state = maybePushed;
	while (1) {
		Delay_Ms(20);
		switch(state)
		{
		case maybePushed:
			if ((GPIOD->INDR&(1<<n)) == 0) state=pushed;
			else state=maybeReleased;
			break;
		case pushed:
			if ((GPIOD->INDR&(1<<n)) == 0) state=pushed;
			else state=maybeReleased;
			break;
		case maybeReleased:
			if ((GPIOD->INDR&(1<<n)) == 0) state=pushed;
			else state=released;
			break;
		case released:
			return true;
		}
	}
}

void ledToggle() {
	if (ledLit) ledOff();
	else ledOn();
}

int main()
{
	SystemInit48HSI();

	initGPIOD0PushPullD4InputPullDown();

	while(1)
	{
		if (btnPushed(4)) { // PD4
			ledToggle();
		}
	}
}
