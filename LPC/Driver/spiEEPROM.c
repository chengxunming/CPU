/****************************************Copyright (c)****************************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           spiEEPROM.c
** Last modified Date:  2014-02-27
** Last Version:        V1.0
** Descriptions:        SPI EEPROM,���磬FLASHͨ����������
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
  �궨��
*********************************************************************************************************/
#define     SPI_EEPROM_ENABLE()         LPC_GPIO0->CLR |= (1UL<<6);
#define     SPI_EEPROM_DISABLE()        LPC_GPIO0->SET |= (1UL<<6);



 

/*********************************************************************************************************
** �������� : SpiReadByte
** �������� ����SPI�ӿڽ���һ�ֽ�����
** ��ڲ��� : ��
** ���ڲ��� : �������ֽ�����
*********************************************************************************************************/
uint8_t SpiReadByte(void)
{
    uint8_t  dat = 0;

    SSPReceive(1, &dat, 1);
     
    return dat;  
}

/*********************************************************************************************************
** �������� : SpiWrite
** �������� ����SPI�ӿڷ���һ�ֽ�����
** ��ڲ��� : dat ��д�������
** ���ڲ��� : ��
*********************************************************************************************************/
uint32_t SpiWrite(uint8_t *buff, uint32_t num)
{
    SSPSend(1, buff, num);
    
    return num;
}

/*********************************************************************************************************
** �������� : SpiSimpleCmd
** �������� : ���ͼ�����
** ��ڲ��� : cmd ��Ҫ���͵�����
** ���ڲ��� : ��
*********************************************************************************************************/ 
void SpiSimpleCmd(uint8_t cmd)
{
    SPI_EEPROM_ENABLE();
	__NOP();
    SpiWrite(&cmd, 1); 
    SPI_EEPROM_DISABLE();
}    


/*********************************************************************************************************
** �������� : ReadSpiEEPROMStatus
** �������� : ��ȡEEPROM��״̬
** ��ڲ��� : ��
** ���ڲ��� : EEPROM��״̬
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
** �������� : WriteSpiEEPROMStatus
** �������� : дEEPROM��״̬
** ��ڲ��� : status : д���״̬
** ���ڲ��� : ��
*********************************************************************************************************/
void WriteSpiEEPROMStatus(uint8_t status)
{
    uint8_t dataout[2];
    
    SpiSimpleCmd(SPI_EEPROM_WREN);                                      /*  ����дʹ���ź�               */ 
    
    dataout[0] = SPI_EEPROM_WRSR;
    dataout[1] = status;
    
    SPI_EEPROM_ENABLE();
    SpiWrite(dataout, 2);    
    SPI_EEPROM_DISABLE();
}



/*********************************************************************************************************
** �������� : ReadSpiEEPROM
** �������� : ��ȡEEPROM�е�����
** ��ڲ��� : addr    ����ַ��ʼ��ַ
              dst     �����ݴ�ŵĵ�ַ
              num     : ��ȡ���ݳ���
** ���ڲ��� : TRUE/FALSE
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
** �������� : SPI_EEPROM_PageWrite
** �������� : ���EEPROM
** ��ڲ��� : addr    ����ַ��ʼ��ַ
              src     ��Դ���ݴ�ŵĵ�ַ
              num     : ������ݳ���
** ���ڲ��� : TRUE/FALSE
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
    
    SpiSimpleCmd(SPI_EEPROM_WREN);                                      /*  ����дʹ���ź�               */
       
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
    while ((ReadSpiEEPROMStatus() & 0x01) == 0x01);                      /*  �ȴ��������� EEPROM��FLASH��Ҫ               */  
	
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


/*д0*/
void FM_WRITE_ZERO(uint16_t addr, uint16_t num)
{
	uint8_t zerotemp[4096];
	memset(zerotemp,0,4096);
	
	SPI_EE_WRITE(addr,zerotemp,num);
		
}

//��ȡ�̶���ַ4�ֽڣ�ULONG)����8λ���ݵ�ַ��ǰ
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


/************дһ���ֽ�*********************/
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
	while ((ReadSpiEEPROMStatus() & 0x01) == 0x01);                      /*  �ȴ��������� EEPROM��FLASH��Ҫ�ò���               */	
}

/************��һ���ֽ�*********************/
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
