/* flashmem.c demonstrates writing, reading and erasing
   the part's flash memory.  
   
   Connect PD5 (UART Tx) to your serial terminal's Rx. 115200 N 8 1

   NOTE: If flash is programmed with the "standard"/compatible procedure,
   It must be erased with standard mode as well.
   I tried mixing standard programming with fast erase modes
   and that did not work.

   UPDATE: The above statement is incorrect.
   For fast-mode code I have been using FLASH_CTLR_PAGE_PG (incorrect uint16_t)
   instead of CR_PAGE_PG (correct uint32_t) !
   This teaches me to pay attention to object sizes !

   UPDATE: You *can* combine standard flash writes with fast 64Byte erases.
 */

#include "ch32v003fun.h"
#include <stdio.h>

#define SYSTEM_CORE_CLOCK 48000000
#define APB_CLOCK SYSTEM_CORE_CLOCK

#define stdFlashStart ((uint16_t *)0x08003C00)
#define fastFlashStart ((uint32_t)((uint32_t)stdFlashStart - 64))



void fastUnlock() {
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;

	FLASH->MODEKEYR = FLASH_KEY1;
	FLASH->MODEKEYR = FLASH_KEY2;
}

void fastLock() {
	FLASH->CTLR |= CR_LOCK_Set;
}

void fastBufRst() {
	FLASH->CTLR |= CR_PAGE_PG;
	FLASH->CTLR |= CR_BUF_RST;
	while(FLASH->STATR & SR_BSY);
	FLASH->CTLR &= ~CR_PAGE_PG;
}

void fastBufLoad(uint32_t addr, uint32_t dat) {
	FLASH->CTLR |= CR_PAGE_PG;
	*(__IO uint32_t *)(addr) = dat;
	FLASH->CTLR |= CR_BUF_LOAD;
	while(FLASH->STATR & SR_BSY);
	FLASH->CTLR &= ~CR_PAGE_PG;
}

void fastErasePage(uint32_t addr) {
	FLASH->CTLR |= CR_PAGE_ER;
	FLASH->ADDR = addr;
	FLASH->CTLR |= CR_STRT_Set;
	while(FLASH->STATR & SR_BSY);
	FLASH->CTLR &= ~CR_PAGE_ER;
}

void fastProgPage(uint32_t addr) {
	FLASH->CTLR |= CR_PAGE_PG;
	FLASH->ADDR = addr;
	FLASH->CTLR |= CR_STRT_Set;
	while(FLASH->STATR & FLASH_STATR_BSY);
	FLASH->CTLR &= ~CR_PAGE_PG;
}

void fastShowMem(){
	for (uint8_t i=0;i<16;i++){
		printf("%lx",*((uint32_t *)(fastFlashStart)+i));
	}
	printf("\n");
}

void fastProgDemo() {
	printf("--- starting fast programming\n");
	uint32_t buf[16];

	uint8_t i, verify;
	for (i=0;i<16;i++) {
		buf[i]=i;
	}
	fastShowMem();

	fastUnlock();

	fastErasePage(fastFlashStart);
	printf("64 byte page erased\n");
	fastShowMem();


	fastBufRst();
	for(i=0; i<16;i++) {
		fastBufLoad(fastFlashStart+4*i,buf[i]);
	}

	fastProgPage(fastFlashStart);
	printf("64 byte page programmed\n");
	fastShowMem();

	//fastLock();

	for(i=0;i<16;i++) {
		if(buf[i] == *(uint32_t *)(fastFlashStart+4*i)) {
			verify = 0;
		} else {
			verify = 1;
			break;
		}
	}

	if(verify) {
		printf("verify failed\n");
	} else {
		printf("verified\n");
	}

}

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
	//erase1KBlock((uint32_t)stdFlashStart);
	fastUnlock();
	fastErasePage((uint32_t)stdFlashStart);
	stdShow();

	printf("  programming flash\n");
	progFlash(stdFlashStart, 0xbeef);
	progFlash(stdFlashStart+1, 0xdead);
	stdShow();
}

int main() {
	SystemInit48HSI();
	SetupUART( UART_BRR );
	Delay_Ms(200);

        stdProgDemo();
	fastProgDemo();

	printf("--- end\n");

	while(1);
}
