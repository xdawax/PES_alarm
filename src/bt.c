#include "bt.h"

void empty_buf(uint8_t buf[]) {
	for (int i = 0; i < MAX_BUF_SIZE; i++) {
		buf[i] = 0;
	}
}

bool tx_data(packet_t packet) {
	
	uint8_t buf[MAX_BUF_SIZE];
	empty_buf(buf);
	packet_set_checksum(&packet);
	
	uint8_t bytes = 0;
	
	bytes = packet_to_buf(buf, packet);
	
	USART_TX_buf(buf, bytes);	
	
	return true;
}

packet_t rx_data() {
	packet_t packet;
	uint8_t buf[255];
	
	USART_RX_buf(buf);
	
	packet = buf_to_packet(buf);
		
	return packet;
}

bool wait_for_ack(packet_t packet) {
	uint16_t ticks = 0;
	packet_t received_packet;
	
	while (ticks < ACK_WAIT_TIME) {
		if (USART_data_available()) {
			received_packet = rx_data();
		}
		if (received_ack(packet, received_packet)) {
			return true;
		}
	}
	
	return false;
}

bool received_ack(packet_t packet, packet_t received_packet) {
	if (packet.data == ACK && packet.sequence == received_packet.sequence) {
		return true;
	}
	return false;
}

bool tx_string(packet_t packet) {
	uint8_t buf[MAX_BUF_SIZE];
	uint8_t buf_size = 0;
	
	buf_size = packet_to_buf(buf, packet);
	
	USART_TX_string(buf, buf_size);	
	
	return true;
}
