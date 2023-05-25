// Could be defined here, or in the processor defines.
#define SYSTEM_CORE_CLOCK 48000000
#define APB_CLOCK SYSTEM_CORE_CLOCK
#include "ch32v003fun.h"
#include <stdio.h>
#include "pt.h"

/* This example shows 3 protothreads running,
 * printing characters at different intervals.
 *
 * Connect your serial terminal RX to PD5 (CH32V003's UART TX).
 * Serial: 115200 8 N 1
 */

#define ONE_INTVL DELAY_MS_TIME*234
uint32_t oneStartTick;
static int 
oneThrd(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1) {
		oneStartTick=SysTick->CNT;
		printf("(1)");
		PT_WAIT_UNTIL(pt, SysTick->CNT - oneStartTick >= ONE_INTVL);
	}
	PT_END(pt);
}


#define TWO_INTVL DELAY_MS_TIME*123
uint32_t twoStartTick;
static int 
twoThrd(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1) {
		twoStartTick=SysTick->CNT;
		printf("<2>");
		PT_WAIT_UNTIL(pt, SysTick->CNT - twoStartTick >= TWO_INTVL);
	}
	PT_END(pt);
}

#define THREE_INTVL DELAY_MS_TIME*456
uint32_t threeStartTick;
static int 
threeThrd(struct pt *pt)
{
	PT_BEGIN(pt);
	while(1) {
		threeStartTick=SysTick->CNT;
		printf("{3}");
		PT_WAIT_UNTIL(pt, SysTick->CNT - threeStartTick >= THREE_INTVL);
	}
	PT_END(pt);
}

static struct pt pt1,pt2,pt3;

int main()
{
	SystemInit48HSI();
	SetupUART( UART_BRR );

	PT_INIT(&pt1);
	PT_INIT(&pt2);
	PT_INIT(&pt3);

	while(1)
	{
		oneThrd(&pt1);
		twoThrd(&pt2);
		threeThrd(&pt3);
	}
}
