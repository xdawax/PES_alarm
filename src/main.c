#include <stdbool.h>
#include <assert.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "usart.h"
#include "defines.h"
#include "packet.h"
#include "rtc.h"
#include "bt.h"
#include "debug.h"
#include "delay.h"

#define MY_ADDRESS 0x07	// 0..255

sensor_t my_type = TEMP;

volatile packet_t packet;
QueueHandle_t tx_queue;
QueueHandle_t rx_queue;

// Functions
void ledInit(void);
void reedInit(void);
void interruptInit(void);
packet_t packet_init(uint8_t address, sensor_t sensor_type);

// Tasks
void data_transmitter(void *pvParameters);


int main(void)
{
	BaseType_t task_creation;
	
	packet = packet_init(MY_ADDRESS, my_type);
	reedInit();
    ledInit();
	usartInit(38400);
	interruptInit();

	tx_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	rx_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	
	packet_t packet_to_transmit;
	
	if (task_creation == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ) {
		assert(false);
	}

	while (1) {

		
		if (uxQueueMessagesWaiting(tx_queue)) {
			xQueueReceive(tx_queue, &packet_to_transmit, 100);
			tx_data(packet_to_transmit);
			wait_for_ack();
			//print_packet(packet_to_transmit);
			//test_buf_to_packet();
		}
		
	};

    
}


void data_transmitter(void *pvParameters) {
	while (1) {
		USART_TX_char(0);
		vTaskDelay(1000 / portTICK_RATE_MS);		// change to delay until
	}
}

packet_t packet_init(uint8_t address, sensor_t sensor_type) {
	packet_t packet;
	packet = packet_new();
	packet.address = address;
	packet.type = sensor_type;
	
	return packet;
}

void interruptInit() {
	__disable_irq();
	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;					// ENABLE clock for AFIO
	AFIO->EXTICR[3] |= (0b0001 << 0); 					// PB12
	EXTI->IMR |= (1 << EXT12);							// Unmask for EXT12
	EXTI->FTSR |= (1 << EXT12);							// Falling trigger
	EXTI->RTSR |= (1 << EXT12);							// Rising trigger
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	__enable_irq();
}



// Interrupt for event
void EXTI15_10_IRQHandler(void) {
	
	BaseType_t xHigherPriorityTaskWoken;

    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    if (!(GPIOB->IDR & GPIO_Pin_12)) {
		packet.data = DATA_SWITCH_OPEN;
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    }
    else {
		packet.data = DATA_SWITCH_CLOSED;
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
    }	

	packet.sequence++;
	uint8_t seq = packet.sequence;
	xQueueSendToBackFromISR(tx_queue, (void*)&packet, &xHigherPriorityTaskWoken);
	EXTI->PR = (1 << EXT12);
}

void reedInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void ledInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure PC13, PC14, PC15 as push-pull output
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}




