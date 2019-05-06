#include "debug.h"


void print_packet(packet_t packet) {
	USART_TX_buf((uint8_t*)"PACKET CONTENTS: \r\n", 19);
	sensor_t type = packet.type;
	switch (type) {
		case REED:
			USART_TX_buf((uint8_t*)"Type: REED\r\n", 12);
			break;
		case TEMP:
			USART_TX_buf((uint8_t*)"Type: TEMP\r\n", 12);
			break;
		default:
			break;
	}
	
	USART_TX_buf((uint8_t*)"Data: ", 6);
	USART_TX_char(packet.data);
	USART_TX_buf((uint8_t*)"\r\n", 2);

	USART_TX_buf((uint8_t*)"Adress: ", 8);
	USART_TX_char(packet.adress);
	USART_TX_buf((uint8_t*)"\r\n", 3);	
}
