#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO


/// Initiates the USART1 with the settings 
/// 8bit word, 1 stop bit, no parity, no flow control, TX = PA9, RX = PA10
///
/// @param baudrate[in] the baudrate used by the USART1
///
/// @returns the word located at address
void usartInit(uint32_t baudrate);

#endif
