#include <stdbool.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "../header/usart.h"

#define EXT12 12
#define ONOFF 1
#define TYPE_ALARM 1
#define DATA_SWITCH_OPEN 0
#define DATA_SWITCH_CLOSED 1
#define QUEUE_LENGTH 100
#define QUEUE_ITEM_SIZE sizeof(txData)


typedef struct txData {
	uint8_t sensorType;		// defines what type of sensor has read the data (1 == ALARM, 2 == ?, 3 == ? etc...)
	uint8_t data;			// the data represented by an integer (if reed => 0 == switch open, 1 == switch closed)
} txData_t;

volatile txData_t txData;


void ledInit(void);
void reedInit(void);
void interruptInit(void);
void dataInit(uint8_t type, uint8_t data);
// RTOS task
void vTaskLedRed(void *p);
void vTaskLedYellow(void *p);
void vTaskLedGreen(void *p);


int main(void)
{
    dataInit(TYPE_ALARM, DATA_SWITCH_OPEN); 
	reedInit();
    ledInit();
	usartInit(9600);
	interruptInit();
	
    // Create LED blink task
    xTaskCreate(vTaskLedRed, (const char*) "Red LED Blink", 
        128, NULL, 1, NULL);
    xTaskCreate(vTaskLedYellow, (const char*) "Yellow LED Blink",
        128, NULL, 1, NULL);
    xTaskCreate(vTaskLedGreen, (const char*) "Green LED Blink", 128, NULL, 1, NULL);
	
	//Start RTOS scheduler
    vTaskStartScheduler();

	while (1) {};
				
    return 0;
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

void tx_dataStruct(txData_t data) {
	USART1->DR = data.sensorType + '0';
	while (!(USART1->SR & USART_SR_TXE)){}; // empty
	USART1->DR = data.data + '0';
	while (!(USART1->SR & USART_SR_TXE)){}; // empty
	USART1->DR = '\n';
	while (!(USART1->SR & USART_SR_TXE)){}; // empty
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
		
		tx_dataStruct(txData);
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



void vTaskLedRed(void *p)
{
	  for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_14;
        vTaskDelay(100/portTICK_RATE_MS);
    }
}

void vTaskLedYellow(void *p)
{
    for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_15;
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

void vTaskLedGreen(void *p)
{
    for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_13;
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}


