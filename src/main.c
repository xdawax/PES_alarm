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

#define MY_ADDRESS (*(unsigned long *)0x1FFFF7E8)		// 16 lsb of the device id
#define TX_ATTEMPTS 5
sensor_t my_type = TEMP;

volatile packet_t packet;
QueueHandle_t tx_queue;
QueueHandle_t rx_queue;

// Functions
void ledInit(void);
void reedInit(void);
void interruptInit(void);
packet_t packet_init(uint32_t address, sensor_t sensor_type);

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

	int tries;
	
	tx_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	rx_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
	
	packet_t packet_to_transmit;
	packet_t ack;
	if (task_creation == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ) {
		assert(false);
	}
	packet_to_transmit.sequence = 240;
	while (1) {
		tries = 0;
		//ack = rx_data();
		if (uxQueueMessagesWaiting(tx_queue)) {
			xQueueReceive(tx_queue, &packet_to_transmit, 100);
			while (tries < TX_ATTEMPTS) {
				tx_data(packet_to_transmit);
				ack = rx_data();
				tries++;
				if (packet_to_transmit.sequence == ack.sequence) {
					break;
				}
			}
		} 
		ack.data = 0;
	};
    
}


void data_transmitter(void *pvParameters) {
	while (1) {
		USART_TX_char(0);
		vTaskDelay(1000 / portTICK_RATE_MS);		// change to delay until
	}
}

packet_t packet_init(uint32_t address, sensor_t sensor_type) {
	uint32_t deb = address;
	packet_t packet;
	packet = packet_new();
	packet.address = (uint8_t)address;
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
	
	// 255 is reserved for END_OF_COM, 254 for ACK
	if (packet.sequence == 254) {
		packet.sequence = 0;
	}
	
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




