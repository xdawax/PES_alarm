#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

typedef struct time_stamp {
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} time_stamp_t;
#endif
