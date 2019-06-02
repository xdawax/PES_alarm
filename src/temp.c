#include "temp.h"

#define STACK_SIZE 256
#define TEMP_TASK_PERIOD 500

int debug = 0;	

void initTasks(QueueHandle_t *tx_queue);
void vTempReadTask(void *queue);

void tempInit(QueueHandle_t *tx_queue) {
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
	
	initTasks(tx_queue);
	vTaskStartScheduler();
}

void initTasks(QueueHandle_t *tx_queue) {
	BaseType_t xReturned;
TaskHandle_t xHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    vTempReadTask,       /* Function that implements the task. */
                    "Temperature Reader",          /* Text name for the task. */
                    STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) tx_queue,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */
}

void vTempReadTask(void *queue) {
	
	while(1)
	{
		ADC1->CR2 |= (1 << 22); // start conversion 
		while(!(ADC1->SR & (1 << 1)));	// wait until conversion completes
		debug = ADC1->DR;
		vTaskDelay(TEMP_TASK_PERIOD / portTICK_RATE_MS);
	}
}

