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
	USART_TX_char(packet.address);
	USART_TX_buf((uint8_t*)"\r\n", 3);
	USART_TX_buf((uint8_t*)"Sequence: ", 10);
	USART_TX_char(packet.sequence);
	USART_TX_buf((uint8_t*)"\r\n", 3);	
}

void test_buf_to_packet() {
	uint8_t buf[255];
	packet_t original_packet = packet_new();
	packet_t packet_from_buf;
	
	original_packet.address = 1;
	original_packet.data = 123456789;
	original_packet.sequence = 5;
	original_packet.type = REED;
	
	packet_to_buf(buf, original_packet);
	packet_from_buf = buf_to_packet(buf);
	
	print_packet(original_packet);
	print_packet(packet_from_buf);
		
}




