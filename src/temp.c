#include "temp.h"

void tempInit() {
	// Enable alternate function clock. Bit 0 in RCC APB2ENR register
	RCC->APB2ENR |= (1 << 0);
	// Enable GPIOD clock. Bit 5 in RCC APB2ENR register
	RCC->APB2ENR |= (1 << 5);
	// Enable GPIOA clock. Bit 2 in RCC APB2ENR register
	RCC->APB2ENR |= (1 << 2);
	// Enable clock for ADC1 clock. Bit 9 in RCC APB2ENR register
	RCC->APB2ENR |= (1 << 9);

	// Make PA0 analog input (0b0000)
	GPIOC->CRL &= 0xFFFFFFF0;
	GPIOC->CRL |= 0x00000000;

	// Make PORTD output
	GPIOD->CRL = 0x22222222;
	GPIOD->CRH = 0x22222222;
	GPIOD->ODR = 0;

	// Enable End of Conversion (EOC) interrupt
	ADC1->CR1 |= (1 << 5);

	// One conversion
	ADC1->SQR1 = 0x00000000;

	// Choose the analog channel to read
	// Since we want channel 10 to be the first
	//   conversion we write 10 to SQ1 bits (3:0)
	//   which is in SQR3 register. For multiple conversions
	//   keep writing the channel numbers to SQx bits.
	ADC1->SQR3 = (0 << 0);

	// Set up software trigger to start conversion
	ADC1->CR2 |= (7 << 17);  // Select SWSTART as trigger
	ADC1->CR2 |= (1 << 20);  // Enable external trigger

	// Enable continuous conversion
	ADC1->CR2 |= (1 << 1);

	//enable_interrupt(ADC1_2_IRQn);

	// Enable A/D conversion
	ADC1->CR2 |= (1 << 0);

	// Calibration reset and start
	//    Optional for better accuracy.
	ADC1->CR2 |= (1 << 3);
	while((ADC1->CR2 & (1 << 3)));
	ADC1->CR2 |= (1 << 2);
	while((ADC1->CR2 & (1 << 2)));

	// Start conversion with software trigger
	ADC1->CR2 |= (1<<22);
}