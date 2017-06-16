#include    "I2CINT.h"
#include    "bsp_pcf8563.h"

uint8_t PCF8563_INT_Count=0;
/*********************************************************************************************************
** Function name:       PCF8563_Set
** Descriptions:        设置PCF8563
** input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
INT8U PCF8563_Set(PCF8563_DATE *tp)
{
   INT8U temp[7];
   INT8U year;

   temp[0] = HEC_to_BCD(tp->second);
   temp[1] = HEC_to_BCD(tp->minute);
   temp[2] = HEC_to_BCD(tp->hour);
   temp[3] = HEC_to_BCD(tp->date);
   temp[4] = tp->week;//HEC_to_BCD(tp->week);
   temp[5] = HEC_to_BCD(tp->month);

   if (tp->year >= 2000){
      temp[5] |= 0x80;
      year = tp->year - 2000;
      temp[6] = HEC_to_BCD(year);
   }
   else{
      year = tp->year - 1900;
      temp[6] = HEC_to_BCD(year);
   }
   return(I2C_WriteNByte(PCF8563ADR, 1, 0x02, temp, 7));
}
/*********************************************************************************************************
** Function name:       PCF8563_Read
** Descriptions:        读取PCF8563
** input parameters:    tp:    指向时间结构体的指针
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
INT8U PCF8563_Read(PCF8563_DATE *tp)
{
   INT8U temp[7];

   if (I2C_ReadNByte(PCF8563ADR, 1, 0x02, temp, 7)==FALSE)
      return FALSE;

   tp->second     = BCD_to_HEC((temp[0]&0x7f));
   tp->minute     = BCD_to_HEC((temp[1]&0x7f));
   tp->hour       = BCD_to_HEC((temp[2]&0x3f));
   tp->date       = BCD_to_HEC((temp[3]&0x3f));
   tp->week       = temp[4]&0x07;//BCD_to_HEC((temp[4]&0x07));
  
   if(tp->week==0)
   {
	   tp->week=0x07;                            //星期定义区别 星期天=0x00
   }

   if (temp[5]&0x80){
      tp->month = BCD_to_HEC((temp[5]&0x1f));
      tp->year  = BCD_to_HEC(temp[6])+2000;
   }
   else{
      tp->month = BCD_to_HEC((temp[5]&0x1f));
      tp->year  = BCD_to_HEC(temp[6])+1900;
   }
   return TRUE;
}

/*********************************************************************************************************
** Function name:       PCF8563_SS
** Descriptions:        Start或Stop PCA8563
** input parameters:    cmd     PCF8563Start       开启PCF8563
**                              PCF8563Stop        关闭PCF8563
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
INT8U PCF8563_SS(INT8U cmd)
{
   INT8U temp;

   if (cmd == Start)
      temp = 0;
   else
      temp = 0x20;
   return(I2C_WriteNByte(PCF8563ADR, 1, 0x00, &temp, 1));
}
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
INT8U PCF8563_Set_TD(INT8U cmd, INT16U TD)
{
   INT8U d[2];
   INT8U n;
   INT8U temp;
   INT8U err = TRUE;

   if (cmd == Year){
      err &= I2C_ReadNByte(PCF8563ADR, 1, Month, d, 2);
      d[0] &= 0x9f;
      if (TD < 2000){
         d[0] |= 0x80;
         d[1] = TD -1900;
      }
      else{
         d[0] &= ~(0x80);
         d[1] = TD - 2000;
      }
      d[1] = HEC_to_BCD(d[1]);
      n = 2;
      cmd = Month;
   }

   else if (cmd == Month){
      err &= I2C_ReadNByte(PCF8563ADR, 1, Month, &temp, 1);
      temp &= 0x80;
      d[0] = (INT8U)TD;
      d[0] = HEC_to_BCD(d[0]);
      d[0] = (d[0]|temp);
      n = 1;
   }

   else {
      d[0] = (INT8U)TD;
      d[0] = HEC_to_BCD(d[0]);
      n = 1;
   }
   err &= I2C_WriteNByte(PCF8563ADR, 1, cmd, d, n);
   return err;
}
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
INT16U PCF8563_Get_TD(INT8U cmd)
{
   INT8U d[2];

   if (cmd == Year){
      if (I2C_ReadNByte(PCF8563ADR, 1, Month, d, 2)==FALSE){
         return FALSE;
      }
      d[1] = BCD_to_HEC(d[1]);
      if (d[0]&0x80){
         return ((INT16U)d[1]+1900);
      }
      else {
         return ((INT16U)d[1]+2000);
      }
   }

   I2C_ReadNByte(PCF8563ADR, 1, cmd, d, 1);
   switch (cmd){
   case Minute :
   case Second :
      d[0] &= 0x7f;
      break;
   case Hour :
   case Date :
      d[0] &= 0x3f;
      break;
   case Month :
      d[0] &= 0x1f;
      break;
   case Week :
      d[0] &= 0x07;
      break;
   default :
      break;
   }
   d[0] = BCD_to_HEC(d[0]);
   return (INT16U)d[0];
}
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
INT8U PCF8563_Set_ClkOut(INT8U cmd)
{
   return(I2C_WriteNByte(PCF8563ADR, 1, 0x0d, &cmd, 1));
}
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
INT8U PCF8563_Set_Alarm(INT8U cmd, INT8U tm)
{
   INT8U temp;
   temp = 0;

   if ((cmd&0x80) == AlarmEn){
      temp = HEC_to_BCD(tm);
   }
   else temp = 0x80;

   cmd = (cmd&0x0f);
   return(I2C_WriteNByte(PCF8563ADR, 1, cmd+6, &temp,1));
}
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
INT8U PCF8563_Set_Timer(INT8U cmd, INT8U cnt)
{
   INT8U b[2];
   b[0] = cmd;
   b[1] = cnt;
   return(I2C_WriteNByte(PCF8563ADR, 1, 0x0E, b, 2));
}
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
INT8U PCF8563_INT_State(INT8U cmd)
{
   INT8U temp,INTstate;
   temp = 0;

   if ((cmd&SetINT) == SetINT){
      temp = (cmd&0x1f);
      I2C_WriteNByte(PCF8563ADR, 1, 0x01, &temp, 1);
      I2C_ReadNByte(PCF8563ADR, 1, 0x01, &temp, 1);
      return (temp&0x1f);
   }
   else if ((cmd&0x40) == ClearINT){
      I2C_ReadNByte(PCF8563ADR, 1, 0x01, &temp, 1);
      INTstate = (temp&0x1f);
      temp &= 0x13;
      I2C_WriteNByte(PCF8563ADR, 1, 0x01, &temp, 1);
      return INTstate;
   }
   return FALSE;
}

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
//PCF8563中断引脚初始化
void PCF8563_INT_GPIO_Init(void)
{
	LPC_IOCON->P2_13 |= 0x01 ;                                          /* 设置外部中断3有效            */
    LPC_SC->EXTMODE  |= 1ul << 3;                                       /* 设置触发方式                 */
    LPC_SC->EXTPOLAR &= ~(1ul << 3);                                    /* 设置下降沿触发               */
    LPC_SC->EXTINT    = 1ul << 3;
    
    NVIC_SetPriority(EINT3_IRQn, 2);                                    /* 设置EINT3中断优先级并使能中断*/   /* 开启PCF8563                  */
}
//PCF8563中断使能
void PCF8563_INT_Enable(void)
{
	NVIC_EnableIRQ(EINT3_IRQn);
}
//关PCF8563中断
void PCF8563_INT_Disable(void)
{
	NVIC_DisableIRQ(EINT3_IRQn);
}


/*********************************************************************************************************
** 函数名称：eint3Isr
** 函数描述：外部中断3服务函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void EINT3_IRQHandler (void)
{
	LPC_SC->EXTINT = 1ul << 3;                                  //清除中断

	PCF8563_INT_Count++;
}
