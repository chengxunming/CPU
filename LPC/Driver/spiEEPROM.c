/****************************************Copyright (c)****************************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           spiEEPROM.c
** Last modified Date:  2014-02-27
** Last Version:        V1.0
** Descriptions:        SPI EEPROM,铁电，FLASH通用驱动函数
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
** Rechecked by:

*********************************************************************************************************/

#include "../User_code/global.h"

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define     SPI_EEPROM_ENABLE()         LPC_GPIO0->CLR |= (1UL<<6);
#define     SPI_EEPROM_DISABLE()        LPC_GPIO0->SET |= (1UL<<6);



 

/*********************************************************************************************************
** 函数名称 : SpiReadByte
** 函数功能 ：从SPI接口接收一字节数据
** 入口参数 : 无
** 出口参数 : 读出的字节数据
*********************************************************************************************************/
uint8_t SpiReadByte(void)
{
    uint8_t  dat = 0;

    SSPReceive(1, &dat, 1);
     
    return dat;  
}

/*********************************************************************************************************
** 函数名称 : SpiWrite
** 函数功能 ：从SPI接口发送一字节数据
** 入口参数 : dat ：写入的数据
** 出口参数 : 无
*********************************************************************************************************/
uint32_t SpiWrite(uint8_t *buff, uint32_t num)
{
    SSPSend(1, buff, num);
    
    return num;
}

/*********************************************************************************************************
** 函数名称 : SpiSimpleCmd
** 函数功能 : 发送简单命令
** 入口参数 : cmd ：要发送的命令
** 出口参数 : 无
*********************************************************************************************************/ 
void SpiSimpleCmd(uint8_t cmd)
{
    SPI_EEPROM_ENABLE();
	__NOP();
    SpiWrite(&cmd, 1); 
    SPI_EEPROM_DISABLE();
}    


/*********************************************************************************************************
** 函数名称 : ReadSpiEEPROMStatus
** 函数功能 : 读取EEPROM的状态
** 入口参数 : 无
** 出口参数 : EEPROM的状态
*********************************************************************************************************/
uint8_t ReadSpiEEPROMStatus(void)
{
    uint8_t EEPROM_Status=0,datain[2];
    
    
    SPI_EEPROM_ENABLE();
	datain[0]= SPI_EEPROM_RDSR;
	datain[1]= 0xFF;
    
    SpiWrite(datain, 2);
	for(uint8_t i=0; i < 2; i++) {
        datain[i] = SpiReadByte();
    }
    
    SPI_EEPROM_DISABLE();
	EEPROM_Status=datain[1];

    return EEPROM_Status;
}

/*********************************************************************************************************
** 函数名称 : WriteSpiEEPROMStatus
** 函数功能 : 写EEPROM的状态
** 入口参数 : status : 写入的状态
** 出口参数 : 无
*********************************************************************************************************/
void WriteSpiEEPROMStatus(uint8_t status)
{
    uint8_t dataout[2];
    
    SpiSimpleCmd(SPI_EEPROM_WREN);                                      /*  发送写使能信号               */ 
    
    dataout[0] = SPI_EEPROM_WRSR;
    dataout[1] = status;
    
    SPI_EEPROM_ENABLE();
    SpiWrite(dataout, 2);    
    SPI_EEPROM_DISABLE();
}



/*********************************************************************************************************
** 函数名称 : ReadSpiEEPROM
** 函数功能 : 读取EEPROM中的数据
** 入口参数 : addr    ：地址开始地址
              dst     ：数据存放的地址
              num     : 读取数据长度
** 出口参数 : TRUE/FALSE
*********************************************************************************************************/
uint8_t ReadSpiEEPROM(uint16_t addr, 
                   uint8_t *dst, 
                   uint16_t num)
{
    uint8_t  dataout[4];

    if (addr > MAX_ADDR) 
	{
        return FALSE;
    }

    dataout[0] = SPI_EEPROM_READ;
	dataout[1] = (uint8_t)(addr >>  8); 
    dataout[2] = (uint8_t)(addr >>  0); 
    
    
    SPI_EEPROM_ENABLE();
    
    SpiWrite(dataout, 3);

    while (num-- > 0) {
        *dst++= SpiReadByte();
    }
    
    SPI_EEPROM_DISABLE();

    return TRUE;
}

uint8_t FM_BUFRD(uint16_t addr, 
                   uint8_t *dst, 
                   uint16_t num)
{
    uint8_t  dataout[4];

    if (addr > MAX_ADDR) 
	{
        return FALSE;
    }

    dataout[0] = SPI_EEPROM_READ;
	dataout[1] = (uint8_t)(addr >>  8); 
    dataout[2] = (uint8_t)(addr >>  0); 
    
    
    SPI_EEPROM_ENABLE();
    
    SpiWrite(dataout, 3);

    while (num-- > 0) {
        *dst++= SpiReadByte();
    }
    
    SPI_EEPROM_DISABLE();

    return TRUE;
}

/*********************************************************************************************************
** 函数名称 : SPI_EEPROM_PageWrite
** 函数功能 : 编程EEPROM
** 入口参数 : addr    ：地址开始地址
              src     ：源数据存放的地址
              num     : 编程数据长度
** 出口参数 : TRUE/FALSE
*********************************************************************************************************/
uint8_t SPI_EEPROM_PageWrite(uint16_t addr, uint8_t *src, uint16_t num)
{
    uint8_t  dataout[3];
    uint16_t count;
    
//    if ((addr > MAX_ADDR) || (num > 64)) {
//        return FALSE;
//    }
    if ((addr > MAX_ADDR) || (num > 128)) {
        return FALSE;
    }
    
    SpiSimpleCmd(SPI_EEPROM_WREN);                                      /*  发送写使能信号               */
       
    dataout[0] = SPI_EEPROM_WRITE;
	dataout[1] = (uint8_t)(addr >>  8); 
    dataout[2] = (uint8_t)(addr >>  0); 
  
    SPI_EEPROM_ENABLE(); 
	
	__NOP();

    SpiWrite(dataout, 3);

    while (num > 0){
        count = SpiWrite(src, num);
        num -= count;
        src += count;
    }    
   SPI_EEPROM_DISABLE();     
    while ((ReadSpiEEPROMStatus() & 0x01) == 0x01);                      /*  等待操作结束 EEPROM及FLASH需要               */  
	
	//SPI_EEPROM_DISABLE();
    
    return TRUE;
}


/*******************************************************************************
* Function Name  : FM_WRITE
* Description    : Writes block of data to the EEPROM. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_EE_WRITE(uint16_t WriteAddr,uint8_t* pBuffer, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_EEPROM_PageSize;
  count = SPI_EEPROM_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_EEPROM_PageSize;
  NumOfSingle = NumByteToWrite % SPI_EEPROM_PageSize;
  
  
  if(Addr == 0) /* WriteAddr is SPI_EEPROM_PageSize aligned  */
  {
    if(NumOfPage == 0) /* NumByteToWrite < SPI_EEPROM_PageSize */
    {
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer,NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_EEPROM_PageSize */ 
    {
      while(NumOfPage--)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, SPI_EEPROM_PageSize);
        WriteAddr +=  SPI_EEPROM_PageSize;
        pBuffer += SPI_EEPROM_PageSize;  
      }    
     
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumOfSingle);
   }
  }
  else /* WriteAddr is not SPI_EEPROM_PageSize aligned  */
  {
    if(NumOfPage== 0) /* NumByteToWrite < SPI_EEPROM_PageSize */
    {
      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_EEPROM_PageSize */
      {
        temp = NumOfSingle - count;
      
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, count);
        WriteAddr +=  count;
        pBuffer += count; 
        
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, temp);
      }
      else
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_EEPROM_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_EEPROM_PageSize;
      NumOfSingle = NumByteToWrite % SPI_EEPROM_PageSize;
      
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer,count);
      WriteAddr +=  count;
      pBuffer += count;  
     
      while(NumOfPage--)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, SPI_EEPROM_PageSize);
        WriteAddr +=  SPI_EEPROM_PageSize;
        pBuffer += SPI_EEPROM_PageSize;
      }
      
      if(NumOfSingle != 0)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumOfSingle);
      }
    }
  }
}


void FM_WRITE(uint16_t WriteAddr,uint8_t* pBuffer, uint16_t NumByteToWrite)
{	
	
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_EEPROM_PageSize;
  count = SPI_EEPROM_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_EEPROM_PageSize;
  NumOfSingle = NumByteToWrite % SPI_EEPROM_PageSize;
  
  if((WriteAddr<MAX_Config)&&(!ConfigDownLoadStartFlg))
  {
	 ReloadADDR = WriteAddr;
	 Reloadnum  = NumByteToWrite;
  }
  
  if(Addr == 0) // WriteAddr is SPI_EEPROM_PageSize aligned  
  {
    if(NumOfPage == 0) // NumByteToWrite < SPI_EEPROM_PageSize 
    {
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer,NumByteToWrite);
    }
    else // NumByteToWrite > SPI_EEPROM_PageSize 
    {
      while(NumOfPage--)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, SPI_EEPROM_PageSize);
        WriteAddr +=  SPI_EEPROM_PageSize;
        pBuffer += SPI_EEPROM_PageSize;  
      }    
     
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumOfSingle);
   }
  }
  else 
  {
    if(NumOfPage== 0)
    {
      if(NumOfSingle > count)
      {
        temp = NumOfSingle - count;
      
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, count);
        WriteAddr +=  count;
        pBuffer += count; 
        
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, temp);
      }
      else
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumByteToWrite);
      }
    }
    else 
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_EEPROM_PageSize;
      NumOfSingle = NumByteToWrite % SPI_EEPROM_PageSize;
      
      SPI_EEPROM_PageWrite(WriteAddr,pBuffer,count);
      WriteAddr +=  count;
      pBuffer += count;  
     
      while(NumOfPage--)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, SPI_EEPROM_PageSize);
        WriteAddr +=  SPI_EEPROM_PageSize;
        pBuffer += SPI_EEPROM_PageSize;
      }
      
      if(NumOfSingle != 0)
      {
        SPI_EEPROM_PageWrite(WriteAddr,pBuffer, NumOfSingle);
      }
    }
  }
}


/*写0*/
void FM_WRITE_ZERO(uint16_t addr, uint16_t num)
{
	uint8_t zerotemp[4096];
	memset(zerotemp,0,4096);
	
	SPI_EE_WRITE(addr,zerotemp,num);
		
}

//读取固定地址4字节（ULONG)，低8位数据地址在前
uint32_t FM_RDLONG(uint16_t addr)
{
	uint32_t data;
	uint8_t  dataout[3];
	if((addr<MAX_Config)&&(Loadconfig2ram))
	{
		 data = (uint32_t)Config[addr];
		 data |=(uint32_t)Config[addr+1]<<8;
		 data |=(uint32_t)Config[addr+2]<<16;
		 data |=(uint32_t)Config[addr+3]<<24;
	}
	else
	{
		SPI_EEPROM_ENABLE();
	
		dataout[1] =(uint8_t)(addr >> 8);
		dataout[2] =(uint8_t)(addr >> 0);
		dataout[0] =SPI_EEPROM_READ;
		SpiWrite(dataout,3);
		data	=	(uint32_t)SpiReadByte();	
		data	|=	((uint32_t)SpiReadByte()<<8);
		data	|=	((uint32_t)SpiReadByte()<<16);
		data	|=	((uint32_t)SpiReadByte()<<24);
		
		SPI_EEPROM_DISABLE();				
	}
   	
	return data;
}


/************写一个字节*********************/
void SPI_EE_WRITESingle(uint16_t addr,uint8_t data)
{
	uint8_t  dataout[4];
	SpiSimpleCmd(SPI_EEPROM_WREN);
	SPI_EEPROM_ENABLE();
	dataout[0] = SPI_EEPROM_WRITE;
	dataout[1] =(uint8_t)(addr >> 8);
	dataout[2] =(uint8_t)(addr >> 0);
	dataout[3] = data; 
	SpiWrite(dataout,4);	
	SPI_EEPROM_DISABLE();	
	while ((ReadSpiEEPROMStatus() & 0x01) == 0x01);                      /*  等待操作结束 EEPROM及FLASH需要该操作               */	
}

/************读一个字节*********************/
uint8_t SPI_EE_READSingle(uint16_t addr)
{
	uint8_t data;
	uint8_t  dataout[3];
	SPI_EEPROM_ENABLE();
	
	dataout[1] =(uint8_t)(addr >> 8);
	dataout[2] =(uint8_t)(addr >> 0);
	dataout[0] =SPI_EEPROM_READ;
	SpiWrite(dataout,3);
   	data=SpiReadByte();	
	SPI_EEPROM_DISABLE();
	return data;
}

uint8_t FM_RD(uint16_t addr)
{
	
	uint8_t data;
	uint8_t  dataout[3];
	if((addr<MAX_Config)&&(Loadconfig2ram))
	{
		data = Config[addr];
	}
	else
	{
		SPI_EEPROM_ENABLE();
	
		dataout[1] =(uint8_t)(addr >> 8);
		dataout[2] =(uint8_t)(addr >> 0);
		dataout[0] =SPI_EEPROM_READ;
		SpiWrite(dataout,3);
		data=SpiReadByte();	
		SPI_EEPROM_DISABLE();
	}
	return data;
}


/*********************************************************************************************************
  End File
*********************************************************************************************************/
