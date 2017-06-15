/********************************Copyright (c)*************************************
**                         Guangzou ZLG-MCU Development Co.,LTD.
**                               graduate school
**                            http://www.zlgmcu.com
**
**--------------File Info----------------------------------------------------------
** File name:           main.c
** Last modified Date:  2011-07-20
** Last Version:        V1.0
** Descriptions:        the main function
**
**----------------------------------------------------------------------------------
** Created by:          chenyicheng
** Created date:        2011-07-20
** Version:             V1.0
** Descriptions:        the main function
**
************************************************************************************/
#ifndef __EEPROM_H 
#define __EEPROM_H

#include "../LPC177x_8x/LPC177x_8x.h"
#include "../LPC177x_8x/type.h"
#include "../LPC177x_8x/system_LPC177x_8x.h"
/************************************************************************************
添加用户宏定义
************************************************************************************/
#define READ8               0
#define READ16              1
#define READ32              2
#define WRITE8              3
#define WRITE16             4
#define WRITE32             5
#define ERASE_PROG          6
#define RD_PFETCH           (1<<3)

#define END_OF_RDWR         26
#define END_OF_PROG         28

#define RDPREFETCH          1
#define STALLED             1


#define EEPROM_CMD_8_BIT_READ           (0)
#define EEPROM_CMD_16_BIT_READ          (1)
#define EEPROM_CMD_32_BIT_READ          (2)
#define EEPROM_CMD_8_BIT_WRITE          (3)
#define EEPROM_CMD_16_BIT_WRITE         (4)
#define EEPROM_CMD_32_BIT_WRITE         (5)
#define EEPROM_CMD_ERASE_PRG_PAGE       (6)
#define EEPROM_CMD_RDPREFETCH           (1<<3)
#define EEPROM_PAGE_SIZE                64
#define EEPROM_PAGE_NUM                 64
#define EEPROM_PWRDWN                   (1<<0)
#define EEPROM_ENDOF_RW                 (26)
#define EEPROM_ENDOF_PROG               (28)

#define EEPROM_PAGE_OFFSET(n)           (n&0x3F)
#define EEPROM_PAGE_ADRESS(n)           ((n&0x3F)<<6)

typedef enum                                        /* EEPROM操作模式                */
{
    MODE_8_BIT = 0,
    MODE_16_BIT,
    MODE_32_BIT
}EEPROM_Mode_Type;

/******************************************************************************
** Function name:		EEPROM_Init
**
** Descriptions:		Initialize eeprom
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
extern void EEPROM_Init (void);

/******************************************************************************
** Function name:		EEPROM_Write
**
** Descriptions:		Write eeprom
**
** parameters:			page_offset ：页数据地址
**                      page_address：页地址
**                      data        ：要写入的数据
**                      EEPROM_Mode_Type：写入的模式
**                      count       ：写入的数据个数
** Returned value:		None
** 
******************************************************************************/
extern void EEPROM_Write(uint16_t page_offset, 
                         uint16_t page_address, 
                         void* data, 
                         EEPROM_Mode_Type mode, 
                         uint32_t count);

/******************************************************************************
** Function name:		EEPROM_Read
**
** Descriptions:		Read eeprom
**
** parameters:			page_offset ：页数据地址
**                      page_address：页地址
**                      data        ：读出的数据
**                      EEPROM_Mode_Type：读的模式
**                      count       ：读出的数据个数
** Returned value:		None
** 
******************************************************************************/
extern void EEPROM_Read(uint16_t page_offset, 
                        uint16_t page_address, 
                        void* data, 
                        EEPROM_Mode_Type mode, 
                        uint32_t count);

#endif 
/*****************************************************************************
**                            End Of File
******************************************************************************/
