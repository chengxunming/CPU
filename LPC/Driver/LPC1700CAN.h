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
**               C++�������                                                                            
*********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */

/*********************************************************************************************************
**                    Fpclk=48MHz CAN��׼������ֵ                                                       
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
**                    Fpclk=60MHz CAN��׼������ֵ                                                       
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


#define COMMAND_ENABLE			0xAA				//���Ϳ���ʹ������
#define COMMAND_DISABLE			0x55				//���Ϳ��Ƴ�������



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
**   ���� CAN �����շ��� �ṹ��:
*********************************************************************************************************/
typedef struct _MessageDetail
{
    unsigned char LEN;
    unsigned char FF;                                                   /* �Ƿ��׼֡                   */
    unsigned int CANID;                                                 /* ������FF�й�                 */
    unsigned int DATAA;
    unsigned int DATAB;
}MessageDetail;



/*********************************************************************************************************
**   ���� CANMOD �Ĵ���:
1.This register controls the basic operating mode of the CAN Controller. 
2.Bits not listed read as 0 and should be written as 0.
*********************************************************************************************************/
typedef union   _canmod_
{
    unsigned long DWord;
    struct {
        unsigned int    RM      :1;                                     /* ����RMλ(��λλ)             */
        unsigned int    LOM     :1;                                     /*����LOMλ(ֻ��ģʽλ)         */

        /*
        * ����STMλ "0"���͵���Ϣ���뱻Ӧ��ű��Ͽ�
        * "1"�Բ���ģʽ,CANCMR��SRRλһ��ʹ�ã����Դﵽ�Է����յ�Ч��
        */
        unsigned int    STM     :1; 
        /*
         * ����TPMλ "0"3�����ͻ����������ȼ��ɸ��Ե�CAN ID����
         * "1"3�����ͻ����������ȼ��ɸ��Ե�Tx���ȼ������
         */
        unsigned int    TPM     :1;
                
        unsigned int    SM      :1;                                     /* ����SMλ(˯��ģʽλ)         */

        unsigned int    RPM     :1;
        /*
        * ����RPMλ "0"��������������λ��Rx��Tx��Ϊ�͵�ƽ
        * "1"��ת����ģʽ--��������������λ��Rx��Ϊ�ߵ�ƽ
        */
        unsigned int    RSV1    :1;                                     /* ����λ                       */
        unsigned int    TM      :1;                                     /* ����TMλ(����ģʽ)           */
        unsigned int    RSV24   :24;                                    /* ����λ                       */
    }Bits;
}REG_CANMOD;

/*********************************************************************************************************
**   ���� CAN ֡�ṹ��:
*********************************************************************************************************/
typedef struct _CANFRMAE
{
    unsigned long can_id;                                                   /* ֡ID                         */ 
    unsigned char can_dlc;                                                  /* ���ݳ���                     */
    unsigned char RTR;                                                  /* Զ��֡��־                   */
    unsigned char FF;                                                   /* ��չ֡��־                   */
    unsigned char data[8];                                              /* ֡����                       */
}CANFRAME;

/*********************************************************************************************************
** Function name:  	unsigned char Enter_SWRst(unsigned char Ch);
** Descriptions:  	Ch:CAN�������ţ�0��ʾ��һ·CAN,CAN���������������λģʽ
** Returned value:      0��          ʧ��
**                      1:           �ɹ�	
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char Enter_SWRst(unsigned char Ch);

/**********************************************************************************************************
** Function name:       unsigned char Quit_SWRst(unsigned char Ch);
** Descriptions:        Ch:CAN�������ţ�0��ʾ��һ·CAN,CAN�������˳������λģʽ
** Returned value:      0��          ʧ��
**                      1:           �ɹ�	
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char Quit_SWRst(unsigned char Ch);

/*********************************************************************************************************
** Function name:   	unsigned char CAN_Init(unsigned char Ch, unsigned long Baud);
** Descriptions:        CAN�������˳������λģʽ
** input parameters:    Ch:          CAN�������ţ�0��ʾ��һ·CAN
**                      Baud:        CAN������ֵ
** Returned value:      ��
*********************************************************************************************************/
CANAPP_GLOBAL void CAN_Init(unsigned char Ch, unsigned long Baud);

/*********************************************************************************************************
** Function name:  	unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int 
**                      ID, unsigned char *data);
** Descriptions: 	�û���д���� ֡��Ϣ
** input parameters:    LEN:         ���ݳ���
**                      *data:       �����������ڵ����� 
** Returned value:      �����ɹ����� 1 ʧ�ܷ��� 0
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char writedetail(unsigned char LEN,unsigned char FF,
                                        unsigned int ID,unsigned char *data);

/*********************************************************************************************************
** Function name: 	unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);
** Descriptions:        CAN�������˳������λģʽ
** input parameters:    Ch:CAN�������ţ�0��ʾ��һ·CAN
**                       BUFNum ѡ�񻺳���
                         MessageDetailT ���ĵ�������Ϣ������
** Returned value:��
*********************************************************************************************************/
CANAPP_GLOBAL unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);

//������
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
