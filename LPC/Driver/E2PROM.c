/*********************************Copyright (c)********************************
**                           Guangzou ZLG-MCU Development Co.,LTD.
**                                 graduate school
**                             http://www.zlgmcu.com
**
**--------------File Info------------------------------------------------------
** File name:           main.c
** Last modified Date:  2011-07-20
** Last Version:        V1.0
** Descriptions:        the main function
**
**-----------------------------------------------------------------------------
** Created by:          chenyicheng
** Created date:        2011-07-20
** Version:             V1.0
** Descriptions:        the main function
**
*******************************************************************************/
#include "../User_code/global.h"
#include "E2PROM.h"

volatile uint32_t end_of_rdwr;
volatile uint32_t end_of_prog;

/******************************************************************************
** Function name:		EEPROM_Init
**
** Descriptions:		Initialize eeprom
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void EEPROM_Init ( void ) 
{
  uint32_t regVal;

  LPC_EEPROM->PWRDWN = 0x0;
/* 
 * 设置时钟，EEPROM 需要一个375kHz的时钟。这个时钟有系统时钟分频获得。
 */
  regVal =( SystemFrequency / 375000 )-1;
  LPC_EEPROM->CLKDIV = regVal;
  regVal = LPC_EEPROM->CLKDIV;

  regVal  = ((((SystemFrequency / 1000000) * 15) / 1000) + 1);
  regVal |= (((((SystemFrequency / 1000000) * 55) / 1000) + 1) << 8);
  regVal |= (((((SystemFrequency / 1000000) * 35) / 1000) + 1) << 16);
  LPC_EEPROM->WSTATE = regVal;

  return;
}

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
void EEPROM_Write(uint16_t page_offset, 
                  uint16_t page_address, 
                  void* data, 
                  EEPROM_Mode_Type mode, 
                  uint32_t count)
{
    uint32_t i;

#ifdef __IAR_SYSTEMS_ICC__
#if (mode == MODE_8_BIT)
        uint8_t *tmp  = (uint8_t *)data;
#elif (mode == MODE_16_BIT)
        uint16_t *tmp = (uint16_t *)data;
#else
        uint32_t *tmp = (uint32_t *)data;
#endif
#endif

    LPC_EEPROM->INT_CLR_STATUS = ((1 << EEPROM_ENDOF_RW)|(1 << EEPROM_ENDOF_PROG));
    if(mode == MODE_16_BIT){
        if((page_offset & 0x01)!=0) while(1);
    }
    else if(mode == MODE_32_BIT){
        if((page_offset & 0x03)!=0) while(1);
    }
    LPC_EEPROM->ADDR = EEPROM_PAGE_OFFSET(page_offset);
                                                /* 设置写数据地址              */
    LPC_EEPROM->INT_CLR_STATUS = (1 << EEPROM_ENDOF_RW);
    for(i=0;i<count;i++)
    {
        if(mode == MODE_8_BIT){
            LPC_EEPROM->CMD = EEPROM_CMD_8_BIT_WRITE;
                                               /* 把数据写入页寄存器           */
#ifdef __IAR_SYSTEMS_ICC__
            LPC_EEPROM -> WDATA = *tmp;
            tmp++;
#else
            LPC_EEPROM -> WDATA = *(((uint8_t *)data)++);
#endif
            page_offset +=1;
        }
        if((page_offset >= EEPROM_PAGE_NUM)|(i==count-1)){
                                              /* 把数据写入EEPROM中           */
            LPC_EEPROM->INT_CLR_STATUS = (0x1 << EEPROM_ENDOF_PROG);
            LPC_EEPROM->ADDR = EEPROM_PAGE_ADRESS(page_address);
            LPC_EEPROM->CMD = EEPROM_CMD_ERASE_PRG_PAGE;
            while(!((LPC_EEPROM->INT_STATUS >> 28)&0x01));
                                              /* 等待写完成                   */
        }
        if(page_offset >= EEPROM_PAGE_NUM)
        {
            page_offset = 0;
            page_address +=1;
            LPC_EEPROM->ADDR =0;
            if(page_address > EEPROM_PAGE_NUM - 1) page_address = 0;
        }
    }
}

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
void EEPROM_Read(uint16_t page_offset, 
                 uint16_t page_address, 
                 void* data, 
                 EEPROM_Mode_Type mode,
                 uint32_t count)
{
    uint32_t i;
#ifdef __IAR_SYSTEMS_ICC__
#if (mode == MODE_8_BIT)
        uint8_t *tmp  = (uint8_t *)data;
#elif (mode == MODE_16_BIT)
        uint16_t *tmp = (uint16_t *)data;
#else
        uint32_t *tmp = (uint32_t *)data;
#endif
#endif
    LPC_EEPROM->INT_CLR_STATUS = ((1 << EEPROM_ENDOF_RW)|(1 << EEPROM_ENDOF_PROG));
    LPC_EEPROM->ADDR = EEPROM_PAGE_ADRESS(page_address)|EEPROM_PAGE_OFFSET(page_offset);
    if(mode == MODE_8_BIT)
        LPC_EEPROM->CMD = EEPROM_CMD_8_BIT_READ|EEPROM_CMD_RDPREFETCH;
    else if(mode == MODE_16_BIT){
        LPC_EEPROM->CMD = EEPROM_CMD_16_BIT_READ|EEPROM_CMD_RDPREFETCH;
        if((page_offset &0x01)!=0)         /* page_offset必须是2的整数倍      */
            while(1);
    }
    else{
        LPC_EEPROM->CMD = EEPROM_CMD_32_BIT_READ|EEPROM_CMD_RDPREFETCH;
        if((page_offset & 0x03)!=0)        /* page_offset必须是4的整数倍      */
            while(1);
    }

    for(i=0;i<count;i++){                  /* 读EEPROM中的数据                */
        LPC_EEPROM->INT_CLR_STATUS = (1 << EEPROM_ENDOF_RW);
        if(mode == MODE_8_BIT){            /* 以8位的模式读数据               */
#ifdef __IAR_SYSTEMS_ICC__
            *tmp = (uint8_t)(LPC_EEPROM -> RDATA);
            tmp++;
#else
            *(((uint8_t *)data)++) = (uint8_t)(LPC_EEPROM -> RDATA);
#endif
            page_offset +=1;
        }
        else if (mode == MODE_16_BIT){     /* 以16位的模式读数据              */
#ifdef __IAR_SYSTEMS_ICC__
            *tmp =  (uint16_t)(LPC_EEPROM -> RDATA);
            tmp++;
#else
            *(((uint16_t *)data)++) = (uint16_t)(LPC_EEPROM -> RDATA);
#endif
            page_offset +=2;
        }
        else{                             /* 以32位的模式读数据              */
#ifdef __IAR_SYSTEMS_ICC__
            *tmp = (uint32_t)(LPC_EEPROM ->RDATA);
            tmp++;
#else
            *(((uint32_t *)data)++) = (uint32_t)(LPC_EEPROM ->RDATA);
#endif
            page_offset +=4;
        }
                                          /* 等待读完成                     */
        while(!((LPC_EEPROM->INT_STATUS >> 26)&0x01));
        if(page_offset >= EEPROM_PAGE_SIZE) {
            page_offset = 0;
            page_address++;
            LPC_EEPROM->ADDR = EEPROM_PAGE_ADRESS(page_address)|EEPROM_PAGE_OFFSET(page_offset);
            if(mode == MODE_8_BIT)
                LPC_EEPROM->CMD = EEPROM_CMD_8_BIT_READ|EEPROM_CMD_RDPREFETCH;
            else if(mode == MODE_16_BIT)
                LPC_EEPROM->CMD = EEPROM_CMD_16_BIT_READ|EEPROM_CMD_RDPREFETCH;
            else
                LPC_EEPROM->CMD = EEPROM_CMD_32_BIT_READ|EEPROM_CMD_RDPREFETCH;
        }
    }
}
/******************************************************************************
**                            End Of File
******************************************************************************/
