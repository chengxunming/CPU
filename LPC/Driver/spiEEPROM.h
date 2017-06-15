/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:           spiEEPROM.h
** Last modified Date:  2009-03-09
** Last Version:        V1.0
** Descriptions:        Spi下SST25VF016B的操作函数头文件
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
** SPI EEPROM命令(针对CAT25256)  
 */
#define   SPI_EEPROM_WREN                  0x06                          /* write enable                 */
#define   SPI_EEPROM_WRDI                  0x04                          /* write disable                */
#define   SPI_EEPROM_RDSR                  0x05                          /* read status register         */
#define   SPI_EEPROM_WRSR                  0x01                          /* write status register        */
#define   SPI_EEPROM_READ                  0x03                          /* read data                    */
#define   SPI_EEPROM_WRITE                 0x02                          /* Write data               */


//#define   MAX_ADDR            0x7FFF                                  /* 芯片内部最大地址           铁电    */
//#define   SPI_EEPROM_PageSize           64                                       /* 扇区大小                  铁电   */
#define   MAX_ADDR            0xFFFF                                  /* 芯片内部最大地址           CAT25512    */
#define   SPI_EEPROM_PageSize           128                                       /* 扇区大小                 CAT25512    */

/*********************************************************************************************************
** 函数名称 : SpiSimpleCmd
** 函数功能 : 发送简单命令
** 入口参数 : cmd ：要发送的命令
** 出口参数 : 无
*********************************************************************************************************/ 
void SpiSimpleCmd(uint8_t cmd);

/*********************************************************************************************************
** 函数名称 : ReadSpiEEPROMID
** 函数功能 : 读取EEPROMID
** 入口参数 : 无
** 出口参数 : id
*********************************************************************************************************/
//extern uint32_t ReadSpiEEPROMID(void);

/*********************************************************************************************************
** 函数名称 : ReadSpiEEPROMStatus
** 函数功能 : 读取EEPROM的状态
** 入口参数 : 无
** 出口参数 : EEPROM的状态
*********************************************************************************************************/
extern uint8_t ReadSpiEEPROMStatus(void);

/*********************************************************************************************************
** 函数名称 : WriteSpiEEPROMStatus
** 函数功能 : 写EEPROM的状态
** 入口参数 : status : 写入的状态
** 出口参数 : 无
*********************************************************************************************************/
extern void WriteSpiEEPROMStatus(uint8_t status);

/*********************************************************************************************************
** 函数名称 : ReadSpiEEPROM
** 函数功能 : 读取EEPROM中的数据
** 入口参数 : addr    ：地址开始地址
              dst     ：数据存放的地址
              num     : 读取数据长度
              readmode: 读模式，0x03：普通读，0x08：快速读
** 出口参数 : TRUE/FALSE
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
** 函数名称 : EraseSpiEEPROMChip
** 函数功能 : 整片擦除SPI NorEEPROM
** 入口参数 : 无
** 出口参数 : 无
*********************************************************************************************************/
//extern void EraseSpiEEPROMChip(void);

/*********************************************************************************************************
** 函数名称 : ProgramSpiEEPROMPage
** 函数功能 : 编程EEPROM
** 入口参数 : addr    ：地址开始地址
              src     ：源数据存放的地址
              num     : 读取数据长度
** 出口参数 : TRUE/FALSE
*********************************************************************************************************/
extern uint8_t ProgramSpiEEPROMPage(uint32_t addr, uint8_t *src, uint32_t num);

/*********************************************************************************************************
** 函数名称 : CProgramSpiEEPROM
** 函数功能 : 连续编程EEPROM
** 入口参数 : addr    ：地址开始地址
              src     ：源数据存放的地址
              num     : 编程数据长度
** 出口参数 : TRUE/FALSE
*********************************************************************************************************/
extern uint8_t CProgramSpiEEPROM(uint32_t addr, uint8_t *src, uint32_t num);

#ifdef __cplusplus
    }
#endif                                                                  /*  __cplusplus                 */
#endif
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
