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

void blink_led(){
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(50);
	funDigitalWrite(LED,FUN_LOW);
}

void double_blink_led(){
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(50);
	funDigitalWrite(LED,FUN_LOW);
	Delay_Ms(150);
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(50);
	funDigitalWrite(LED,FUN_LOW);
	Delay_Ms(50);
}

typedef enum {
	BtnDblClickStateReleased,
	BtnDblClickStateCandPushed,
	BtnDblClickStatePushed,
	BtnDblClickStateCandLongClicked,
	BtnDblClickStateLongClicked,
	BtnDblClickStateCandReleasedForSingleOrDoubleClick,
	BtnDblClickStateReleasedForSingleOrDoubleClick,
	BtnDblClickStateSingleClicked,
	BtnDblClickStateDoubleClicked,
	BtnDblClickStateCandReleased,
} BtnDblClkState_Typedef;

uint32_t long_click_max_interval = Ticks_from_Ms(750);
uint32_t double_click_max_interval = Ticks_from_Ms(350); // tune this value to properly detect a double-click
volatile uint32_t interval_timer_start_tick;
BtnDblClkState_Typedef dblclick_state = BtnDblClickStateReleased;
// statemachine markdown:
// [![](https://mermaid.ink/img/pako:eNqVlE1P4zAQhv_KyMdVGrVuUqfRCg4gTnys4LTQPZh4mkQ4duU4CLbqf8dxICTQLmxO8fiZ1-94NN6STAskKaktt3ha8tzwavJIVwrcd_fjD0wmR3CNEnmNoosqbREkri3odb-VworcN9ZqBZumLiArMHtAAaUC5FkBXn9FOoW3JC9-wpUIf7kct07hd0cMg0MHjrjcT3S_TiBozzIWbFmh6dgB5tPOtcpPZNka7E8cVfWZGtTmlNx2pw9HIB0GWcuBLQzWhZbiGDAPgcXTqh64HQh6L_tsfGR84vslX_5DboSeaXNTqlzilTnVzb1ETzqBAJzHjxf0jdRRGw4gvb9vSA3Wh7rwVS2HevIThMf2dQXm074pIwufr_C_yunifTmviaPoV_0cTcUAIQFxVVW8FG5Ot23CitgCKzdO7dQJXPNG2na2dg7ljdU3zyojqTUNBsToJi9IuuaydqtmI97n_A3ZcHWrdb9EUVptLrp3wT8PHiHpljyRlLIwmUUsShbLKaURjQPyTNLJPIlDxpZJvFjM2GLKZvEuIH-9Kg0ZXUYJjWZ0Pk9ozHYvDyJztw?type=png)](https://mermaid.live/edit#pako:eNqVlE1P4zAQhv_KyMdVGrVuUqfRCg4gTnys4LTQPZh4mkQ4duU4CLbqf8dxICTQLmxO8fiZ1-94NN6STAskKaktt3ha8tzwavJIVwrcd_fjD0wmR3CNEnmNoosqbREkri3odb-VworcN9ZqBZumLiArMHtAAaUC5FkBXn9FOoW3JC9-wpUIf7kct07hd0cMg0MHjrjcT3S_TiBozzIWbFmh6dgB5tPOtcpPZNka7E8cVfWZGtTmlNx2pw9HIB0GWcuBLQzWhZbiGDAPgcXTqh64HQh6L_tsfGR84vslX_5DboSeaXNTqlzilTnVzb1ETzqBAJzHjxf0jdRRGw4gvb9vSA3Wh7rwVS2HevIThMf2dQXm074pIwufr_C_yunifTmviaPoV_0cTcUAIQFxVVW8FG5Ot23CitgCKzdO7dQJXPNG2na2dg7ljdU3zyojqTUNBsToJi9IuuaydqtmI97n_A3ZcHWrdb9EUVptLrp3wT8PHiHpljyRlLIwmUUsShbLKaURjQPyTNLJPIlDxpZJvFjM2GLKZvEuIH-9Kg0ZXUYJjWZ0Pk9ozHYvDyJztw)
void indicate_led_on_single_and_double_click(){
	if (btn_next_check_tick>SysTick->CNT) {
		return;
	}

	printf("dblclick state=%d\n",dblclick_state);
	switch (dblclick_state) {
		case BtnDblClickStateReleased:
			if (funDigitalRead(BTN) == 0) {
				dblclick_state = BtnDblClickStateCandPushed;
			}
			break;
		case BtnDblClickStateCandPushed:
			if (funDigitalRead(BTN) == 0) {
				dblclick_state = BtnDblClickStatePushed;
				interval_timer_start_tick = SysTick->CNT;
			}

			break;
		case BtnDblClickStatePushed:
			if (funDigitalRead(BTN) == 0) {
				dblclick_state = BtnDblClickStateCandLongClicked;
			}
			break;
		case BtnDblClickStateCandLongClicked:
			if (funDigitalRead(BTN) == 0){
				if (SysTick->CNT - interval_timer_start_tick > long_click_max_interval) {
					dblclick_state = BtnDblClickStateLongClicked;
					double_blink_led();
					blink_led();
					break;
				}
				break;
			}
			interval_timer_start_tick = SysTick->CNT;
			dblclick_state = BtnDblClickStateCandReleasedForSingleOrDoubleClick;
			break;
		case BtnDblClickStateLongClicked:
			if (funDigitalRead(BTN) == 1) {
				dblclick_state = BtnDblClickStateCandReleased;
			}
			break;
		case BtnDblClickStateCandReleasedForSingleOrDoubleClick:
			if (funDigitalRead(BTN) == 1) {
				dblclick_state = BtnDblClickStateReleasedForSingleOrDoubleClick;
			}
			break;
		case BtnDblClickStateReleasedForSingleOrDoubleClick:
			while (SysTick->CNT - interval_timer_start_tick < double_click_max_interval) {
				if (funDigitalRead(BTN) == 0) {
					dblclick_state = BtnDblClickStateDoubleClicked;
					double_blink_led();
					goto end;
				}
			}

			if (funDigitalRead(BTN) == 0) {
				if (SysTick->CNT - interval_timer_start_tick > double_click_max_interval) {
				}
				dblclick_state = BtnDblClickStateSingleClicked;
				blink_led();
				break;
			}
			dblclick_state = BtnDblClickStateCandReleased;
end:
			break;
		case BtnDblClickStateSingleClicked:
			if (funDigitalRead(BTN) == 1) {
				dblclick_state = BtnDblClickStateCandReleased;
			}
			break;
		case BtnDblClickStateDoubleClicked:
			if (funDigitalRead(BTN) == 1) {
				dblclick_state = BtnDblClickStateCandReleased;
			}
			break;
		case BtnDblClickStateCandReleased:
			if (funDigitalRead(BTN) == 1) {
				dblclick_state = BtnDblClickStateReleased;
			}
			break;
	}
	btn_next_check_tick = SysTick->CNT + btn_check_interval;
}


int main() {
	SystemInit();

	// Enable GPIOs
	funGpioInitAll();

	funPinMode(BTN, GPIO_CNF_IN_PUPD);	
	funDigitalWrite(BTN, FUN_HIGH ); // pull-up

	funPinMode(LED, GPIO_Speed_10MHz|GPIO_CNF_OUT_PP);	

	printf("Entering main loop\n");
	btn_next_check_tick = SysTick->CNT + btn_check_interval;
	while(1) {
		//toggle_button();
		indicate_led_on_single_and_double_click();
	}
}
