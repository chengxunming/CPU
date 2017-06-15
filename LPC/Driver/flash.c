/****************************************Copyright (c)****************************************************
** File name:                  flash.c
** Created by:                 Demos
** Created Date:               2016-6-15
** Version:                    V1.0
** Descriptions:                                            
**                             
*********************************************************************************************************/
#include "flash.h"
#include "iap.h"
#include "../User_code/global.h"
#include "spiEEPROM.h"

unsigned param_table[5];//传递参数列表
unsigned result_table[5];//返回结果列表
uint32_t addrCur = FLASH_APP1_ADDR;			//当前系统写入地址,每次写入之后地址增加2048

//uint16_t Max_bagnum;
uint16_t Update_BagNum;
//uint8_t Update_Lastbag_flag;
//uint8_t iapbuf[1024]; //用于缓存数据的数组
uint16_t iapbuf[1024]; //用于缓存数据的数组
//uint16_t Datalenth;


//定义系统时钟
unsigned cclk = CCLK;

//flash结束地址的一张表,在写入数据的时候有用
const unsigned sector_end_map[MAX_FLASH_SECTOR] = {SECTOR_0_END,SECTOR_1_END,    \
SECTOR_2_END,SECTOR_3_END,SECTOR_4_END,SECTOR_5_END,SECTOR_6_END,SECTOR_7_END,   \
SECTOR_8_END,SECTOR_9_END,SECTOR_10_END,SECTOR_11_END,SECTOR_12_END,             \
SECTOR_13_END,SECTOR_14_END,SECTOR_15_END,SECTOR_16_END,SECTOR_17_END,           \
SECTOR_18_END,SECTOR_19_END,SECTOR_20_END,SECTOR_21_END,SECTOR_22_END,           \
SECTOR_23_END,SECTOR_24_END,SECTOR_25_END,SECTOR_26_END,                         \
SECTOR_27_END,SECTOR_28_END,SECTOR_29_END										 };

//关闭系统总中断
__asm void NVIC_SETPRIMASK(void) 
{
  CPSID i
	BX r14
}

//打开系统总中断
__asm void NVIC_RESETPRIMASK(void) 
{
  CPSIE i
	BX r14
}
/*********************************************************************************************************
** 函数名称：iap_entry
** 函数描述：调用iap命令
** 输入参数：param_tab[]     指令组 
**           result_tab[]    结果存储数组
** 返回值  ：
** 备注    ：
*********************************************************************************************************/
void iap_entry(unsigned param_tab[],unsigned result_tab[])
{
  void (*iap)(unsigned [],unsigned []);

  iap = (void (*)(unsigned [],unsigned []))IAP_ENTER_ADR;
  iap(param_tab,result_tab);
}
/*********************************************************************************************************
** 函数名称：Flash_dowloads
** 函数描述：Flash数据下载存储
** 输入参数：
** 返回值  ：
** 备注    ：
*********************************************************************************************************//*
void Flash_dowloads(void){
	uint8_t num1,num2,value;
  addrCur = FLASH_APP1_ADDR + (Update_BagNum-1)*512;
	if(!FM_RD(Flash_Clear_Flag)){
		return;
		//删除失败无法写入；
	}
	if(Update_Lastbag_flag){
		if(write_flash(100000,addrCur,(unsigned*)iapbuf,Datalenth-4)){
			
//			SPI_EE_WRITESingle(Flash_Clear_Flag,0x00);
		 	Flash_write_flag = 0;
//			Update_FVER_flag = 0;
//			RESET_FLAG=1;
   //写入失败，等待重写
			return;
		}
		else{
			SPI_EE_WRITESingle(Flash_Clear_Flag,0x00);
		 	Flash_write_flag = 0;
			Update_FVER_flag = 0;
			RESET_FLAG=1;
			addrCur=FLASH_APP1_ADDR;
		}
	 //写完之后Flash_write_flag要清零等待下一次传输
   //Flash_write_flag = 0;
	}
  else if(Datalenth == 516){
	 //写入flash中
	  if(write_flash(100000,addrCur,(unsigned*)iapbuf,512)){
   //receiveMode = 0;
			Flash_write_flag = 0;
		  return;					
	  }
		else{
			Flash_write_flag = 0;
			
		  num1=Update_BagNum % 8;                             //余数，第几位置位
		  num2=Update_BagNum / 8;                             //第几个字节置位
		  value = FM_RD(Flash_Clear_Flag1+num2);
		  value |= (0x01<<(num1-1));
		  SPI_EE_WRITESingle((Flash_Clear_Flag+num2),value);
		}
//写完之后Flash_write_flag要清零等待下一次传输
//	  Flash_write_flag = 0;
  }
	else{                                                    
		Flash_write_flag = 0;	                               //数据长度错误，不写直接返回
		return;
	}
}*/
/*********************************************************************************************************
** 函数名称：prepare_sector
** 函数描述：扇区准备好指令
** 输入参数：start_sector    起始扇区号 
**           end_sector      结束扇区号
**           cclk            系统时钟 
** 返回值  ：
** 备注    ：
*********************************************************************************************************/
void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk)
{
  param_table[0] = PREPARE_SECTOR_FOR_WRITE;
  param_table[1] = start_sector;
  param_table[2] = end_sector;
  param_table[3] = cclk;
  iap_entry(param_table,result_table);
}
/*********************************************************************************************************
** 函数名称：erase_sector
** 函数描述：擦除扇区指令
** 输入参数：start_sector    起始扇区号 
**           end_sector      结束扇区号
**           cclk            系统时钟 
** 返回值  ：
** 备注    ：
*********************************************************************************************************/
void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk)
{
  param_table[0] = ERASE_SECTOR;
  param_table[1] = start_sector;
  param_table[2] = end_sector;
  param_table[3] = cclk;
  iap_entry(param_table,result_table);
}

/*********************************************************************************************************
** 函数名称：erase_user_flash
** 函数描述：擦除用户代码空间
** 输入参数：
** 返回值  ：成功返回0 失败返回1
** 备注    ：
*********************************************************************************************************/
uint8_t erase_user_flash(void)
{
  unsigned i;
/*	unsigned MAX_SECTOR;
	uint8_t j1,j2;
	
	j1 = Max_bagnum%8;
	j2 = Max_bagnum/8;
  if(j1>0){
		j2++;
	}
	if(j2<MAX_USER_SECTOR){
		MAX_SECTOR=j2;
	}
	else MAX_SECTOR=MAX_USER_SECTOR;*/
	
	NVIC_SETPRIMASK();//关闭系统中断
	//一个一个扇区擦过来
//  for(i=USER_START_SECTOR;i<=MAX_SECTOR;i++){
  for(i=USER_START_SECTOR;i<=MAX_USER_SECTOR;i++)
	{
		prepare_sector(i,i,cclk);
		erase_sector(i,i,cclk);
		if(result_table[0] != CMD_SUCCESS)
		{
		  return 1;
		}
  }
	NVIC_RESETPRIMASK();//打开系统中断
	return 0;//成功返回0
}
/*********************************************************************************************************
** 函数名称：erase_sector_flash
** 函数描述：擦除指定扇区
** 输入参数：i          扇区号
** 返回值  ：成功返回0 失败返回1
** 备注    ：
*********************************************************************************************************/
uint8_t erase_sector_flash(unsigned i)
{
	
	NVIC_SETPRIMASK();//关闭系统中断
	//扇区擦除
	prepare_sector(i,i,cclk);
	erase_sector(i,i,cclk);
	if(result_table[0] != CMD_SUCCESS)
	{
	  return 1;
	}
	NVIC_RESETPRIMASK();//打开系统中断
	return 0;//成功返回0
}

/*********************************************************************************************************
** 函数名称：write_data
** 函数描述：扇区写入数据,写入之前需要准备好扇区
** 输入参数：cclk              系统时钟 
**           flash_address     写入地址
**           * flash_data_buf  写入数据的缓存
**           count             写入数量必须为256 512 1024 4096
** 返回值  ：成功返回0 失败返回1
** 备注    ：理论上写入的数据都是已经擦除的空间,应该不存在已经存在数据的情况
*********************************************************************************************************/
void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count)
{
	NVIC_SETPRIMASK();//关闭系统中断
  param_table[0] = COPY_RAM_TO_FLASH;
  param_table[1] = flash_address;
  param_table[2] = (unsigned)flash_data_buf;
  param_table[3] = count;
  param_table[4] = cclk;
  iap_entry(param_table,result_table);
	NVIC_RESETPRIMASK();//打开系统中断
}
/*********************************************************************************************************
** 函数名称：write_flash
** 函数描述：擦除指定扇区
** 输入参数：cclk              写入频率
**           flash_address     写入地址
**           * flash_data_buf  写入数据的缓存
**           count             写入数量必须为256 512 1024 4096
** 返回值  ：成功返回0 失败返回1
** 备注    ：理论上写入的数据都是已经擦除的空间,应该不存在已经存在数据的情况
*********************************************************************************************************/
unsigned write_flash(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count)
{
	uint8_t i = 0;
	for(i=USER_START_SECTOR;i<=MAX_USER_SECTOR;i++)
	{
		if(flash_address < sector_end_map[i])
		{
			prepare_sector(i,i,cclk);                                  //根据地址找出应当准备哪一个扇区
			break;
		}
  }
	write_data(cclk,flash_address,flash_data_buf,count);
	//写完之后查看响应
	if(result_table[0] != CMD_SUCCESS)
	{
	  return 1;
	}
	return 0;
}










