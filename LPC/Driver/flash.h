/****************************************Copyright (h)****************************************************
** File name:                  flash.h
** Created by:                 Demos
** Created Date:               2016-6-15
** Version:                    V1.0
** Descriptions:                                            
**                             
*********************************************************************************************************/
#ifndef __FLASH_H_
#define __FLASH_H_

//#include "sys.h"
#include "sysTick.h"

//IAP�����
#define PREPARE_SECTOR_FOR_WRITE	50
#define COPY_RAM_TO_FLASH			    51
#define ERASE_SECTOR				      52
#define BLANK_CHECK_SECTOR			  53
#define READ_PART_ID				      54
#define READ_BOOT_VER				      55
#define COMPARE						        56
#define REINVOKE_ISP				      57

/*
//lpc1768������ʼ��ַ����
//IAP����ռ������������12K�Ŀռ�,ʣ�µĿռ�ȫ�����û��ɱ�̿ռ�
//LPC17x1 - 8 KB
//LPC17x2 - 64 KB
//LPC17x4 - 128 KB
//LPC17x5/6 - 256 KB
//LPC17x8 - 512 KB
#define SECTOR_0_START      0x00000000
#define SECTOR_1_START      0x00001000
#define SECTOR_2_START      0x00002000
#define SECTOR_3_START      0x00003000
#define SECTOR_4_START      0x00004000
#define SECTOR_5_START      0x00005000
#define SECTOR_6_START      0x00006000
#define SECTOR_7_START      0x00007000
#define SECTOR_8_START      0x00008000
#define SECTOR_9_START      0x00009000
#define SECTOR_10_START     0x0000A000
#define SECTOR_11_START     0x0000B000
#define SECTOR_12_START     0x0000C000
#define SECTOR_13_START     0x0000D000
#define SECTOR_14_START     0x0000E000
#define SECTOR_15_START     0x0000F000
#define SECTOR_16_START     0x00010000
#define SECTOR_17_START     0x00018000
#define SECTOR_18_START     0x00020000
#define SECTOR_19_START     0x00028000
#define SECTOR_20_START     0x00030000
#define SECTOR_21_START     0x00038000
#define SECTOR_22_START     0x00040000
#define SECTOR_23_START     0x00048000
#define SECTOR_24_START     0x00050000
#define SECTOR_25_START     0x00058000
#define SECTOR_26_START     0x00060000
#define SECTOR_27_START     0x00068000
#define SECTOR_28_START     0x00070000
#define SECTOR_29_START     0x00078000
*/


//lpc1788����������ַ����//512KB
#define SECTOR_0_END        0x00000FFF      //4KB
#define SECTOR_1_END        0x00001FFF      //4KB
#define SECTOR_2_END        0x00002FFF
#define SECTOR_3_END        0x00003FFF
#define SECTOR_4_END        0x00004FFF
#define SECTOR_5_END        0x00005FFF      //4KB
#define SECTOR_6_END        0x00006FFF
#define SECTOR_7_END        0x00007FFF
#define SECTOR_8_END        0x00008FFF
#define SECTOR_9_END        0x00009FFF
#define SECTOR_10_END       0x0000AFFF
#define SECTOR_11_END       0x0000BFFF
#define SECTOR_12_END       0x0000CFFF
#define SECTOR_13_END       0x0000DFFF
#define SECTOR_14_END       0x0000EFFF
#define SECTOR_15_END       0x0000FFFF
#define SECTOR_16_END       0x00017FFF      //32KB
#define SECTOR_17_END       0x0001FFFF
#define SECTOR_18_END       0x00027FFF
#define SECTOR_19_END       0x0002FFFF
#define SECTOR_20_END       0x00037FFF
#define SECTOR_21_END       0x0003FFFF
#define SECTOR_22_END       0x00047FFF
#define SECTOR_23_END       0x0004FFFF
#define SECTOR_24_END       0x00057FFF
#define SECTOR_25_END       0x0005FFFF
#define SECTOR_26_END       0x00067FFF
#define SECTOR_27_END       0x0006FFFF
#define SECTOR_28_END       0x00077FFF
#define SECTOR_29_END       0x0007FFFF


#define FLASH_APP1_ADDR		  0x00006000  	  //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)(4096*6=24576)
											                      //�����Ŀռ�ΪIAPʹ��
//#define FLASH_Updata_FLAG		0x00005800      //������־,24320


#define SIZE_UPDATA  512
//#define IAP_ADDRESS 0x1FFF1FF1	          //iap�����ַ,�ٷ��ֲ�����0x7ffffff0

//����������������޸�
#define USER_START_SECTOR 	5		            //�����û��������ʼ����
#define APP_START_SECTOR 	6		              //�����û��������ʼ����
#define MAX_USER_SECTOR   	29		          //����ϵͳ��ʹ�õ������������
#define MAX_FLASH_SECTOR 	30		            //ϵͳһ��ӵ�е���������

#define CCLK 	100000 		                    //���嵱ǰϵͳʱ��100Mʱ��
#define CMD_SUCCESS 0		                    //������������ִ�гɹ�
/*
extern unsigned result_table[5];            //���ؽ���б�

void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
void write_data(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count);
*/
//һ��һ�������û��ռ����,ʧ�ܷ���1
uint8_t erase_user_flash(void);
uint8_t erase_sector_flash(unsigned i);
void Flash_dowloads(void);

//������д��flash,ʧ�ܷ���0
unsigned write_flash(unsigned cclk,unsigned flash_address,unsigned * flash_data_buf, unsigned count);

#endif









