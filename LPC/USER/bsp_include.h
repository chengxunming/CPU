#ifndef __BSP_INCLUDE_H
#define __BSP_INCLUDE_H

/* Includes ------------------------------------------------------------------*/
//lib
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//设备相关
#include "LPC177x_8x.h"
#include "sys.h"
#include "delay.h"
#include "time.h"
#include "LPC1700CAN.h"

#include "bsp_net.h"
#include "bsp_pcf8563.h"


//CAN通信测试状态
typedef enum  
{
	CAN_EV_None=0,
	CAN_EV_ResOk,
	CAN_EV_ResFail,
	CAN_EV_ResTimeOut,
	CAN_EV_ResReady,
}CAN_EV_TYPE;
//CAN通信测试状态
typedef enum  
{
	NET_EV_None=0,
	NET_EV_ResOk,
	NET_EV_ResFail,
	NET_EV_ResTimeOut,
	NET_EV_ResReady,
}NET_EV_TYPE;
//单板类型定义
typedef enum  
{
	MD_NONE=0,
	MD_CPU=1,
	MD_COU=2,
	MD_SSU=3,
	MD_PDU=4,
	MD_DET=5,
	MD_IOU=6,
	MD_MAU=7,
	MD_MFU=8
}MODULE_TYPE;
//通信帧功能码定义
typedef enum  
{
	FUN_TX=1,
	FUN_RESULT,
	FUN_END
}FUN_TYPE;
//电路类型定义
typedef enum  
{
	CIRCUIT_CAN=1,
	CIRCUIT_CAN2,
	CIRCUIT_W5500,
	CIRCUIT_RS232,
	CIRCUIT_RS485,
	CIRCUIT_RTC
}CIRCUIT_TYPE;
//通信帧定义
typedef struct
{
	uint8_t module_id;
	uint8_t fun_code;
	uint8_t dataH;
	uint8_t dataL;
	uint8_t check_sum;
}Frame_DefType;

#define CPU_CAN_ID   0x01

#define TEST_CAN1_ID   0x11
#define TEST_CAN2_ID   0x12

extern uint8_t canUartData[256];
extern uint8_t candataLen;
extern CAN_EV_TYPE ev_CanRespone;
extern NET_EV_TYPE ev_NetRespone;

extern uint8_t Time_CanRespone_Count;
extern uint8_t Time_NetRespone_Count;

extern bool Flag_PCF8563_OK;

#endif

