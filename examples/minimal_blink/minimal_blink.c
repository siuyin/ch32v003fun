#define uint32_t unsigned int

#define MY_RCC_BASE 0x40021000 // Reset and Clock Control
#define RCC_APB2PCENR *(uint32_t *)(MY_RCC_BASE+0x18) // Advanced Peripheral Bus 2 Peripheral Clock Enable Register

#define MY_GPIOD_BASE 0x40011400
#define GPIOD_CFGLR *(uint32_t *)(MY_GPIOD_BASE+0x00) // Config register for GPIO Lower (bits 0 to 7) 
void initGPIO()
{
	RCC_APB2PCENR |= (1<<5); // Send clock to Bit 5 to enable GPIO Port D.
	GPIOD_CFGLR &=~(0xf<<(4*0)); // Clear 4-bits relating to Port D0 (PD0).
	GPIOD_CFGLR |= (0x01 | 0x00)<<(4*0); // Set PD0 output to 10MHz slew rate, push-pull mode. 
}

#define GPIOD_BSHR *(uint32_t *)(MY_GPIOD_BASE+0x10) // Bit Set, High 16-bits Reset
void setPD0()
{
	// writing 0's have no effect on the BSHR register
	// thus |= not needed
	GPIOD_BSHR = (1<<0); // set PD0
}

#define GPIOD_BCR *(uint32_t *)(MY_GPIOD_BASE+0x14) // Bit Clear Register
void resetPD0()
{
	// writing 0's have no effect on the BCR register
	// thus |= not needed
	
	GPIOD_BSHR = (1<<(16+0)); // reset PD0
	//GPIOD_BCR = (1<<0); //reset PD0
}

void delay(uint32_t n)
{
	// for n = 100,000, I measured the delay to be about 162 (+/- 1) ms
	// volatile tells the compiler always use/read the value and not optimise
	// the for loop to take zero time.
	for (volatile uint32_t i=0;i<n;i++);
}

int main()
{
	initGPIO();

	while(1) {
		setPD0();
		delay(100000);
		resetPD0();
		delay(200000);
	}
}
