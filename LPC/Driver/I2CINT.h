/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  I2CINT.h
** Latest modified Date:       2009-6-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chenmingji
** Created date:               2009-6-8
** Version:
** Descriptions:               LPC17系列芯片硬件I2C软件包。主程序要配置好I2C总线接口(I2C引脚功能和
**                             I2C中断，并已使能I2C主模式)
**--------------------------------------------------------------------------------------------------------
** Modified by:                Zhangrong
** Modified date:              2009-6-8
** Version:
** Descriptions:
**
*********************************************************************************************************/
#ifndef I2CINT_H
#define I2CINT_H
#include "lpc177x_8x.h"
#define uint8_t    uint8_t
#define uint8	  uint8_t
#define INT16U   uint16_t
#define uint16   uint16_t
#define INT32U   uint32_t
#define uint32   uint32_t

#ifndef FALSE
#define FALSE    0
#endif

#ifndef TRUE
#define TRUE     1
#endif

#define ONE_BYTE_SUBA   1
#define TWO_BYTE_SUBA   2
#define X_ADD_8_SUBA    3

void TWIBus_init(void) ;

void fatalfault_TWI(uint8_t enable_ctdn);

void TWI_initial(void);

void TWI_poll(void);

//void select_commodule(const uint8_t);

//void TWI_SendStart(void);



extern  uint8_t	FailTWI[];				    //故障模块

//PCU
extern	uint8_t	TPCUSize;					//MCU发送至PCU数据长度
extern	uint8_t	RPCUSize;					//MCU接收PCU数据长度
extern	uint8_t	TPCU[];						//MCU发送至PCU缓冲数组
extern	signed char	RPCU[];					//MCU接收PCU缓冲数组
//extern	struct PCU;						//PCU通讯结构体声明

//EIU
//extern	typedef struct TWIEIU TEIU;						//EIU通讯结构体声明

//MCM
extern	uint8_t	TMCMSize;					//MCM
extern	uint8_t	RMCMSize;
extern 	uint8_t	TMCM[];
extern	uint8_t	RMCM[];
//extern	MCM;						//MCM协议变量

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        向无子地址器件发送1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t  ISendByte(uint8_t sla, uint8_t dat);

/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        向无子地址器件读取1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t  IRcvByte(uint8_t sla, uint8_t *dat);

/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        从有子地址器件任意地址开始读取N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           数据接收缓冲区指针
**                      num         读取的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t I2C_ReadNByte (uint8_t sla, INT32U suba_type, INT32U suba, uint8_t *s, INT32U num);

/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        向有子地址器件写入N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           将要写入的数据的指针
**                      num         将要写入的数据的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, INT32U suba, uint8_t *s, INT32U num);

#endif

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
