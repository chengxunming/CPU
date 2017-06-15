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
** Descriptions:               LPC17ϵ��оƬӲ��I2C�������������Ҫ���ú�I2C���߽ӿ�(I2C���Ź��ܺ�
**                             I2C�жϣ�����ʹ��I2C��ģʽ)
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



extern  uint8_t	FailTWI[];				    //����ģ��

//PCU
extern	uint8_t	TPCUSize;					//MCU������PCU���ݳ���
extern	uint8_t	RPCUSize;					//MCU����PCU���ݳ���
extern	uint8_t	TPCU[];						//MCU������PCU��������
extern	signed char	RPCU[];					//MCU����PCU��������
//extern	struct PCU;						//PCUͨѶ�ṹ������

//EIU
//extern	typedef struct TWIEIU TEIU;						//EIUͨѶ�ṹ������

//MCM
extern	uint8_t	TMCMSize;					//MCM
extern	uint8_t	RMCMSize;
extern 	uint8_t	TMCM[];
extern	uint8_t	RMCM[];
//extern	MCM;						//MCMЭ�����

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        �����ӵ�ַ��������1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t  ISendByte(uint8_t sla, uint8_t dat);

/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        �����ӵ�ַ������ȡ1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t  IRcvByte(uint8_t sla, uint8_t *dat);

/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        �����ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ���ݽ��ջ�����ָ��
**                      num         ��ȡ�ĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t I2C_ReadNByte (uint8_t sla, INT32U suba_type, INT32U suba, uint8_t *s, INT32U num);

/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        �����ӵ�ַ����д��N�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ��Ҫд������ݵ�ָ��
**                      num         ��Ҫд������ݵĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
extern uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, INT32U suba, uint8_t *s, INT32U num);

#endif

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
