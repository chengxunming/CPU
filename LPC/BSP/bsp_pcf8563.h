/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  PCF8563.h
** Latest modified Date:       2009-6-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Zhangrong
** Created date:               2009-6-8
** Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Modified by:                
** Modified date:              2009-6-8
** Version:
** Descriptions:
**
*********************************************************************************************************/
#ifndef __PCF8563_H
#define __PCF8563_H

#include "lpc177x_8x.h"

#define INT8U    uint8_t
#define INT16U   uint16_t
#define INT32U   uint32_t
//#define FALSE    0
//#define TRUE     1

typedef struct _PCF8563_DATE{
   INT8U  second;
   INT8U  minute;
   INT8U  hour;
   INT8U  date;
   INT8U  week;
   INT8U  month;
   INT16U  year;
}PCF8563_DATE;

#define PCF8563ADR    0xA2
/*********************************************************************************************************
 *  定义:秒/分/时/日/星期/月/年
 *********************************************************************************************************/
#define Second      0x02
#define Minute      0x03
#define Hour        0x04
#define Date        0x05
#define Week        0x06
#define Month       0x07
#define Year        0x08
/*********************************************************************************************************
 *  报警设置
 *********************************************************************************************************/
#define AlarmEn      0x00
#define AlarmDs      0x80
/*********************************************************************************************************
 *  ClkOut输出设置
 *********************************************************************************************************/
#define ClkOutEn    0x80
#define ClkOutDs    0x00
#define F32768Hz    0x00
#define F1024Hz     0x01
#define F32Hz       0x02
#define F1Hz        0x03
/**********************************************************************************************************
 *  开关设置
 **********************************************************************************************************/
#define Start      1
#define Stop       0
/**********************************************************************************************************
 *  定时器设置
 **********************************************************************************************************/
#define TimerOn         0x80
#define TimerOff        0x00
#define TimerClk4096Hz  0x00
#define TimerClk64Hz    0x01
#define TimerClk1Hz     0x02
#define TimerClk1_64Hz  0x03
/**********************************************************************************************************
 *  中断设置
 **********************************************************************************************************/
#define SetINT      0x80
#define ClearINT    0x40
#define TITP        0x10
#define AIE         0x02
#define TIE         0x01
/**********************************************************************************************************
 *  数据转换
 **********************************************************************************************************/
#define  BCD_to_HEC(b)  (((b>>4)*10)+(b&0x0f))
#define  HEC_to_BCD(h)  (((h/10)<<4)|(h%10))

/*********************************************************************************************************
** Function name:       PCF8563_Set
** Descriptions:        设置PCF8563
** input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
extern INT8U PCF8563_Set(PCF8563_DATE *tp);

/*********************************************************************************************************
** Function name:       PCF8563_Read
** Descriptions:        读取PCF8563
** input parameters:    tp:    指向时间结构体的指针
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
extern INT8U PCF8563_Read(PCF8563_DATE *tp);

/*********************************************************************************************************
** Function name:       PCF8563_SS
** Descriptions:        Start或Stop PCA8563
** input parameters:    cmd     PCF8563Start       开启PCF8563
**                              PCF8563Stop        关闭PCF8563
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
extern INT8U PCF8563_SS(INT8U cmd);

/*********************************************************************************************************
** Function name:       PCF8563_Set_TD
** Descriptions:        设置PCF8563的时间和日期
** input parameters:    cmd       Second         设置秒
**                                Minute         设置分
**                                Hour           设置小时
**                                Day            设置日
**                                Week           设置星期
**                                Month          设置月
**                                Year           设置年
**                        TD        设定值
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
extern INT8U PCF8563_Set_TD(INT8U cmd, INT16U TD);

/*********************************************************************************************************
** Function name:       PCF8563_Get_TD
** Descriptions:        读取PCF8563的时间和日期
** input parameters:    cmd        Second        设置秒
**                                Minute         设置分
**                                Hour           设置小时
**                                Day            设置日
**                                Week           设置星期
**                                Month          设置月
**                                Year           设置年
** Output parameters:   读取值
** Returned value:      NONE
*********************************************************************************************************/
extern INT16U PCF8563_Get_TD(INT8U cmd);

/*********************************************************************************************************
** Function name:       PCF8563_Set_ClkOut
** Descriptions:        设置PCF8563的输出时钟
** input parameters:    cmd     ClkOutEn   允许时钟输出
**                              ClkOutDs   禁止时钟输出
**                              F32768Hz   输出频为32.768KHz
**                              F1024Hz    输出频为1024Hz
**                              F32Hz      输出频为32Hz
**                              F1Hz       输出频为1Hz
** Output parameters:   NONE
** Returned value:      ClkOutEn|F32768Hz  设置ClkOut输出32.768KHz方波
*********************************************************************************************************/
extern INT8U PCF8563_Set_ClkOut(INT8U cmd);

/*********************************************************************************************************
** Function name:       PCF8563_Set_Alarm
** Descriptions:        设置PCF8563的报警状态
** input parameters:    cmd     AlarmEn    使能相应报警
**                              AlarmDs    禁止相应报警
**                              Minute     分报警
**                              Hour       小时报警
**                              Day        日报警
**                              Week       星期报警
**                      tm      报警时刻
** Output parameters:   NONE
** Returned value:      AlarmEn|Minute      使能分钟报警
*********************************************************************************************************/
extern INT8U PCF8563_Set_Alarm(INT8U cmd, INT8U tm);

/*********************************************************************************************************
** Function name:       PCF8563_Set_Timer
** Descriptions:        设置PCF8563的定时器
** input parameters:    cmd     TimerOn         使能定时器
**                              TimerOff        禁能定时器
**                              TimerClk4096Hz  设置定时器时钟源频率为4096Hz
**                              TimerClk64Hz    设置定时器时钟源频率为64Hz
**                              TimerClk1Hz     设置定时器时钟源频率为1Hz
**                              TimerClk1_64Hz  设置定时器时钟源频率为1/64Hz
**                      cnt     定时值
** Output parameters:   NONE
** Returned value:      TimerOn|TimerClk4096Hz    使能定时器,设置时钟源频率4096Hz
*********************************************************************************************************/
extern INT8U PCF8563_Set_Timer(INT8U cmd, INT8U cnt);

/*********************************************************************************************************
** Function name:       PCF8563_INT_State
** Descriptions:        设置中断或读取中断状态
** input parameters:    cmd     SetINT      设置中断
*                               ClearINT    清除所有中断标致
*                               TITP        使能INT脉冲输出
*                               AIE         使能报警中断
*                               TIE         使能定时器中断
** Output parameters:   返回中断状态
** Returned value:      NONE
*********************************************************************************************************/
extern INT8U PCF8563_INT_State(INT8U cmd);
extern uint8_t PCF8563_INT_Count;

void PCF8563_INT_GPIO_Init(void);
void PCF8563_INT_Enable(void);
void PCF8563_INT_Disable(void);

#endif

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
