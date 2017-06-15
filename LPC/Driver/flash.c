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

unsigned param_table[5];//���ݲ����б�
unsigned result_table[5];//���ؽ���б�
uint32_t addrCur = FLASH_APP1_ADDR;			//��ǰϵͳд���ַ,ÿ��д��֮���ַ����2048

//uint16_t Max_bagnum;
uint16_t Update_BagNum;
//uint8_t Update_Lastbag_flag;
//uint8_t iapbuf[1024]; //���ڻ������ݵ�����
uint16_t iapbuf[1024]; //���ڻ������ݵ�����
//uint16_t Datalenth;


//����ϵͳʱ��
unsigned cclk = CCLK;

//flash������ַ��һ�ű�,��д�����ݵ�ʱ������
const unsigned sector_end_map[MAX_FLASH_SECTOR] = {SECTOR_0_END,SECTOR_1_END,    \
SECTOR_2_END,SECTOR_3_END,SECTOR_4_END,SECTOR_5_END,SECTOR_6_END,SECTOR_7_END,   \
SECTOR_8_END,SECTOR_9_END,SECTOR_10_END,SECTOR_11_END,SECTOR_12_END,             \
SECTOR_13_END,SECTOR_14_END,SECTOR_15_END,SECTOR_16_END,SECTOR_17_END,           \
SECTOR_18_END,SECTOR_19_END,SECTOR_20_END,SECTOR_21_END,SECTOR_22_END,           \
SECTOR_23_END,SECTOR_24_END,SECTOR_25_END,SECTOR_26_END,                         \
SECTOR_27_END,SECTOR_28_END,SECTOR_29_END										 };

//�ر�ϵͳ���ж�
__asm void NVIC_SETPRIMASK(void) 
{
  CPSID i
	BX r14
}

//��ϵͳ���ж�
__asm void NVIC_RESETPRIMASK(void) 
{
  CPSIE i
	BX r14
}
/*********************************************************************************************************
** �������ƣ�iap_entry
** ��������������iap����
** ���������param_tab[]     ָ���� 
**           result_tab[]    ����洢����
** ����ֵ  ��
** ��ע    ��
*********************************************************************************************************/
void iap_entry(unsigned param_tab[],unsigned result_tab[])
{
  void (*iap)(unsigned [],unsigned []);

  iap = (void (*)(unsigned [],unsigned []))IAP_ENTER_ADR;
  iap(param_tab,result_tab);
}
/*********************************************************************************************************
** �������ƣ�Flash_dowloads
** ����������Flash�������ش洢
** ���������
** ����ֵ  ��
** ��ע    ��
*********************************************************************************************************//*
void Flash_dowloads(void){
	uint8_t num1,num2,value;
  addrCur = FLASH_APP1_ADDR + (Update_BagNum-1)*512;
	if(!FM_RD(Flash_Clear_Flag)){
		return;
		//ɾ��ʧ���޷�д�룻
	}
	if(Update_Lastbag_flag){
		if(write_flash(100000,addrCur,(unsigned*)iapbuf,Datalenth-4)){
			
//			SPI_EE_WRITESingle(Flash_Clear_Flag,0x00);
		 	Flash_write_flag = 0;
//			Update_FVER_flag = 0;
//			RESET_FLAG=1;
   //д��ʧ�ܣ��ȴ���д
			return;
		}
		else{
			SPI_EE_WRITESingle(Flash_Clear_Flag,0x00);
		 	Flash_write_flag = 0;
			Update_FVER_flag = 0;
			RESET_FLAG=1;
			addrCur=FLASH_APP1_ADDR;
		}
	 //д��֮��Flash_write_flagҪ����ȴ���һ�δ���
   //Flash_write_flag = 0;
	}
  else if(Datalenth == 516){
	 //д��flash��
	  if(write_flash(100000,addrCur,(unsigned*)iapbuf,512)){
   //receiveMode = 0;
			Flash_write_flag = 0;
		  return;					
	  }
		else{
			Flash_write_flag = 0;
			
		  num1=Update_BagNum % 8;                             //�������ڼ�λ��λ
		  num2=Update_BagNum / 8;                             //�ڼ����ֽ���λ
		  value = FM_RD(Flash_Clear_Flag1+num2);
		  value |= (0x01<<(num1-1));
		  SPI_EE_WRITESingle((Flash_Clear_Flag+num2),value);
		}
//д��֮��Flash_write_flagҪ����ȴ���һ�δ���
//	  Flash_write_flag = 0;
  }
	else{                                                    
		Flash_write_flag = 0;	                               //���ݳ��ȴ��󣬲�дֱ�ӷ���
		return;
	}
}*/
/*********************************************************************************************************
** �������ƣ�prepare_sector
** ��������������׼����ָ��
** ���������start_sector    ��ʼ������ 
**           end_sector      ����������
**           cclk            ϵͳʱ�� 
** ����ֵ  ��
** ��ע    ��
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
** �������ƣ�erase_sector
** ������������������ָ��
** ���������start_sector    ��ʼ������ 
**           end_sector      ����������
**           cclk            ϵͳʱ�� 
** ����ֵ  ��
** ��ע    ��
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
** �������ƣ�erase_user_flash
** ���������������û�����ռ�
** ���������
** ����ֵ  ���ɹ�����0 ʧ�ܷ���1
** ��ע    ��
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
	
	NVIC_SETPRIMASK();//�ر�ϵͳ�ж�
	//һ��һ������������
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
	NVIC_RESETPRIMASK();//��ϵͳ�ж�
	return 0;//�ɹ�����0
}
/*********************************************************************************************************
** �������ƣ�erase_sector_flash
** ��������������ָ������
** ���������i          ������
** ����ֵ  ���ɹ�����0 ʧ�ܷ���1
** ��ע    ��
*********************************************************************************************************/
uint8_t erase_sector_flash(unsigned i)
{
	
	NVIC_SETPRIMASK();//�ر�ϵͳ�ж�
	//��������
	prepare_sector(i,i,cclk);
	erase_sector(i,i,cclk);
	if(result_table[0] != CMD_SUCCESS)
	{
	  return 1;
	}
	NVIC_RESETPRIMASK();//��ϵͳ�ж�
	return 0;//�ɹ�����0
}

/*********************************************************************************************************
** �������ƣ�write_data
** ��������������д������,д��֮ǰ��Ҫ׼��������
** ���������cclk              ϵͳʱ�� 
**           flash_address     д���ַ
**           * flash_data_buf  д�����ݵĻ���
**           count             д����������Ϊ256 512 1024 4096
** ����ֵ  ���ɹ�����0 ʧ�ܷ���1
** ��ע    ��������д������ݶ����Ѿ������Ŀռ�,Ӧ�ò������Ѿ��������ݵ����
*********************************************************************************************************/
void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count)
{
	NVIC_SETPRIMASK();//�ر�ϵͳ�ж�
  param_table[0] = COPY_RAM_TO_FLASH;
  param_table[1] = flash_address;
  param_table[2] = (unsigned)flash_data_buf;
  param_table[3] = count;
  param_table[4] = cclk;
  iap_entry(param_table,result_table);
	NVIC_RESETPRIMASK();//��ϵͳ�ж�
}
/*********************************************************************************************************
** �������ƣ�write_flash
** ��������������ָ������
** ���������cclk              д��Ƶ��
**           flash_address     д���ַ
**           * flash_data_buf  д�����ݵĻ���
**           count             д����������Ϊ256 512 1024 4096
** ����ֵ  ���ɹ�����0 ʧ�ܷ���1
** ��ע    ��������д������ݶ����Ѿ������Ŀռ�,Ӧ�ò������Ѿ��������ݵ����
*********************************************************************************************************/
unsigned write_flash(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count)
{
	uint8_t i = 0;
	for(i=USER_START_SECTOR;i<=MAX_USER_SECTOR;i++)
	{
		if(flash_address < sector_end_map[i])
		{
			prepare_sector(i,i,cclk);                                  //���ݵ�ַ�ҳ�Ӧ��׼����һ������
			break;
		}
  }
	write_data(cclk,flash_address,flash_data_buf,count);
	//д��֮��鿴��Ӧ
	if(result_table[0] != CMD_SUCCESS)
	{
	  return 1;
	}
	return 0;
}










