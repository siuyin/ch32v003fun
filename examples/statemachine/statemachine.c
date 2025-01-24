#include "ch32v003fun.h"
#include <stdio.h>
#include <stdbool.h>

#define BTN PD2
#define LED PD0

// bool btn_pushed() {
// 	if (funDigitalRead(BTN) == 0) return true;
// 	return false;
// }

#define btn_pushed() ( funDigitalRead(BTN) == 0 ? 1 : 0 )

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

void double_blink_led() {
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(125);
	funDigitalWrite(LED,FUN_LOW);
	Delay_Ms(125);
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(125);
	funDigitalWrite(LED,FUN_LOW);
	Delay_Ms(10);
}

void long_blink_led() {
	funDigitalWrite(LED,FUN_HIGH);
	Delay_Ms(500);
	funDigitalWrite(LED,FUN_LOW);
	Delay_Ms(10);
}

typedef enum {
	DCSReleased,
	DCSCandPushed,
	DCSPushed,
	DCSCandLongClicked,
	DCSLongClickedTimer,
	DCSLongClicked,
	DCSCandFirstClicked,
	DCSFirstClicked,
	DCSClicked,
	DCSCandDblClicked,
	DCSDblClicked,
	DCSCandReleased
} DblClkState_Typedef;

uint32_t long_click_max_interval = Ticks_from_Ms(500);
uint32_t double_click_max_interval = Ticks_from_Ms(150); // tune this value to properly detect a double-click
volatile uint32_t interval_timer_start_tick;
DblClkState_Typedef dblclick_state = DCSReleased;
// statemachine markdown:
// [![](https://mermaid.ink/img/pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A?type=png)](https://mermaid.live/edit#pako:eNqVVFtvgjAU_ivNeVyEVcDByOaLbk8uWbY9TXyoUIWIYEpZ5oz_faVehrQy9clz-C7nlm4gzCMKPhSccDpMyJyRpfFlBRkSv_HNBBlGH73RlJKCRrvsIZKfBiSLzNeyiEXso_GUZ2glo4kGfAz2SLaP99i6VgVvka2hJGuUZ_NBmoQLBY9ukeiNccSTJWU1ozql0qnFHxVU66tlyuRzwgreKKHRn-LQsK1oskrUR6lIo7DKIx4zWphVIzI5TZNsgUZPwxbRZplH4YdHRVktTleXMgntEA4Lvmy_J5S2m7jq4lTGPxbNii7YpIprZav1KdxWU90N692H0zRsHpPYebRPH27pnIR6h3rmGZLa67kDaZtrrYeKd9qSfpwNygUbvM6kgW5vBTogZrckSSTe1U0lEACP6ZIG4Iu_EZ2RMuUBBNlWQEnJ8_d1FoLPWUk7wPJyHoM_I2khonIV_b3LB8iKZJ95fgxplPCcvezecfmcSwj4G_gGv4tN2_Ww5biO5d7Zdq_bgTX4hm3bJsYW7t1h13Nw19524EeqWqbjuJ6HXce5tx3sWdtf5aP94A)
void indicate_led_on_single_and_double_click(){
	if (btn_next_check_tick>SysTick->CNT) {
		return;
	}

	printf("dblclick state=%d\n",dblclick_state);
	switch (dblclick_state) {
		case DCSReleased:
			if (btn_pushed()){
				dblclick_state = DCSCandPushed;
				break;
			}
			dblclick_state = DCSReleased;
			break;
		case DCSCandPushed:
			if (btn_pushed()){
				dblclick_state = DCSPushed;
				break;
			}
			dblclick_state = DCSCandReleased;
			break;
		case DCSPushed:
			if (btn_pushed()){
				dblclick_state = DCSCandLongClicked;
				interval_timer_start_tick = SysTick->CNT;
				break;
			}
			dblclick_state = DCSCandFirstClicked;
			break;
		case DCSCandLongClicked:
			if (btn_pushed()){
				dblclick_state = DCSLongClickedTimer;
				break;
			}
			dblclick_state = DCSCandFirstClicked;
			break;
		case DCSLongClickedTimer:
			if (SysTick->CNT - interval_timer_start_tick > long_click_max_interval) {
				dblclick_state = DCSLongClicked;
				long_blink_led();
				break;
			}
			dblclick_state = DCSCandLongClicked;
			break;
		case DCSLongClicked:
			if (btn_pushed()) {
				dblclick_state = DCSLongClicked;
				break;
			}
			dblclick_state = DCSCandReleased;
			break;
		case DCSCandFirstClicked:
			if (btn_pushed()){
				dblclick_state = DCSCandFirstClicked;
				break;
			}
			dblclick_state = DCSFirstClicked;
			interval_timer_start_tick = SysTick->CNT;
			break;
		case DCSFirstClicked:
			if (SysTick->CNT - interval_timer_start_tick <= double_click_max_interval) {
				dblclick_state = DCSCandDblClicked;
				break;
			}
			dblclick_state = DCSClicked;
			blink_led();
			break;
		case DCSCandDblClicked:
			if (btn_pushed()){
				dblclick_state = DCSDblClicked;
				double_blink_led();
				break;
			}
			dblclick_state = DCSFirstClicked;
			break;
		case DCSClicked:
			if (btn_pushed()){
				dblclick_state = DCSClicked;
				break;
			}
			dblclick_state = DCSCandReleased;
			break;
		case DCSDblClicked:
			if (btn_pushed()){
				dblclick_state = DCSDblClicked;
				break;
			}
			dblclick_state = DCSCandReleased;
			break;
		case DCSCandReleased:
			if (btn_pushed()){
				dblclick_state = DCSCandPushed;
				break;
			}
			dblclick_state = DCSReleased;
			break;
	}

	btn_next_check_tick = SysTick->CNT + btn_check_interval;
}

typedef enum {
	LCSReleased,
	LCSCandPushed,
	LCSPushed,
	LCSCandLongClicked,
	LCSLongClicked,
	LCSClicked,
	LCSCandReleased
} LongClickState;

LongClickState lc_state = LCSReleased;
// state diagram markdown:
// [![](https://mermaid.ink/img/pako:eNqdk09TgzAQxb9KZo8OIElopDn0Uo864-jN0kOEtDBC0gnBsXb63U3pH1uk0yonXvjt27dkdgWpziRwqK2w8r4QcyMq_4MkCrlncjNFvj9Cz7KUopbZ9nSv2k9jobLgqalzpzmavFmFFq2a9sAHsSPNTu_YY68N3muLbpHLaiyyRSVNf2Grr2n253S_Z-9WnUzfzfSg1XxcFun7Ffwp1hfm2O2_Jdek6q08W4Qcvb0cNEKlo1C6wZDNjaxzXR5czyc_7d6Ne-GeOkEvDdf7R862AA_cYJUoMrczq41DAjaXlUyAu9dMzkRT2gQStXaoaKx-WaoUuDWN9MDoZp4Dn4mydqpZZD87t0cWQr1qfZAyK6w2j9sdbVe1RYCv4BO4j4csYCGOKI4xCQfEg6U7JSQO7gY4ZIxQEjMakbUHX60rDiJCGYtDwoYkwpTS9TdmL0hz?type=png)](https://mermaid.live/edit#pako:eNqdk09TgzAQxb9KZo8OIElopDn0Uo864-jN0kOEtDBC0gnBsXb63U3pH1uk0yonXvjt27dkdgWpziRwqK2w8r4QcyMq_4MkCrlncjNFvj9Cz7KUopbZ9nSv2k9jobLgqalzpzmavFmFFq2a9sAHsSPNTu_YY68N3muLbpHLaiyyRSVNf2Grr2n253S_Z-9WnUzfzfSg1XxcFun7Ffwp1hfm2O2_Jdek6q08W4Qcvb0cNEKlo1C6wZDNjaxzXR5czyc_7d6Ne-GeOkEvDdf7R862AA_cYJUoMrczq41DAjaXlUyAu9dMzkRT2gQStXaoaKx-WaoUuDWN9MDoZp4Dn4mydqpZZD87t0cWQr1qfZAyK6w2j9sdbVe1RYCv4BO4j4csYCGOKI4xCQfEg6U7JSQO7gY4ZIxQEjMakbUHX60rDiJCGYtDwoYkwpTS9TdmL0hz)
void flash_led_on_single_and_long_click() {
	if (btn_next_check_tick>SysTick->CNT) {
		return;
	}

	printf("state=%d pushed=%d\n",lc_state,btn_pushed());
	switch(lc_state) {
		case LCSReleased:
			if (btn_pushed()) {
				lc_state = LCSCandPushed;
				break;
			}
			lc_state = LCSReleased;
			break;
		case LCSCandPushed:
			if (btn_pushed()) {
				lc_state = LCSPushed;
				interval_timer_start_tick = SysTick->CNT;
				break;
			}
			lc_state = LCSCandReleased;
			break;
		case LCSPushed:
			if (btn_pushed()) {
				lc_state = LCSCandLongClicked;
				break;
			}
			lc_state = LCSClicked;
			blink_led();
			break;
		case LCSCandLongClicked:
			if (btn_pushed()) {
				if (SysTick->CNT - interval_timer_start_tick > long_click_max_interval) {
					lc_state = LCSLongClicked;
					double_blink_led();
					break;
				}
				lc_state = LCSCandLongClicked;
				break;
			}
			lc_state = LCSClicked;
			blink_led();
			break;
		case LCSLongClicked:
			if (btn_pushed()) {
				lc_state = LCSLongClicked;
				break;
			}
			lc_state = LCSCandReleased;
			break;
		case LCSClicked:
			if (btn_pushed()) {
				lc_state = LCSClicked;
				break;
			}
			lc_state = LCSCandReleased;
			break;
		case LCSCandReleased:
			if (btn_pushed()) {
				lc_state = LCSCandReleased;
				break;
			}
			lc_state = LCSReleased;
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
		//flash_led_on_single_and_long_click();
	}
}
