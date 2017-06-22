/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           LPC1700CAN.h
** Last modified Date:  2011-07-14
** Last Version:        V1.0
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Liangbaoqiong
** Created date:        2011-07-14
** Version:             V1.0
** Descriptions:        First version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         chenyicheng   
** Modified date:       2011-07-14       
** Version:             V1.0           
** Descriptions:                    
**
*********************************************************************************************************/
#ifndef _LPC1700CAN_H_
#define _LPC1700CAN_H_

#include "LPC177x_8x.h"
#include "type.h"
#define uint8	  uint8_t
#define uint16	uint16_t
#define int16	int16_t

/*********************************************************************************************************
**               C++代码兼容                                                                            
*********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */

/*********************************************************************************************************
**                    Fpclk=48MHz CAN标准波特率值                                                       
*********************************************************************************************************/
//#define BPS_1000K   0x50005
//#define BPS_800K    0x2000B
//#define BPS_500K    0x5000B
//#define BPS_250K    0x50017
//#define BPS_125K    0x5002F
//#define BPS_100K    0x5003B
//#define BPS_50K     0x50077
//#define BPS_20K     0x5012B
//#define BPS_10K     0x50257
//#define BPS_5K      0x2F01E0

/*********************************************************************************************************
**                    Fpclk=60MHz CAN标准波特率值                                                       
*********************************************************************************************************/
#define BPS_1000K   0x160005
#define BPS_800K    0x2E0003
#define BPS_500K    0x2F0005
#define BPS_250K    0x2F000B
#define BPS_125K    0x2F0017
#define BPS_100K    0x1B0027
#define BPS_50K     0x2F003B
#define BPS_20K     0xFF0077
#define BPS_10K     0xFF00EF
#define BPS_5K      0xFF01DF

#define CANOFFSET   0x4000
#define RGE(Address)    (*((volatile unsigned long*)Address))
#define CANAPP_GLOBAL   extern


#define COMMAND_ENABLE			0xAA				//发送控制使能命令
#define COMMAND_DISABLE			0x55				//发送控制除能命令



typedef struct 
{
	uint32_t id; 			/**< 29 bit identifier, it depend on "format" value
								 - if format = STD_ID_FORMAT, id should be 11 bit identifier
								 - if format = EXT_ID_FORMAT, id should be 29 bit identifier
							 */
	uint8_t dataA[4]; 		/**< Data field A */
	uint8_t dataB[4]; 		/**< Data field B */
	uint8_t len; 			/**< Length of data field in bytes, should be:
								 - 0000b-0111b: 0-7 bytes
								 - 1xxxb: 8 bytes
							*/
	uint8_t format; 		/**< Identifier Format, should be:
								 - STD_ID_FORMAT: Standard ID - 11 bit format
								 - EXT_ID_FORMAT: Extended ID - 29 bit format
							*/
	uint8_t type; 			/**< Remote Frame transmission, should be:
								 - DATA_FRAME: the number of data bytes called out by the DLC
								 field are send from the CANxTDA and CANxTDB registers
								 - REMOTE_FRAME: Remote Frame is sent
							*/
} CAN_MSG_Type;

typedef enum 
{
	DATA_FRAME = 0, 	/**< Data frame */
	REMOTE_FRAME = 1	/**< Remote frame */
} CAN_FRAME_Type;
	
typedef enum 
{
	STD_ID_FORMAT = 0, 	/**< Use standard ID format (11 bit ID) */
	EXT_ID_FORMAT = 1	/**< Use extended ID format (29 bit ID) */
} CAN_ID_FORMAT_Type;

/*********************************************************************************************************
**   定义 CAN 报文收发项 结构体:
*********************************************************************************************************/
typedef struct _MessageDetail
{
    unsigned char LEN;
    unsigned char FF;                                                   /* 是否标准帧                   */
    unsigned int CANID;                                                 /* 长度与FF有关                 */
    unsigned int DATAA;
    unsigned int DATAB;
}MessageDetail;



/*********************************************************************************************************
**   定义 CANMOD 寄存器:
1.This register controls the basic operating mode of the CAN Controller. 
2.Bits not listed read as 0 and should be written as 0.
*********************************************************************************************************/
typedef union   _canmod_
{
    unsigned long DWord;
    struct {
        unsigned int    RM      :1;                                     /* 定义RM位(复位位)             */
        unsigned int    LOM     :1;                                     /*定义LOM位(只听模式位)         */

        /*
        * 定义STM位 "0"发送的信息必须被应答才被认可
        * "1"自测试模式,CANCMR的SRR位一起使用，可以达到自发自收的效果
        */
        unsigned int    STM     :1; 
        /*
         * 定义TPM位 "0"3个发送缓冲区的优先级由各自的CAN ID决定
         * "1"3个发送缓冲区的优先级由各自的Tx优先级域决定
         */
        unsigned int    TPM     :1;
                
        unsigned int    SM      :1;                                     /* 定义SM位(睡眠模式位)         */

        unsigned int    RPM     :1;
        /*
        * 定义RPM位 "0"如果传输的是显性位，Rx和Tx脚为低电平
        * "1"翻转极性模式--如果传输的是显性位，Rx脚为高电平
        */
        unsigned int    RSV1    :1;                                     /* 保留位                       */
        unsigned int    TM      :1;                                     /* 定义TM位(测试模式)           */
        unsigned int    RSV24   :24;                                    /* 保留位                       */
    }Bits;
}REG_CANMOD;

/*********************************************************************************************************
**   定义 CAN 帧结构体:
*********************************************************************************************************/
typedef struct _CANFRMAE
{
    unsigned long can_id;                                                   /* 帧ID                         */ 
    unsigned char can_dlc;                                                  /* 数据长度                     */
    unsigned char RTR;                                                  /* 远程帧标志                   */
    unsigned char FF;                                                   /* 扩展帧标志                   */
    unsigned char data[8];                                              /* 帧数据                       */
}CANFRAME;

/*********************************************************************************************************
** Function name:  	unsigned char Enter_SWRst(unsigned char Ch);
** Descriptions:  	Ch:CAN控制器号，0表示第一路CAN,CAN控制器进入软件复位模式
** Returned value:      0：          失败
**                      1:           成功	
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char Enter_SWRst(unsigned char Ch);

/**********************************************************************************************************
** Function name:       unsigned char Quit_SWRst(unsigned char Ch);
** Descriptions:        Ch:CAN控制器号，0表示第一路CAN,CAN控制器退出软件复位模式
** Returned value:      0：          失败
**                      1:           成功	
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char Quit_SWRst(unsigned char Ch);

/*********************************************************************************************************
** Function name:   	unsigned char CAN_Init(unsigned char Ch, unsigned long Baud);
** Descriptions:        CAN控制器退出软件复位模式
** input parameters:    Ch:          CAN控制器号，0表示第一路CAN
**                      Baud:        CAN波特率值
** Returned value:      无
*********************************************************************************************************/
CANAPP_GLOBAL void CAN_Init(unsigned char Ch, unsigned long Baud);

/*********************************************************************************************************
** Function name:  	unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int 
**                      ID, unsigned char *data);
** Descriptions: 	用户填写发送 帧信息
** input parameters:    LEN:         数据长度
**                      *data:       发送数据所在的数组 
** Returned value:      操作成功返回 1 失败返回 0
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char writedetail(unsigned char LEN,unsigned char FF,
                                        unsigned int ID,unsigned char *data);

/*********************************************************************************************************
** Function name: 	unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);
** Descriptions:        CAN控制器退出软件复位模式
** input parameters:    Ch:CAN控制器号，0表示第一路CAN
**                       BUFNum 选择缓冲区
                         MessageDetailT 报文的特征信息及数据
** Returned value:无
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);

//过滤器
void CAN_wrFilter (uint32_t ctrl, uint32_t id, uint8_t format);

/** CAN peripheral ID 0 */
#define CAN_1		0

/** CAN peripheral ID 1 */
#define CAN_2		1

#define STANDARD_FORMAT  0
#define EXTENDED_FORMAT  1

typedef enum
{
	CAN_ID_1 = CAN_1,
	CAN_ID_2 = CAN_2
}en_CAN_unitId;


Status Send_CAN_DataFrame(uint8_t ch,uint8_t addr,uint8_t *ptr,uint8_t len);



#ifdef __cplusplus
}
#endif 
                                                                       /*  __cplusplus                 */
#endif

/********************************************************************************************************
**                                        End Of File                                                                             
********************************************************************************************************/
