#ifndef __BSP_RS232_H
#define __BSP_RS232_H 			   

#include "bsp_include.h"

void RS232_Config(void);
void RS232_Send( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length );
void RS232_1_Respone_SendCheck(void);

void get_UAET0_useful_data(uint8_t *BufferPtr, uint32_t Length);
void get_UAET1_useful_data(uint8_t *BufferPtr, uint32_t Length);

#endif


