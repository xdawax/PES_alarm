#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

// Register defines -> h-file
#define USART_UE 13
#define USART_M 12
#define USART_DMAT 7
#define USART_TE 3
#define USART_TC 6


void ledInit(void);
void usartInit(void);
// RTOS task
void vTaskLedRed(void *p);
void vTaskLedYellow(void *p);
void vTaskLedGreen(void *p);

int main(void)
{
    // Configure GPIO for LED
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

    return 0;
}

void ledInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure PC13, PC14, PC15 as push-pull output
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void usartInit()
{
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;              // enable clock for GPIOA
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;            // enable clock for USART1
		// PINS
		GPIOA->CRH |= (0x0BUL << 4);                  // Tx (PA9) alt. out push-pull
		GPIOA->CRH |= (0x04UL << 8);    
	
	
		// USART SETTINGS
		USART1->CR1 |= USART_CR1_UE;							// enable
		USART1->CR1 |= USART_CR1_M;								// word length 8 bit
		USART1->CR2 &= ~((1 << 13) | (1 << 12));	// bit 12-13 == 0,0 => 1 stop bit
		USART1->CR3 |= USART_CR3_DMAT;
		USART1->BRR = 0x1d4c;											// mantissa 468, fraction 0,75 => 111010100, 1100 (72MHz)
		USART1->CR1 |= USART_CR1_TE;							// tx enable
}

void vTaskLedRed(void *p)
{
	  for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_14;
				
				USART1->DR = (uint16_t)'a';
				while (!(USART1->SR & USART_SR_TXE)){}; // empty
        vTaskDelay(100/portTICK_RATE_MS);
    }
}

void vTaskLedYellow(void *p)
{
    for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_15;
        vTaskDelay(500/portTICK_RATE_MS);
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
