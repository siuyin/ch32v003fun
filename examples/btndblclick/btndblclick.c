// Could be defined here, or in the processor defines.
// NOTE: if the value is below 8MHz, the LinkE programmer
// sucessfully programs the part the *first* time.
// But will fail subsequently. This is because LinkE expects
// a timely response from the part which it cannot do because
// it now runs more slowly than expected.
//
// Fix with ../../minichlink/minichlink -u to erase and "unbrick"
// the part.
//
//#define SYSTEM_CORE_CLOCK 93750
#define SYSTEM_CORE_CLOCK 8000000
//#define SYSTEM_CORE_CLOCK 24000000

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

void  ledLongFlash() {
	ledOn();
	Delay_Ms(200);
	ledOff();
}

void ledDoubleFlash() {
	for (uint8_t i=0;i<2;i++) {
		ledOn();
		Delay_Ms(80);
		ledOff();
		Delay_Ms(80);
	}
}

void ledToggle() {
	if (ledLit) ledOff();
	else ledOn();
}


enum btnstate {
	released,
	maybeClicked,
	clicked,
	armedForDoubleClick,
	maybeReleasedFromClick,
	doubleClicked,
	maybeReleasedFromDoubleClick
};

enum btnstate state = released;
uint32_t startTime;

#define BTN_PUSHED ((GPIOD->INDR&(1<<4)) == 0)
#define DBLCLICK_INTVL (DELAY_MS_TIME * 200)
int main()
{
	// If there is no clock initialisation, the HCLK will be 8HMz (24/3 MHz) after reset.
	//SystemInit48HSI();
	//RCC->CFGR0 = RCC_HPRE_DIV256;
	//FLASH->ACTLR = FLASH_ACTLR_LATENCY_0;

	initGPIOD0PushPullD4InputPullDown();

	while(1)
	{
		switch (state) 
		{
		case released:
			if (BTN_PUSHED) state = maybeClicked;
			else state = released;
			break;
		case maybeClicked:
			if (BTN_PUSHED) {
				state = clicked;
			} else state = maybeReleasedFromClick;
			break;
		case clicked:
			if (BTN_PUSHED) state = clicked;
			else {
				state = armedForDoubleClick;
				startTime = SysTick->CNT;
			}
			break;
		case armedForDoubleClick:
			while (SysTick->CNT - startTime <= DBLCLICK_INTVL) {
				if (BTN_PUSHED) {
					ledDoubleFlash();
					state = doubleClicked;
					goto END_1;
				}
			}
			state = released;
			ledLongFlash();
			END_1:
			break;
		case maybeReleasedFromClick:
			if (BTN_PUSHED) state = maybeClicked;
			else state = released;
			break;
		case doubleClicked:
			if (BTN_PUSHED) state = doubleClicked;
			else state = maybeReleasedFromDoubleClick;
			break;
		case maybeReleasedFromDoubleClick:
			if (BTN_PUSHED) state = doubleClicked;
			else state = released;
			break;
		}
		Delay_Ms(20);
	}
}
