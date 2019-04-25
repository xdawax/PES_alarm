#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"


void ledInit(void);
void usartInit(void);
void buttonInit(void);
// RTOS task
void vTaskLedRed(void *p);
void vTaskLedYellow(void *p);
void vTaskLedGreen(void *p);

int main(void)
{
    // Configure GPIO for LED
		buttonInit();
    ledInit();
		usartInit();
    // Create LED blink task
    xTaskCreate(vTaskLedRed, (const char*) "Red LED Blink", 
        128, NULL, 1, NULL);
    xTaskCreate(vTaskLedYellow, (const char*) "Yellow LED Blink",
        128, NULL, 1, NULL);
    xTaskCreate(vTaskLedGreen, (const char*) "Green LED Blink", 128, NULL, 1, NULL);
    //Start RTOS scheduler
    vTaskStartScheduler();

		while (1) {
			
        if (!(GPIO_ReadInputData(GPIOB) & GPIO_Pin_12))
        {
            // Turn on LED on PA0 (LED circuit is active low)
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        }
        else
        {
            // Turn off LED on PA0
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
        }
		}
		
    return 0;
}

void buttonInit()
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

void usartInit()
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable peripheral clocks for USART1 on GPIOA */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_USART1 |
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO, ENABLE);
        
    /* Configure PA9 and PA10 as USART1 TX/RX */
    
    /* PA9 = alternate function push/pull output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* PA10 = floating input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure and initialize usart... */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
    USART_Init(USART1, &USART_InitStructure);
    
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE); 
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
				USART1->DR = (uint16_t)'A';
				while (!(USART1->SR & USART_SR_TXE)){}; // empty
				USART1->DR = (uint16_t)'T';
				while (!(USART1->SR & USART_SR_TXE)){}; // empty
				USART1->DR = (uint16_t)'\r';
				while (!(USART1->SR & USART_SR_TXE)){}; // empty
				USART1->DR = (uint16_t)'\n';
				while (!(USART1->SR & USART_SR_TXE)){}; // empty	
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
