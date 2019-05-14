#ifndef BT_H_
#define BT_H_

#include <stdint.h>
#include "packet.h"
#include "usart.h"
#include "rtc.h"

#define ACK_WAIT_TIME 20				// in ms
#define MAX_BUF_SIZE sizeof(packet_t)
	
/// Transmits a buffer using the UART protocol
///
/// @param buf[in] The buffer containing the packet data
///	@param size The length of the buffer to be transmitted
///
/// @retuns true if the packet was sent
bool tx_data(packet_t packet);

/// Receives a buffer using the UART protocol
///
/// @retuns the received packet
packet_t rx_data(void);

bool wait_for_ack(packet_t packet);

bool received_ack(packet_t packet, packet_t received_packet);

bool tx_string(packet_t packet);
#endif
