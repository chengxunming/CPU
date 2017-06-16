#include    "I2CINT.h"
#include    "bsp_pcf8563.h"

uint8_t PCF8563_INT_Count=0;
/*********************************************************************************************************
** Function name:       PCF8563_Set
** Descriptions:        ����PCF8563
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
** Descriptions:        ��ȡPCF8563
** input parameters:    tp:    ָ��ʱ��ṹ���ָ��
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
	   tp->week=0x07;                            //���ڶ������� ������=0x00
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
** Descriptions:        Start��Stop PCA8563
** input parameters:    cmd     PCF8563Start       ����PCF8563
**                              PCF8563Stop        �ر�PCF8563
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
** Descriptions:        ����PCF8563��ʱ�������
** input parameters:    cmd       Second         ������
**                                Minute         ���÷�
**                                Hour           ����Сʱ
**                                Day            ������
**                                Week           ��������
**                                Month          ������
**                                Year           ������
**                        TD        �趨ֵ
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
** Descriptions:        ��ȡPCF8563��ʱ�������
** input parameters:    cmd        Second        ������
**                                Minute         ���÷�
**                                Hour           ����Сʱ
**                                Day            ������
**                                Week           ��������
**                                Month          ������
**                                Year           ������
** Output parameters:   ��ȡֵ
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
** Descriptions:        ����PCF8563�����ʱ��
** input parameters:    cmd     ClkOutEn   ����ʱ�����
**                              ClkOutDs   ��ֹʱ�����
**                              F32768Hz   ���ƵΪ32.768KHz
**                              F1024Hz    ���ƵΪ1024Hz
**                              F32Hz      ���ƵΪ32Hz
**                              F1Hz       ���ƵΪ1Hz
** Output parameters:   NONE
** Returned value:      ClkOutEn|F32768Hz  ����ClkOut���32.768KHz����
*********************************************************************************************************/
INT8U PCF8563_Set_ClkOut(INT8U cmd)
{
   return(I2C_WriteNByte(PCF8563ADR, 1, 0x0d, &cmd, 1));
}
/*********************************************************************************************************
** Function name:       PCF8563_Set_Alarm
** Descriptions:        ����PCF8563�ı���״̬
** input parameters:    cmd     AlarmEn    ʹ����Ӧ����
**                              AlarmDs    ��ֹ��Ӧ����
**                              Minute     �ֱ���
**                              Hour       Сʱ����
**                              Day        �ձ���
**                              Week       ���ڱ���
**                      tm      ����ʱ��
** Output parameters:   NONE
** Returned value:      AlarmEn|Minute      ʹ�ܷ��ӱ���
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
** Descriptions:        ����PCF8563�Ķ�ʱ��
** input parameters:    cmd     TimerOn         ʹ�ܶ�ʱ��
**                              TimerOff        ���ܶ�ʱ��
**                              TimerClk4096Hz  ���ö�ʱ��ʱ��ԴƵ��Ϊ4096Hz
**                              TimerClk64Hz    ���ö�ʱ��ʱ��ԴƵ��Ϊ64Hz
**                              TimerClk1Hz     ���ö�ʱ��ʱ��ԴƵ��Ϊ1Hz
**                              TimerClk1_64Hz  ���ö�ʱ��ʱ��ԴƵ��Ϊ1/64Hz
**                      cnt     ��ʱֵ
** Output parameters:   NONE
** Returned value:      TimerOn|TimerClk4096Hz    ʹ�ܶ�ʱ��,����ʱ��ԴƵ��4096Hz
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
** Descriptions:        �����жϻ��ȡ�ж�״̬
** input parameters:    cmd     SetINT      �����ж�
*                               ClearINT    ��������жϱ���
*                               TITP        ʹ��INT�������
*                               AIE         ʹ�ܱ����ж�
*                               TIE         ʹ�ܶ�ʱ���ж�
** Output parameters:   �����ж�״̬
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
//PCF8563�ж����ų�ʼ��
void PCF8563_INT_GPIO_Init(void)
{
	LPC_IOCON->P2_13 |= 0x01 ;                                          /* �����ⲿ�ж�3��Ч            */
    LPC_SC->EXTMODE  |= 1ul << 3;                                       /* ���ô�����ʽ                 */
    LPC_SC->EXTPOLAR &= ~(1ul << 3);                                    /* �����½��ش���               */
    LPC_SC->EXTINT    = 1ul << 3;
    
    NVIC_SetPriority(EINT3_IRQn, 2);                                    /* ����EINT3�ж����ȼ���ʹ���ж�*/   /* ����PCF8563                  */
}
//PCF8563�ж�ʹ��
void PCF8563_INT_Enable(void)
{
	NVIC_EnableIRQ(EINT3_IRQn);
}
//��PCF8563�ж�
void PCF8563_INT_Disable(void)
{
	NVIC_DisableIRQ(EINT3_IRQn);
}


/*********************************************************************************************************
** �������ƣ�eint3Isr
** �����������ⲿ�ж�3������
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void EINT3_IRQHandler (void)
{
	LPC_SC->EXTINT = 1ul << 3;                                  //����ж�

	PCF8563_INT_Count++;
}
