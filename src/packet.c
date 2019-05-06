#include "packet.h"
#include "usart.h"

uint8_t packet_to_buf(uint8_t buf[], packet_t packet) {
	uint8_t i;
	
	for (i = 0; i < 10; i++) {
		buf[i] = i;
	}
	
	return i;
}

packet_t buf_to_packet(uint8_t buf[]) {
	packet_t packet;
	
	return packet;
}

packet_t packet_new() {
	packet_t packet;
	
	packet.adress = 0;
	packet.data   = 0;
	packet.type   = REED;
	
	return packet;
	
}





