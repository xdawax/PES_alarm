#include "temp.h"

#define STACK_SIZE 256
#define SAMPLE_INTERVAL 600
#define MAX_READS 10

volatile packet_t temp_packet;
int debug = 0;	

void initTasks(QueueHandle_t *tx_queue);
void initADC();
void vTempReadTask(void *queue);


void tempInit(QueueHandle_t *tx_queue, packet_t packet) {
	temp_packet = packet;
	initADC();
	initTasks(tx_queue);
	vTaskStartScheduler();
}

void initADC() {
	// enable clocks for ADC1 and PORTA
	RCC->APB2ENR |= (1 << 2);
	RCC->APB2ENR |= (1 << 9);

	// Make PA0 analog input (0b0000)
	GPIOC->CRL &= 0xFFFFFFF0;
	GPIOC->CRL |= 0x00000000;

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

void initTasks(QueueHandle_t *tx_queue) {
	
    /* Create the task, storing the handle. */
    xTaskCreate(
			vTempReadTask,       /* Function that implements the task. */
      "Temperature Reader",          /* Text name for the task. */
      STACK_SIZE,      /* Stack size in words, not bytes. */
      ( void * ) tx_queue,    /* Parameter passed into the task. */
      tskIDLE_PRIORITY,/* Priority at which the task is created. */
      NULL );      /* Used to pass out the created task's handle. */
}

uint32_t tempAverage(uint32_t readings[]) {
	uint32_t accumulator = 0;
	
	for (int i = 0; i < MAX_READS; i++) {
			accumulator += readings[i];
	}
	
	return accumulator / MAX_READS;
}

void vTempReadTask(void *tx_queue) {
	uint8_t readings;
	uint32_t tempData[MAX_READS];
	while(1)
	{
		readings = 0;
		
		for (;readings < MAX_READS; readings++) {
			ADC1->CR2 |= (1 << 22); // start conversion 
			while(!(ADC1->SR & (1 << 1)));	// wait until conversion completes
			tempData[readings] = ADC1->DR;
			vTaskDelay(SAMPLE_INTERVAL / portTICK_RATE_MS);
		}
		
		temp_packet.data = tempAverage(tempData);
		temp_packet.sequence++;
		
	// 255 is reserved for END_OF_COM, 254 for ACK
		if (temp_packet.sequence == 254) {
			temp_packet.sequence = 0;
		}
	
		packet_set_checksum(&temp_packet);
		//xQueueSendToBack(tx_queue, (void*)&temp_packet, 0);
		tx_data(temp_packet);
		
		}
}


