#ifndef __BSP_H
#define __BSP_H 			   

#include "bsp_include.h"

//void CAN_Respone_SendCheck(void);
//void MFU_TestCheck(void);
void NET_UDP_SocketConfig(void);
void NET_UDP_LoopBack(void);
//void TEST_Finish_Check(void);
void CAN1_RxMessage_Deal(Frame_DefType * FrameRx);
void CAN2_RxMessage_Deal(Frame_DefType * FrameRx);

void CPU_PCF8563_Test(void);

uint8_t DataCheckSum(uint8_t *data,uint8_t len);

#endif
