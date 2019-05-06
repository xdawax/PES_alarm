#include "usart.h"


void usartInit(uint32_t baudRate)
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
    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
    USART_Init(USART1, &USART_InitStructure);
    
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE); 
}

uint8_t USART_RX_buf(uint8_t buf[]) {
	
	uint8_t index = 0;
	uint8_t received = 1;
	while (received != '\r') {
		received = USART_RX_byte();
		buf[index] = received;
		USART_TX_byte(buf[index]);
		index++;
	}
		
	return index;
}


void USART_TX_buf(uint8_t buf[], uint8_t size) {
	int i;
	
	for (i = 0; i < size; i++) {
		USART_TX_byte(buf[i]);
	}
}


void USART_TX_byte(uint8_t byte) {
	
	USART1->DR = byte;
	while (!(USART1->SR & USART_SR_TC)){}; // empty
	//USART1->DR = '\r';	
	//while (!(USART1->SR & USART_SR_TC)){}; // empty
}

uint8_t USART_RX_byte() {
	while(!(USART1->SR & (1 << SR_RXNE))) {};
	return USART1->DR;
}

void USART_TX_string(uint8_t buf[], uint8_t size) {
	int i;
	
	for (i = 0; i < size; i++) {
		USART_TX_char(buf[i]);
	}
}


void USART_TX_char(uint8_t byte) {
	
	USART1->DR = byte + '0';
	while (!(USART1->SR & USART_SR_TC)){}; // empty
	USART1->DR = '\r';	
	while (!(USART1->SR & USART_SR_TC)){}; // empty
}


