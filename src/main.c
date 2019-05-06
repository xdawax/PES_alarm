#include <stdbool.h>

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

#define MY_ADRESS 0x01	// 0..255

sensor_t my_type = TEMP;

volatile packet_t packet;

void ledInit(void);
void reedInit(void);
void interruptInit(void);
packet_t packet_init(uint8_t adress, sensor_t sensor_type);


int main(void)
{
	packet = packet_init(MY_ADRESS, my_type);
	reedInit();
    ledInit();
	usartInit(38400);
	interruptInit();

	while (1) {
		
	};

    
}

packet_t packet_init(uint8_t adress, sensor_t sensor_type) {
	packet_t packet;
	packet = packet_new();
	packet.adress = adress;
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


void EXTI15_10_IRQHandler(void) {
		// POSTA I KÖ!!! GÖR JOBBET UTANFÖR!!!
	    if (!(GPIOB->IDR & GPIO_Pin_12))
        {
            packet.data = 6666666;//!(GPIOB->IDR & GPIO_Pin_12);
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        }
        else
        {
            packet.data = 1111111; //(GPIOB->IDR & GPIO_Pin_12);
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
        }	
		
		tx_data(packet);
		print_packet(packet);
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




