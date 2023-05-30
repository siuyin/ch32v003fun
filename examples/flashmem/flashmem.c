/* flashmem.c demonstrates writing, reading and erasing
   the part's flash memory.  

   NOTE: If flash is programmed with the "standard"/compatible procedure,
   It must be erased with standard mode as well.
   I tried mixing standard programming with fast erase modes
   and that did not work.
 */

#include "ch32v003fun.h"
#include <stdio.h>

#define SYSTEM_CORE_CLOCK 48000000
#define APB_CLOCK SYSTEM_CORE_CLOCK

#define stdFlashStart ((uint16_t *)0x08003C00)
void stdShow() {
	printf("%x\n",*(stdFlashStart+1));
	printf("%x\n",*stdFlashStart);
}

void unlockFlash() {
	while ((FLASH->CTLR & FLASH_CTLR_LOCK) != 0) {
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

void erase1KBlock(uint32_t startAddr) {
	// standard 1K block erase
	unlockFlash();
	while (FLASH->STATR & FLASH_STATR_BSY); 

	FLASH->CTLR |= FLASH_CTLR_PER;
	FLASH->ADDR = startAddr;
	FLASH->CTLR |= FLASH_CTLR_STRT;
	while (FLASH->STATR & FLASH_STATR_BSY); 

	if (FLASH->STATR & FLASH_STATR_WRPRTERR) {
		printf("error writing flash\n");
	}
}

void progFlash(uint16_t* addr, uint16_t val) {
	unlockFlash();
	FLASH->CTLR |= FLASH_CTLR_PG;
	*addr = val;
	while (FLASH->STATR & FLASH_STATR_BSY); 
	if (FLASH->STATR & FLASH_STATR_WRPRTERR) {
		printf("error writing flash\n");
	}
}

void stdProgDemo() {
	printf("--- reading standard programming flash area\n");
	stdShow();

	printf("  erasing flash\n");
	erase1KBlock((uint32_t)stdFlashStart);
	stdShow();

	printf("  programming flash\n");
	progFlash(stdFlashStart, 0xbeef);
	progFlash(stdFlashStart+1, 0xdead);
	stdShow();
}

int main() {
	SystemInit48HSI();
	SetupUART( UART_BRR );

	stdProgDemo();
	
	printf("# end\n");

	while(1);
}
