#include "bt.h"

#define MAX_BUF_SIZE sizeof(packet_t)

void empty_buf(uint8_t buf[]) {
	for (int i = 0; i < MAX_BUF_SIZE; i++) {
		buf[i] = 0;
	}
}

bool tx_data(packet_t packet) {
	
	uint8_t buf[MAX_BUF_SIZE];
	empty_buf(buf);
	
	uint8_t bytes = 0;
	
	bytes = packet_to_buf(buf, packet);
	
	USART_TX_buf(buf, bytes);	
	
	return true;
}

packet_t rx_data() {
	packet_t packet;
	
	return packet;
}

bool tx_string(packet_t packet) {
	uint8_t buf[MAX_BUF_SIZE];
	uint8_t buf_size = 0;
	
	buf_size = packet_to_buf(buf, packet);
	
	USART_TX_string(buf, buf_size);	
	
	return true;
}
