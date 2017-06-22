#ifndef __BSP_H
#define __BSP_H 			   

#include "bsp_include.h"

void CAN_Config(void);
void NET_UDP_SocketConfig(void);
void NET_UDP_LoopBack(void);
void CAN1_RxMessage_Deal(Frame_DefType * FrameRx);
void CAN2_RxMessage_Deal(Frame_DefType * FrameRx);

void CPU_CanTest(void);
void CPU_NetTest(void);
void CPU_RS232Test(void);
void CPU_RS485Test(void);
void CPU_PCF8563Test(void);
void CPU_TestEnd(void);

uint8_t DataCheckSum(uint8_t *data,uint8_t len);

#endif
