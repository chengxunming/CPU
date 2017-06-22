#ifndef __BSP_RS485_H
#define __BSP_RS485_H 

#include "LPC177x_8x.h"
#include "uart.h"


void RS485_Config(void);
void RS485_Send( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length );
void RS485_SendTest(void);
uint8_t RS485_RevResult(void);

#endif

