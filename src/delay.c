#include "delay.h"


void delay_ms(uint16_t delay) {
	
	SysTick->LOAD = 9000 - 1;			// Reload with number of clock cycles / ms
	SysTick->VAL = 0;					// Reset the systick value register
	SysTick->CTRL |= (1 << 0);
	
	for (int i = 0; i < delay; i++) {
		while(!(SysTick->CTRL & (1 << COUNT_FLAG))) {};		// run until UF flag is set
	}
	
	SysTick->CTRL &= ~(1 << 0);
}
