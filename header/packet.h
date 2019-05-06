#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include "rtc.h"

/********************** DEFINES ****************************/
//[adress, type, data, data, data, data, stamp, stamp, stamp, stamp, stamp]

#define DATA_MAX_SIZE 32	// maximum number of bytes in the data field 0-255
#define PACKET_SIZE sizeof(packet_t);
#define TIME_STAMP_SIZE sizeof(time_stamp_t);
#define BUF_ADRESS 0
#define BUF_TYPE 1
#define BUF_DATA0 2
#define BUF_DATA1 3
#define BUF_DATA2 4
#define BUF_DATA3 5
#define BUF_STAMP0 6
#define BUF_STAMP1 7
#define BUF_STAMP2 8
#define BUF_STAMP3 9
#define BUF_STAMP4 10


/********************** TYPEDEFS ****************************/


typedef enum sensor_type {	// 1 byte if < 256 sensors
	REED, 					// the data can be read from the reed_data enum
	TEMP					// the data is read as centidegrees celcius i.e. 3275 == 32.75 deg C
} sensor_t;

typedef enum reed_data {
	DOOR_CLOSED,			// if the door is closed the pin value == 0
	DOOR_OPEN				// if the door is opened the pin value == 1
} reed_data_t;

typedef struct packet {
	uint8_t adress;
	sensor_t type;
	uint32_t data;
	//time_stamp_t  time_stamp;		// TODO
} packet_t;


/********************** SETTERS ****************************/

/// Initiates a packet with 0 in all fields
///
/// @returns a packet with all fields set to 0
packet_t packet_new(void);

/// Sets the adress field in the packet struct
///
/// @param adress[in] the adress of the transmitting unit
///
/// @returns void
void packet_set_adress(packet_t *packet, uint8_t adress);

/// Sets the type field in the packet struct
///
/// @param type[in] the type of the sensor used to gather the data
///
/// @returns void
void packet_set_type(packet_t *packet, sensor_t type);

/// Sets the data field in the packet struct
/// @ref sensor_type for data interpretation
/// @param data[in] the data part of the packet
///
/// @returns void
void packet_set_data(packet_t *packet, uint32_t data);

/// Sets the time_stamp field in the packet struct
///
/// @param time_stamp[in] the time at when the data was collected
///
/// @returns void
void packet_set_time(packet_t *packet, time_stamp_t time_stamp);


/********************** GETTERS ****************************/


/// Retrieves the adress field in the packet struct
///
/// @returns the transmitters adress
uint8_t packet_get_adress(packet_t *packet);

/// Retrieves the type field in the packet struct
///
/// @returns the type of the sensor responsible for the data
sensor_t packet_get_type(packet_t *packet);

/// Retrieves the data field in the struct 
///
/// @ref sensor_type for data interpretation
/// @returns the data of the packet
uint32_t packet_get_data(packet_t *packet);

/// Retrieves the time_stamp field in the packet struct
///
/// @returns the time stamp for the packet
time_stamp_t packet_get_time(packet_t *packet);


/********************** TX/RX ****************************/


/// Transmits the packet in form of a 8-bit buffer
///
/// @param packet[in] the packet to be transmitted
///
/// @returns void
void packet_TX(packet_t packet);

/// Receives the packet in form of a 8-bit buffer
/// and converts it to the packet format
///
/// @returns packet_t the retrieved data in form of the packet struct
packet_t packet_RX(void);


/******************* PACKET <-> BUF **********************/


/// Converts an 8-bit buffer to a packet struct
///
/// @param buf[in] the buffer to be converted 
///
/// @returns packet_t the packet comprised from the buffer
packet_t buf_to_packet(uint8_t buf[]);

/// Converts a packet struct to an 8-bit buffer 
/// [adress, type, data, data, data, data, stamp, stamp, stamp, stamp, stamp] bytes [0 .. 11]
/// 
/// @param packet[in] the packet to be converted
/// @param buf[out] the buffer to be filled
///
/// @return the number of bytes in the packet
uint8_t packet_to_buf(uint8_t buf[], packet_t packet);



#endif
