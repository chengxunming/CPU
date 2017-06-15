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

#include "bsp.h"
#include "bsp_net.h"



typedef enum  
{
	CAN_TX_WAIT=0,
	CAN_TX_START,
	CAN_TX_OK,
}CANTX_STATE_TYPE;

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
	FUN_CAN=1,
	FUN_RESULT,
	FUN_END
}FUN_TYPE;
//电路类型定义
typedef enum  
{
	CIRCUIT_1=1,
	CIRCUIT_2,
	CIRCUIT_3,
	CIRCUIT_4,
	CIRCUIT_5,
	CIRCUIT_6,
	CIRCUIT_7,
	CIRCUIT_8,
	CIRCUIT_9,
	CIRCUIT_10
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

extern uint8_t canUartData[256];
extern uint8_t candataLen;

#endif

