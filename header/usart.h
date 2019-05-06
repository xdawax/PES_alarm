#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO

#define GPIOC_TX 9
#define GPIOC_RX 10
#define SR_RXNE 5
#define SR_TE  	6

/// Initiates the USART1 with the settings 
/// 8bit word, 1 stop bit, no parity, no flow control, TX = PA9, RX = PA10
///
/// @param baudrate[in] the baudrate used by the USART1
///
/// @returns the word located at address
void usartInit(uint32_t baudrate);

uint8_t USART_RX_buf(uint8_t buf[]);


void USART_TX_buf(uint8_t buf[], uint8_t size);

void USART_TX_byte(uint8_t byte);

void USART_TX_char(uint8_t byte);
	
void USART_TX_string(uint8_t buf[], uint8_t size);
uint8_t USART_RX_byte(void);


#endif
