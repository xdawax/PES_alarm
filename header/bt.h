#ifndef BT_H_
#define BT_H_

#include <stdint.h>

/// Transmits a buffer using the UART protocol
///
/// @param buf[in] The buffer containing the packet data
///	@param size The length of the buffer to be transmitted
///
/// @retuns void
void BT_tx_data(uint8_t buf[], uint8_t size_t);

/// Transmits a buffer using the UART protocol
///
/// @param buf[in] The buffer containing the packet data
///	@param size The length of the buffer to be transmitted
///
/// @retuns void
void BT_register_event(uint8_t buf[], uint8_t size_t);

#endif