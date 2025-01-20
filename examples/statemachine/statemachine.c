#include "ch32v003fun.h"
#include <stdio.h>

#define BTN PD2
#define LED PD0

typedef enum {
	BtnStateReleased,
	BtnStateCandidatePushed,
	BtnStatePushed,
	BtnStateCandidateReleased
} BtnState_Typedef;

// state machine markdown: 
// [![](https://mermaid.ink/img/pako:eNpVkL0OwjAMhF_F8ohaBsYMLDAjBBNQBpO4pKJJqtZBQoh3J6X8lGw5f3e-5I46GEaFnZDwsqJzSy6_zgoP6RwmR8jzOWy4ZurYDKoPwlBzKRDK70hBgacoEjw0sbOgLesLG6g8MGkLr_wCh4SP6RW-IG-m6-RJdwW7gRiLX-i3azWi_sJGCGbouHVUmfS8e28oUCy71KIva7ikWEtf6ZFQihK2N69RSRs5w9iY34egKqnuktqQ34fgPlAb4tm-h48n-QRsCA?type=png)](https://mermaid.live/edit#pako:eNpVkL0OwjAMhF_F8ohaBsYMLDAjBBNQBpO4pKJJqtZBQoh3J6X8lGw5f3e-5I46GEaFnZDwsqJzSy6_zgoP6RwmR8jzOWy4ZurYDKoPwlBzKRDK70hBgacoEjw0sbOgLesLG6g8MGkLr_wCh4SP6RW-IG-m6-RJdwW7gRiLX-i3azWi_sJGCGbouHVUmfS8e28oUCy71KIva7ikWEtf6ZFQihK2N69RSRs5w9iY34egKqnuktqQ34fgPlAb4tm-h48n-QRsCA)
BtnState_Typedef state = BtnStateReleased;
const uint32_t btn_check_interval = Ticks_from_Ms(20); // tune this value to properly debounce the push-button switch
volatile uint32_t btn_next_check_tick;
void toggle_button(){
	if (btn_next_check_tick>SysTick->CNT) {
		return;
	}

	switch (state) {
		case BtnStateReleased:
			if (funDigitalRead(BTN) == 0) {
				state = BtnStateCandidatePushed;
			}
			break;
		case BtnStateCandidatePushed:
			if (funDigitalRead(BTN) == 0) {
				state = BtnStatePushed;
				funDigitalWrite(LED, !(funDigitalRead(LED)));
			}
			break;
		case BtnStatePushed:
			if (funDigitalRead(BTN) == 1) {
				state = BtnStateCandidateReleased;
			}
			break;
		case BtnStateCandidateReleased:
			if (funDigitalRead(BTN) == 1) {
				state = BtnStateReleased;
			}
			break;

	}
	btn_next_check_tick = SysTick->CNT + btn_check_interval;
}

int main()
{
	SystemInit();

	// Enable GPIOs
	funGpioInitAll();

	funPinMode(BTN, GPIO_CNF_IN_PUPD);	
	funDigitalWrite(BTN, FUN_HIGH ); // pull-up

	funPinMode(LED, GPIO_Speed_10MHz|GPIO_CNF_OUT_PP);	

	printf("Entering main loop\n");
	btn_next_check_tick = SysTick->CNT + btn_check_interval;
	while(1) {
		toggle_button();
	}
}
