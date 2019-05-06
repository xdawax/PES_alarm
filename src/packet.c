#include "packet.h"
#include "usart.h"

uint8_t packet_to_buf(uint8_t buf[], packet_t packet) {
	
	uint8_t size = 0;
	
	buf[BUF_ADRESS] = packet.adress;
	size++;
	buf[BUF_TYPE] = packet.type;
	size++;
	buf[BUF_DATA0] = (packet.data >> 8 * 0);	// Storing LSB first (Little endian)
	size++;
	buf[BUF_DATA1] = (packet.data >> 8 * 1); 
	size++;
	buf[BUF_DATA2] = (packet.data >> 8 * 2); 
	size++;
	buf[BUF_DATA3] = (packet.data >> 8 * 3); 
	size++;
	//buf[BUF_STAMP] = packet.time_stamp;	// TODO
	//size++;
	
	return size;
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

