/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:           spiEEPROM.h
** Last modified Date:  2009-03-09
** Last Version:        V1.0
** Descriptions:        Spi��SST25VF016B�Ĳ�������ͷ�ļ�
**
**------------------------------------------------------------------------------------------------------
** Created By:          MeiChengyu
** Created date:        2009-03-09
** Version: 1.0
** Descriptions:        First version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
** Rechecked by:
*********************************************************************************************************/

#ifndef __SPIEEPROM_H 
#define __SPIEEPROM_H

#ifdef __cplusplus
extern "C" {
#endif                                                                  /* __cplusplus                 */
#include "../LPC177x_8x/type.h"
#include "ssp.h"

/*  
** SPI EEPROM����(���CAT25256)  
 */
#define   SPI_EEPROM_WREN                  0x06                          /* write enable                 */
#define   SPI_EEPROM_WRDI                  0x04                          /* write disable                */
#define   SPI_EEPROM_RDSR                  0x05                          /* read status register         */
#define   SPI_EEPROM_WRSR                  0x01                          /* write status register        */
#define   SPI_EEPROM_READ                  0x03                          /* read data                    */
#define   SPI_EEPROM_WRITE                 0x02                          /* Write data               */


//#define   MAX_ADDR            0x7FFF                                  /* оƬ�ڲ�����ַ           ����    */
//#define   SPI_EEPROM_PageSize           64                                       /* ������С                  ����   */
#define   MAX_ADDR            0xFFFF                                  /* оƬ�ڲ�����ַ           CAT25512    */
#define   SPI_EEPROM_PageSize           128                                       /* ������С                 CAT25512    */

/*********************************************************************************************************
** �������� : SpiSimpleCmd
** �������� : ���ͼ�����
** ��ڲ��� : cmd ��Ҫ���͵�����
** ���ڲ��� : ��
*********************************************************************************************************/ 
void SpiSimpleCmd(uint8_t cmd);

/*********************************************************************************************************
** �������� : ReadSpiEEPROMID
** �������� : ��ȡEEPROMID
** ��ڲ��� : ��
** ���ڲ��� : id
*********************************************************************************************************/
//extern uint32_t ReadSpiEEPROMID(void);

/*********************************************************************************************************
** �������� : ReadSpiEEPROMStatus
** �������� : ��ȡEEPROM��״̬
** ��ڲ��� : ��
** ���ڲ��� : EEPROM��״̬
*********************************************************************************************************/
extern uint8_t ReadSpiEEPROMStatus(void);

/*********************************************************************************************************
** �������� : WriteSpiEEPROMStatus
** �������� : дEEPROM��״̬
** ��ڲ��� : status : д���״̬
** ���ڲ��� : ��
*********************************************************************************************************/
extern void WriteSpiEEPROMStatus(uint8_t status);

/*********************************************************************************************************
** �������� : ReadSpiEEPROM
** �������� : ��ȡEEPROM�е�����
** ��ڲ��� : addr    ����ַ��ʼ��ַ
              dst     �����ݴ�ŵĵ�ַ
              num     : ��ȡ���ݳ���
              readmode: ��ģʽ��0x03����ͨ����0x08�����ٶ�
** ���ڲ��� : TRUE/FALSE
*********************************************************************************************************/
uint8_t ReadSpiEEPROM(uint16_t addr, 
                   uint8_t *dst, 
                   uint16_t num);

uint8_t FM_BUFRD(uint16_t addr, 
                   uint8_t *dst, 
                   uint16_t num);

void SPI_EE_WRITE(uint16_t WriteAddr,
				 uint8_t* pBuffer, 
				 uint16_t NumByteToWrite);
				 
void FM_WRITE(uint16_t WriteAddr,
				 uint8_t* pBuffer, 
				 uint16_t NumByteToWrite);				 
				 
				 
void FM_WRITE_ZERO(uint16_t addr, 
					 uint16_t num);

uint32_t FM_RDLONG(uint16_t addr);

void SPI_EE_WRITESingle(uint16_t addr,uint8_t data);
uint8_t SPI_EE_READSingle(uint16_t addr);
uint8_t FM_RD(uint16_t addr);


/*********************************************************************************************************
** �������� : EraseSpiEEPROMChip
** �������� : ��Ƭ����SPI NorEEPROM
** ��ڲ��� : ��
** ���ڲ��� : ��
*********************************************************************************************************/
//extern void EraseSpiEEPROMChip(void);

/*********************************************************************************************************
** �������� : ProgramSpiEEPROMPage
** �������� : ���EEPROM
** ��ڲ��� : addr    ����ַ��ʼ��ַ
              src     ��Դ���ݴ�ŵĵ�ַ
              num     : ��ȡ���ݳ���
** ���ڲ��� : TRUE/FALSE
*********************************************************************************************************/
extern uint8_t ProgramSpiEEPROMPage(uint32_t addr, uint8_t *src, uint32_t num);

/*********************************************************************************************************
** �������� : CProgramSpiEEPROM
** �������� : �������EEPROM
** ��ڲ��� : addr    ����ַ��ʼ��ַ
              src     ��Դ���ݴ�ŵĵ�ַ
              num     : ������ݳ���
** ���ڲ��� : TRUE/FALSE
*********************************************************************************************************/
extern uint8_t CProgramSpiEEPROM(uint32_t addr, uint8_t *src, uint32_t num);

#ifdef __cplusplus
    }
#endif                                                                  /*  __cplusplus                 */
#endif
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
