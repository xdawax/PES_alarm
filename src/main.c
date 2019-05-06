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

typedef struct txData {
	uint8_t sensorType;		// defines what type of sensor has read the data (1 == ALARM, 2 == ?, 3 == ? etc...)
	uint8_t data;			// the data represented by an integer (if reed => 0 == switch open, 1 == switch closed)
} txData_t;

volatile txData_t txData;

sensor_t my_type = REED;

void ledInit(void);
void reedInit(void);
void interruptInit(void);
void dataInit(uint8_t type, uint8_t data);



int main(void)
{
    dataInit(TYPE_ALARM, DATA_SWITCH_OPEN); 
	reedInit();
    ledInit();
	usartInit(38400);
	interruptInit();

	packet_t pkt_test = packet_new();
	
	pkt_test.data = 'a';
	tx_string(pkt_test);
	while (1) {
		
	};

    
}


void dataInit(uint8_t type, uint8_t data) {
	txData.sensorType = type;
	txData.data = data;
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
	    if (!(GPIO_ReadInputData(GPIOB) & GPIO_Pin_12))
        {
            txData.data = DATA_SWITCH_OPEN;
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        }
        else
        {
            txData.data = DATA_SWITCH_CLOSED;
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
        }	
		USART_TX_buf((uint8_t*)"Sensor data: ", 13);
		USART_TX_byte(txData.data + '0');
		USART_TX_byte(txData.sensorType + '0');
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




