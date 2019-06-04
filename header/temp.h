#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "usart.h"
#include "defines.h"
#include "packet.h"
#include "rtc.h"
#include "bt.h"
#include "debug.h"
#include "delay.h"

void temp_init(packet_t packet);

