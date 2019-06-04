#include "temp.h"

#define STACK_SIZE 256
#define SAMPLE_INTERVAL 600
#define MAX_READS 10
#define TX_ATTEMPTS 5

volatile packet_t temp_packet;
int debug = 0;	
packet_t ack;

void init_tasks(void);
void init_ADC(void);
void vTempReadTask(void);


void temp_init(packet_t packet) {
	temp_packet = packet;
	init_ADC();
	init_tasks();
	vTaskStartScheduler();
}

// was one bit off, configettings from Furkan Cayci
void init_ADC() {
	// enable clocks for ADC1 and PORTA
	RCC->APB2ENR |= (1 << 2);
	RCC->APB2ENR |= (1 << 9);

	// Make PA0 analog input (0b0000)
	GPIOC->CRL &= 0xFFFFFFF0;
	GPIOC->CRL |= 0x00000000;

	// One conversion
	ADC1->SQR1 = 0x00000000;

	ADC1->SQR3 = (0 << 0);

	// Set up software trigger to start conversion
	ADC1->CR2 |= (7 << 17);  // Select SWSTART as trigger
	ADC1->CR2 |= (1 << 20);  // Enable external trigger

	// Enable continuous conversion
	ADC1->CR2 |= (1 << 1);

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

// FreeRTOS task example from API
void init_tasks() {
	
    /* Create the task, storing the handle. */
    xTaskCreate(
			vTempReadTask,       /* Function that implements the task. */
      "Temperature Reader",          /* Text name for the task. */
      STACK_SIZE,      /* Stack size in words, not bytes. */
      NULL,    /* Parameter passed into the task. */
      tskIDLE_PRIORITY,/* Priority at which the task is created. */
      NULL );      /* Used to pass out the created task's handle. */
}

uint32_t temp_average(uint32_t readings[]) {
	uint32_t accumulator = 0;
	
	for (int i = 0; i < MAX_READS; i++) {
			accumulator += readings[i];
	}
	
	return accumulator / MAX_READS;
}

void vTempReadTask() {
	uint8_t readings;
	uint8_t tries;
	
	uint32_t temp_data[MAX_READS];
	while(1)
	{
		readings = 0;
		tries = 0;
		for (;readings < MAX_READS; readings++) {
			ADC1->CR2 |= (1 << 22); // start conversion 
			while(!(ADC1->SR & (1 << 1)));	// wait until conversion completes
			temp_data[readings] = ADC1->DR;
			vTaskDelay(SAMPLE_INTERVAL / portTICK_RATE_MS);
		}
		
		temp_packet.data = temp_average(temp_data);
		temp_packet.sequence++;
		
	// 255 is reserved for END_OF_COM, 254 for ACK
		if (temp_packet.sequence == 254) {
			temp_packet.sequence = 0;
		}
	
		packet_set_checksum(&temp_packet);
		
		while (tries < TX_ATTEMPTS) {
				tx_data(temp_packet);
				ack = rx_data();
				tries++;
				if (temp_packet.sequence == ack.sequence) {
					break;
				}
			}
		tx_data(temp_packet);

		}
}


