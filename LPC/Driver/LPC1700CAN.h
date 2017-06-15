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


//1778核心板新增定义

#define CTRL_ACTIVE				1					//控制有效
#define CTRL_INACTIVE			0					//控制无效

#define STATUS_ACTIVE			1					//状态有效
#define STATUS_INACTIVE			0					//状态无效
#define STATUS_UNVAILABLE		2 					//STATUS UNVAILABLE



typedef struct tag_dataFrame
{
	uint8_t ifExt:2;
	uint8_t ifFirmware:1;
	uint8_t reserver:5;
	uint16_t dataType;
	uint8_t data[5];
}t_dataFrame;

typedef struct tag_IDFrame
{
	uint8_t nodeId:7;
	uint8_t addrType:1;
	uint8_t reserver:2;
	uint8_t dataType:1;
}t_IDFrame;


typedef struct tag_DTnS
{
	uint8_t ifHaveCar:1;
	uint8_t detType:1;
	uint8_t reserver0:1;
	uint8_t ifBreakErr:1;
	uint8_t ifOtherErr:1;
	uint8_t ifTune:1;
	uint8_t ifLErr:1;
	uint8_t reserver1:1;
}t_DTnS;

typedef struct tag_IOnS
{
	uint8_t ifHaveSth:1;
	uint8_t ioType:1;
	uint8_t reserver:6;
}t_IOnS;

typedef struct tag_dev_s
{
	uint8_t dev_active;
	uint8_t time_count;
}t_dev_s;

typedef struct S_PDULOADDRV								//PDU控制数据
{
	uint8	cOutput:4;									//M3/M2/M1/M0
	uint8	bOnOff:1;									//x
	uint8	bFlash:1;									//Y
	
}SpduLoadDrv;

typedef struct S_BITDRV
{
	uint8 	ch1:1,ch2:1,ch3:1,ch4:1;					//
	uint8 	ch5:1,ch6:1,ch7:1,ch8:1;
}SbitDrv;

typedef struct S_DETSTACHECK
{
	uint8_t 	bAct:1;			//触发状态，0表示未检测到，1表示检测到
	uint8_t 	bActMode:1;		//触发模式，0表示presence模式，1表示pulse模式
	uint8_t 	bLoopBreak:1;	//线圈断路故障，0表示不存在，1表示存在（仅用于DET）
	uint8_t	bLoopFault:1;	//线圈其他故障，0表示不存在，1表示存在（仅用于DET）
	uint8_t	bChnTune:1;		//通道调谐，0表示非调谐中，1表示调谐中（仅用于DET）
	uint8_t	bChnInEx:1;		//通道线圈电感量变化过大故障，0表示不存在，1表示存在（仅用于DET）
	
}SdetStaCheck;

typedef struct S_PDULOADCHECK
{
	uint8_t ra1:2;
	uint8_t rb1:2;
	uint8_t ya1:2;
	uint8_t yb1:2;
	uint8_t ga1:2;
	uint8_t gb1:2;
	uint8_t ra2:2;
	uint8_t rb2:2;
	uint8_t ya2:2;
	uint8_t yb2:2;
	uint8_t ga2:2;
	uint8_t gb2:2;
	uint8_t ra3:2;
	uint8_t rb3:2;
	uint8_t ya3:2;
	uint8_t yb3:2;
	uint8_t ga3:2;
	uint8_t gb3:2;
	uint8_t ra4:2;
	uint8_t rb4:2;
	uint8_t ya4:2;
	uint8_t yb4:2;
	uint8_t ga4:2;
	uint8_t gb4:2;
	uint8_t current[4];
	uint8_t power;
	
}SpduloadCheck;


typedef struct S_CANSEND								//发送的命令
{
	//SSU
	uint8_t		bLoadRelayOn:1;									//外灯继电器控制	"LMPC"
	uint8_t 		bDimRelayOn:1;									//暗灯继电器控制	"DIMC"
	uint8_t 		bMfuRelayOn:1;									//MFU继电器控制		"MFUC"
	//PDU
	SpduLoadDrv	sLoadDrv[8][4];				//负载驱动			"PDNC"
	//IOU
	SbitDrv		sLogicDrv[5];			//IOU控制数据		"IONC"
	uint8_t		cIouDetReset[4];		//板卡复位			"DRNC"
	//MAU
	uint8_t 		bWarn:1;							//警告故障		"WARC"
	uint8_t 		bFatal:1;							//严重故障		"FATC"
	uint8_t		cMauPass:5;							//密码			"PASC"
	uint8_t 		bLoadStatus:1;						//负载供电开关状态	"LMSC"
	uint8_t 		cCtrlSource:4;						//控制源状态	"CSOC"
	uint8_t 		cFlashSource:4;						//闪烁源状态	"FLSC"
	uint8_t 		bRemoteOpen:1;						//遥控状态		"REMC"
	SbitDrv		sKeyNum;							//数字键状态	"KNMC"
	uint8_t 		bKeyStepOn:1;						//步进状态		"KSPC"
	uint8_t 		bKeyAllRedOn:1;						//全红状态		"KARC"
	uint8_t 		bKeyAOn:1;							//键A 			"KLAC"
	uint8_t 		bKeyBOn:1;							//键B 			"KLBC"
	uint8_t 		bKeyCOn:1;							//键C 			"KLCC"
	uint8_t 		bKeyDOn:1;							//键D			"KLDC"
	uint8_t 		bKeyEOn:1;							//键A 			"KLEC"
	uint8_t 		bKeyTestOn:1;						//键灯测试		"KTSC"
	
}ScanSend;

typedef struct S_CANRECEIVE								//接收的状态
{
	//MAU
	uint8_t	bToggleOn:1;						//外灯打开		"MLMS"	
	uint8_t	bManualOn:1;						//手控使能		"MMAS"
	uint8_t	bFlashOn:1;							//闪烁使能		"MFSS"
	uint8_t	bRemoteOn:1;						//遥控使能		"MRTS"
	uint8_t 	cKey;									//按键号		"MKYS"
	uint8_t 	bMauEncryption:1;						//MAU加密状态	"MAUS"
	//SSU
	uint8_t 	bMfuWork:1;								//MFU运行		"MFUS"
	uint8_t 	bACSA:1;								//交流掉电状态	"ACSA"
	uint8_t 	bACSE:1;								//交流短掉电状态"DIPS"
	uint8_t		bDcFail:1;								//直流掉电状态	"DCFS"
	uint8_t 	cAcVolt;								//交流电压值	"SAMS"
	uint8_t 	cDcVolt;								//直流电压值	"SAMS"
	uint8_t 	cAcFreq;								//交流频率值	"SAMS"
	uint8_t 	cTemp;									//温度值		"SAMS"
	uint8_t 	bACSD:1;						//空开负载回路供电	"PAOS"
	uint8_t		bACSB:1;							//1#220V/暗灯继电器后端供电	"PRIS"
	uint8_t 	bACSC:1;							//3#A/B负载继电器后端供电	"PROS"
	//PDU
	SpduloadCheck	sLoadCheck[8];				//通道灯态检测			"PDNS"
	//DET/IOU
	SdetStaCheck	sDetCheck[4][8];		//DET/IOU状态			"DTNS""IONS"
	
}ScanReceive;

/** CAN peripheral ID 0 */
#define CAN_1		0

/** CAN peripheral ID 1 */
#define CAN_2		1

#define DEV_NUM 128

typedef enum
{
	CAN_ID_1 = CAN_1,
	CAN_ID_2 = CAN_2
} en_CAN_unitId;





enum EXT_FLAG
{
	NO_EXT,
	DATA_BEGIN,
	DATA_MIDD,
	DATA_END
};

enum FIRMWARE_FLAG
{
	FIRMWARE_NOT,
	FIRMWARE_YES
};

enum DATA_TYPE_FLAG
{
	APP_DATA,
	HEART_BEAT,
};

enum DATA_ABLED
{
	COMMAND_EN=0xAA,
	COMMAND_DIS=0x55
};

enum ID_FLAG
{
	ID_REV,
	ID_SEND,
};


enum MKYC_DATA_TYPE
{
	COMMAND_FUN0=0,
	COMMAND_FUN1,
	COMMAND_FUN2,
	COMMAND_FUN3,
	COMMAND_FUN4,
	COMMAND_FUN5,
	COMMAND_FUN6,
	COMMAND_FUN7,
	COMMAND_FUN8,
	COMMAND_FUN9,
	COMMAND_FUN10,
	COMMAND_FUN11,
	COMMAND_FUN12,
	COMMAND_FUN13,
	COMMAND_FUN14,
	COMMAND_FUN15,
	COMMAND_FUN16,
};

enum DEV_STATUS
{
	DEV_STATUS_FAILURE,
	DEV_STATUS_ACTIVE
};


enum CAN_COMM_COMMAND
{
	LMPC=1,
	DIMC,
	MFUC,	
	PASC=11,	
	PD1C=31,
	PD2C,
	PD3C,
	PD4C,
	PD5C,
	PD6C,
	PD7C,
	PD8C,	
	IO1C=71,
	IO2C,
	IO3C,
	IO4C,
	IO5C,
	IO6C,
	IO7C,
	IO8C,
	IO9C,
	IO10C,
	IO11C,
	IO12C,
	IO13C,
	IO14C,
	IO15C,
	IO16C,
	IO17C,
	IO18C,
	IO19C,
	IO20C,
	IO21C,
	IO22C,
	IO23C,
	IO24C,
	IO25C,
	IO26C,
	IO27C,
	IO28C,	
/*	DE1C=111,
	DE2C,
	DE3C,
	DE4C,
	DE5C,
	DE6C,
	DE7C,
	DE8C,
	DE9C,
	DE10C,
	DE11C,
	DE12C,
	DE13C,
	DE14C,
	DE15C,
	DE16C,
	DE17C,
	DE18C,
	DE19C,
	DE20C,
	DE21C,
	DE22C,
	DE23C,
	DE24C,
	DE25C,
	DE26C,
	DE27C,
	DE28C,	
	DT1C=151,
	DT2C,
	DT3C,
	DT4C,
	DT5C,
	DT6C,
	DT7C,
	DT8C,
	DT9C,
	DT10C,
	DT11C,
	DT12C,
	DT13C,
	DT14C,
	DT15C,
	DT16C,
	DT17C,
	DT18C,
	DT19C,
	DT20C,
	DT21C,
	DT22C,
	DT23C,
	DT24C,
	DT25C,
	DT26C,
	DT27C,
	DT28C,	*/
	DR1C=191,
	DR2C,
	DR3C,
	DR4C,
	DR5C,
	DR6C,
	DR7C,
	DR8C,
	DR9C,
	DR10C,
	DR11C,
	DR12C,
	DR13C,
	DR14C,
	DR15C,
	DR16C,
	DR17C,
	DR18C,
	DR19C,
	DR20C,
	DR21C,
	DR22C,
	DR23C,
	DR24C,
	DR25C,
	DR26C,
	DR27C,
	DR28C,
	WARC=321,
	FATC,
	LMSC,
	CSOC,
	FLSC,
	REMC,
	KNMC,
	KSPC,
	KARC,
	KLAC,
	KLBC,
	KLCC,
	KLDC,
	KLEC,
	KTSC,
};

enum CAN_COMM_STATUS
{
	MLMS=21,
	MMAS,
	MFSS,
	MRTS,
	MKYS,
	MAVS,
	MFUS=301,
	ACSA,
	DCFS,
	SAMS,
	ACSD,
	ACSC,
	ACSB,
	SSVS,
	ACSE,
	MAUS=341,
	PD1S=361,
	PD2S,
	PD3S,
	PD4S,
	PD5S,
	PD6S,
	PD7S,
	PD8S,
	PDVS,
	DT1S=381,
	DT2S,
	DT3S,
	DT4S,
	DT5S,
	DT6S,
	DT7S,
	DT8S,
	DT9S,
	DT10S,
	DT11S,
	DT12S,
	DT13S,
	DT14S,
	DT15S,
	DT16S,
	DT17S,
	DT18S,
	DT19S,
	DT20S,
	DT21S,
	DT22S,
	DT23S,
	DT24S,
	DT25S,
	DT26S,
	DT27S,
	DT28S,
	DTVS,
	IO1S=501,
	IO2S,
	IO3S,
	IO4S,
	IO5S,
	IO6S,
	IO7S,
	IO8S,
	IO9S,
	IO10S,
	IO11S,
	IO12S,
	IO13S,
	IO14S,
	IO15S,
	IO16S,
	IO17S,
	IO18S,
	IO19S,
	IO20S,
	IO21S,
	IO22S,
	IO23S,
	IO24S,
	IO25S,
	IO26S,
	IO27S,
	IO28S,
	IOVS,
};

enum DEV_ID
{
	CPU_ID=1,
	SSU_ID,
	MAU_ID,	
	PDU1_ID=8,
	PDU2_ID,
	PDU3_ID,
	PDU4_ID,
	PDU5_ID,
	PDU6_ID,
	PDU7_ID,
	PDU8_ID,	
	DET_IOU01_ID=97,
	DET_IOU02_ID,
	DET_IOU03_ID,
	DET_IOU04_ID,
	DET_IOU05_ID,
	DET_IOU06_ID,
	DET_IOU07_ID,
	DET_IOU08_ID,
	DET_IOU09_ID,
	DET_IOU10_ID,
	DET_IOU11_ID,
	DET_IOU12_ID,
	DET_IOU13_ID,
	DET_IOU14_ID,
	DET_IOU15_ID,
	DET_IOU16_ID,
	DET_IOU17_ID,
	DET_IOU18_ID,
	DET_IOU19_ID,
	DET_IOU20_ID,
	DET_IOU21_ID,
	DET_IOU22_ID,
	DET_IOU23_ID,
	DET_IOU24_ID,
	DET_IOU25_ID,
	DET_IOU26_ID,
	DET_IOU27_ID,
	DET_IOU28_ID,
};


void SendCanCommand(uint16_t nCmdNo);
void SSU_COMM(void);
void MAU_COMM(void);
void Send_Heartbeat(void);


#ifdef __cplusplus
}
#endif 
                                                                       /*  __cplusplus                 */
#endif

/********************************************************************************************************
**                                        End Of File                                                                             
********************************************************************************************************/
