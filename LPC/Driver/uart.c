/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.c
** Last modified Date:  2014-4-18
** Last Version:        V1.0
** Descriptions:        uart����,�����жϴ���UART0,UART1��DMA���ʹ���
**
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

volatile uint32_t UART0Status, UART1Status;
volatile uint32_t UART2Status, UART3Status;
volatile uint32_t UART4Status;

volatile uint32_t UART0Count = 0, UART1Count = 0;
volatile uint32_t UART2Count = 0, UART3Count = 0;
volatile uint32_t UART4Count = 0;

volatile uint8_t UART0TxEmpty = 1, UART1TxEmpty = 1;
volatile uint8_t UART2TxEmpty = 1, UART3TxEmpty = 1;
volatile uint8_t UART4TxEmpty = 1;

uint8_t UART0Buffer[512], UART1Buffer[128];
uint8_t UART2Buffer[16], UART3Buffer[16],UART4Buffer[16];
uint8_t GPS_Data_ok=0;
static uint8_t Gpstime_err,Gps_date_ok;                     //GPSʱ�����GPSȡʱ�ɹ���ʶ

typedef struct tag_gps_data
{
    char GPS_time[20];          	//UTCʱ�䣬hhmmss(ʱ����)��ʽ
    char GPS_valid;              	//��λ״̬��A=��Ч��λ��V=��Ч��λ
    char GPS_wd[12];          		//γ��ddmm.mmmm(�ȷ�)��ʽ
    char dat_sn;					//γ�Ȱ���N(������)��S(�ϰ���)
    char GPS_jd[12];           		//���Ȱ���E(����)��W(����)
    char dat_ew;					//���Ȱ���E(����)��W(����)
    char GPS_speed[5];         		//��������(000.0~999.9�ڣ�ǰ���0Ҳ��������)
    char GPS_date[8];          		//UTC���ڣ�ddmmyy(������)��ʽ

}t_gps_data;

t_gps_data gps_data;

#define GPS_Frame_Buf_Size 100
uint8_t GPS_Frame_Buf[100];







/*******************************************************************************
1.�жϸ����GPS�������ݱ��浽Bufd.recv���ں��ʵĵط����ô˺�����ʼ������
2.���ñ��������Զ��رս���,Ȼ�����ڼ��GPS���ݿ��Ժ��ԡ�
3.�ں��ʵĵط��򿪽��ա�
*******************************************************************************/
void get_gps_useful_data(void)
{ 
     char *p_frame_start = NULL;
     char *p_useful_frame_start = NULL;
     int16_t rcv_buf_data_len = 0;
     int16_t index = 0;
     int16_t frame_start_point = 0;
    
     rcv_buf_data_len = Bufd.rpos;
     p_frame_start = &Bufd.recv[0];
        
    /* data example:
            $GPVTG,,,,,,,,,N*30 
            $GPRMC,132234.00,V,,,,,,,120116,,,N*7D 
            $GPRMC,133735.00,A,3949.63893,N,11616.48419,E,0.296,,120116,,,A*79
    */    
     for(index = 0; index < rcv_buf_data_len; )
     {     
         p_frame_start=strstr(Bufd.recv + index, "$GPRMC,"); //1. find start, "$GPRMC,"
         //$GPRMC,132234.00,V,,,,,,,120116,,,N*7D $GPRMC,133735.00,A,3949.63893,N,11616.48419,E,0.296,,120116,,,A*79
         if(p_frame_start) 
         {
              index = p_frame_start-Bufd.recv;
              if(index>0)
                {
                    memset(Bufd.recv, 0 , index);
                }

              index = index + 7;
              p_useful_frame_start = strstr(Bufd.recv + index, ",A,");//2 find useful data, ",A,"
              frame_start_point = p_useful_frame_start - p_frame_start;
              if((frame_start_point > 20)||(frame_start_point <= 0))//not find useful data
                {
                  GPS_Data_ok = 0;
									Gps.active	=0;  
									continue;
										
                }
                GPS_Data_ok = 1;
								Gps.active	=1;

                memset(GPS_Frame_Buf, 0, GPS_Frame_Buf_Size);
                memcpy(GPS_Frame_Buf, Bufd.recv + index-7 , 100);
								
            }
            else
            {
               break;
            } 
        }
        memset(Bufd.recv, 0, sizeof(Bufd.recv));
		Bufd.rpos=0;
}

/*********************************************************************************************************
** �������ƣ�UART0_IRQHandler/RS232
** ����������UART0�жϴ�����
** �����������
** ����ֵ  ����
** 
*********************************************************************************************************/
void UART0_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART0->IIR;

    uiIIRValue >>= 1;                                                     /* �жϱ�־�Ĵ���ֵ����1λ    */
    uiIIRValue &= 0x07;
	
	switch(uiIIRValue)
	{
		case IIR_RDA:													//����������Ч�ж�(���ﴥ����14)
			
			if((LPC_UART0->LSR & 0x0E)==0)								//��OE��PE��FE�������(��ֹδ��ʱ������жϲ��������)
			{
				do
				{
					Bufd.recv[Bufd.rpos++]	=LPC_UART0->RBR;
				}
				while((LPC_UART0->LSR & 0x01)==0x01);				//�ж����ݾ���λ
				
			}
			else
			{
				uiDummy=LPC_UART0->LSR;									//�������ָʾ
				do{uiDummy=LPC_UART0->RBR;}while((LPC_UART0->LSR & 0x01)==0x01);	//�����������
			}
			
			break;
			
		case IIR_CTI:													//���ճ�ʱ�ж�
			
			//LED_POWER=!LED_POWER;
			if((LPC_UART0->LSR & 0x0E)==0)							//�ж��Ƿ���OE��PE��FE�ײ�λ�������
			{
				do
				{
					Bufd.recv[Bufd.rpos++]	=LPC_UART0->RBR;
				}
				while ((LPC_UART0->LSR & 0x01)==0x01);				//�ж����ݾ���λ
				
				get_gps_useful_data();
				
				if(GPS_Data_ok)							//GPS��������Ч
				{
					Bufd.rpos	=0;                                 //������ָ�븴λ

					signed char hourtemp;

					Gps.hour	=(GPS_Frame_Buf[7]-0x30)*10+GPS_Frame_Buf[8]-0x30;
					Gps.minute	=(GPS_Frame_Buf[9]-0x30)*10+GPS_Frame_Buf[10]-0x30;
					Gps.second	=(GPS_Frame_Buf[11]-0x30)*10+GPS_Frame_Buf[12]-0x30;

					for (uint16_t i=100;i>20;i--)
					{
						if ((GPS_Frame_Buf[i]==',')&&(GPS_Frame_Buf[i-1]==',')&&(GPS_Frame_Buf[i-2]!=',')&&(GPS_Frame_Buf[i-8]==','))
						{
							Gps.date	=(GPS_Frame_Buf[i-7]-0x30)*10+GPS_Frame_Buf[i-6]-0x30;
							Gps.month	=(GPS_Frame_Buf[i-5]-0x30)*10+GPS_Frame_Buf[i-4]-0x30;
							Gps.year	=(GPS_Frame_Buf[i-3]-0x30)*10+GPS_Frame_Buf[i-2]-0x30;

							Gps_date_ok=1;

							break;
						}
						else
							Gps_date_ok=0;

					}
					hourtemp = Gps.hour;
					hourtemp = hourtemp+TIMEDIFF;

					Gps.hour	=((uint8_t)(24+Gps.hour+TIMEDIFF))%24;

					if (hourtemp>23)
					{
						uint8_t yeartemp,max_day;
						uint16_t i,j;

						i=Gps.year+2000;
						j=Gps.month;


						if ((i%400==0)|((i%4==0)&&(i%100!=0)))
						{
							yeartemp=1;
						}
						if (j==2)
						{
							if (yeartemp)
								max_day=29;
							else
								max_day=28;
						}
						else if ((j==1)|(j==3)|(j==5)|(j==7)|(j==8)|(j==10)|(j==12))
						{
							max_day=31;
						}
						else
						{
							max_day=30;
						}

						Gps.date=Gps.date+1;

						if (Gps.date>max_day)
						{
							Gps.date=1;
							Gps.month=Gps.month+1;
							if (Gps.month>12)
							{
								Gps.month=1;
								Gps.year=Gps.year+1;
							}
						}
					}
					else if (hourtemp<0)
					{
						uint8_t yeartemp,max_day;
						uint16_t i,j;

						i=Gps.year+2000;
						j=Gps.month;


						if ((i%400==0)|((i%4==0)&&(i%100!=0)))
						{
							yeartemp=1;
						}
						if (j==2)
						{
							if (yeartemp)
								max_day=29;
							else
								max_day=28;
						}
						else if ((j==1)|(j==3)|(j==5)|(j==7)|(j==8)|(j==10)|(j==12))
						{
							max_day=31;
						}
						else
						{
							max_day=30;
						}

						Gps.date=Gps.date-1;

						if (Gps.date==0)
						{
							Gps.date=max_day;
							Gps.month=Gps.month-1;
							if (Gps.month==0)
							{
								Gps.month=12;
								Gps.year=Gps.year-1;
							}
						}
					}


					if ((Gps.second>59)||(Gps.minute>59)||(Gps.hour>23)||(Gps.date>31)||(Rtc.date==0)||(Gps.month>12)||(Gps.month==0)||(Gps.year>99))

						Gpstime_err=1;
					else
						Gpstime_err=0;


					GPS_Data_ok = 0;

					if ((Gps.updaen)&&(Gpstime_err==0))								//��GPS����ʹ��
					{
						uint16_t y,m,d,w;

						Rtc.hour	=Gps.hour;						//���͸��ݴ����
						Rtc.minute	=Gps.minute%60;
						Rtc.second	=Gps.second%60;

						if (Gps_date_ok)
						{
							Rtc.year	=Gps.year+2000;
							Rtc.month	=Gps.month;
							Rtc.date	=Gps.date;

							y	=	Gps.year+2000;
							m	=	Gps.month;
							d	=	Gps.date;

							if (m == 1 | m == 2)
							{
								m += 12;
								y--;
							}
							//w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 -y / 100 +y / 400) % 7 + 1;  //��ķ����ɭ���㹫ʽ
							w = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400+1)%7;

							Rtc.week = w;
						}


						Gps.wrtlog	=1;								//GPS����ʱ�Ӳ�д��־

						GPS_update_off();							//�������ر�ʹ��

					}
				}	

			}
			else
			{
				uiDummy=LPC_UART0->LSR;									//�������ָʾ		
				do{uiDummy=LPC_UART0->RBR;}while((LPC_UART0->LSR & 0x01)==0x01);	//�����������
			}
			break;
		case IIR_THRE:
			uiLSRValue = LPC_UART0->LSR;
			if ( uiLSRValue & LSR_THRE ) 
			{
				UART0TxEmpty = 1;
			} 
			else 
			{
				UART0TxEmpty = 0;
			}
			break;
		default:
			uiDummy = LPC_UART0->LSR;
			break;		
	}
}


/*********************************************************************************************************
** �������ƣ�UART1_IRQHandler
** ����������UART1�жϴ�����/S2E
** �����������
** ����ֵ  ����
** 
*********************************************************************************************************/
void UART1_IRQHandler (void) 
{
    uint8_t uiIIRValue = 0, uiLSRValue = 0;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART1->IIR;

    uiIIRValue >>= 1;                                                     /* �жϱ�־�Ĵ���ֵ����1λ    */

    uiIIRValue &= 0x07;
	
	switch(uiIIRValue)
	{
		case IIR_RDA:													//����������Ч�ж�(���ﴥ����14)
			
			if((LPC_UART1->LSR & 0x0E)==0)								//��OE��PE��FE�������(��ֹδ��ʱ������жϲ��������)
			{
				do
				{
					Netrev.buf[Netrev.pos++]	=LPC_UART1->RBR;
					
					if((ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_MAXNUM))		//������������ش���
					{
						memset(&Netrev,0,sizeof(NETRECV));				//��λNetrev�ṹ�壬�����ж��ٴν���(Netrev.ok=0)
						
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
						
						return;
					}	
					else if((!ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_NUM))
					{
						memset(&Netrev,0,sizeof(NETRECV));				//��λNetrev�ṹ�壬�����ж��ٴν���(Netrev.ok=0)
						
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
						
						return;
					}
				}
				while((LPC_UART1->LSR & 0x01)==0x01);				//�ж����ݾ���λ
				
				if	((Netrev.buf[Netrev.pos-1]==0x7E)&&
					(Netrev.buf[Netrev.pos-2]==0xE7))					//����������յ������һ���ֽ���֡β���򻺳�����ɱ�־��1
					Netrev.ok	=1;
				else
					Netrev.ok	=0;										//������0����ʹ֮ǰ�Ѿ�������Ч֡
			}
			else
			{
				uiDummy=LPC_UART1->LSR;									//�������ָʾ
				do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
			}
			
			break;
			
		case IIR_CTI:													//���ճ�ʱ�ж�
			
			//LED_POWER=!LED_POWER;
			if((LPC_UART1->LSR & 0x0E)==0)							//�ж��Ƿ���OE��PE��FE�ײ�λ�������
			{
				do
				{
					Netrev.buf[Netrev.pos++]	=LPC_UART1->RBR;			//RBR���������ջ�����	
					//Netrev.pos=Netrev.pos+1;
					
					if((ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_MAXNUM))		//������������ش���
					{
						memset(&Netrev,0,sizeof(NETRECV));				//��λNetrev�ṹ�壬�����ж��ٴν���(Netrev.ok=0)
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
						return;
					}	
					else if((!ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_NUM))
					{
						memset(&Netrev,0,sizeof(NETRECV));				//��λNetrev�ṹ�壬�����ж��ٴν���(Netrev.ok=0)
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
						return;
					}
				}
				while((LPC_UART1->LSR & 0x01)==0x01);			//�ж����ݾ���λ
				
				if	((Netrev.buf[Netrev.pos-1]==0x7E)&&
					(Netrev.buf[Netrev.pos-2]==0xE7))					//����������յ������һ���ֽ���֡β���򻺳�����ɱ�־��1
					Netrev.ok	=1;
				else
				{
					memset(&Netrev,0,sizeof(NETRECV));
					Netrev.ok	=0;										//������0����ʹ֮ǰ�Ѿ�������Ч֡
				}
			}
			else
			{
				uiDummy=LPC_UART1->LSR;									//�������ָʾ		
				do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//�����������
			}
			break;
		case IIR_THRE:
			uiLSRValue = LPC_UART1->LSR;
			if ( uiLSRValue & LSR_THRE ) {
            UART1TxEmpty = 1;
			} else {
            UART1TxEmpty = 0;
			}
			break;
		default:
			uiDummy = LPC_UART1->LSR;
		break;		
	}
}

/*********************************************************************************************************
** �������ƣ�UART2_IRQHandler
** ����������UART2�жϴ�����
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void UART2_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART2->IIR;

    uiIIRValue >>= 1;                                                     /* �жϱ�־�Ĵ���ֵ����1λ    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* ������״̬�ж�             */
        uiLSRValue = LPC_UART2->LSR;
        /* 
         * ����жϡ���ż�жϡ�֡�жϡ�UART RBR�а����������ݺ�֡����жϴ���
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART2Status = uiLSRValue;
            uiDummy = LPC_UART2->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* ����FIFO��Ϊ���ж�         */
            UART2Buffer[UART2Count] = LPC_UART2->RBR;
            UART2Count++;
            if ( UART2Count == BUFSIZE ) {
                UART2Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* �������ݿ����ж�           */
        UART2Buffer[UART2Count] = LPC_UART2->RBR;
        UART2Count++;
        if ( UART2Count == BUFSIZE ) {
            UART2Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* �ַ���ʱ�ж�               */
        UART2Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* ���ͱ��ּĴ������ж�       */
        uiLSRValue = LPC_UART2->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART2TxEmpty = 1;
        } else {
            UART2TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** �������ƣ�UART3_IRQHandler
** ����������UART3�жϴ�����
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void UART3_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART3->IIR;

    uiIIRValue >>= 1;                                                     /* �жϱ�־�Ĵ���ֵ����1λ    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* ������״̬�ж�             */
        uiLSRValue = LPC_UART3->LSR;
        /* 
         * ����жϡ���ż�жϡ�֡�жϡ�UART RBR�а����������ݺ�֡����жϴ���
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART3Status = uiLSRValue;
            uiDummy = LPC_UART3->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* ����FIFO��Ϊ���ж�         */
            UART3Buffer[UART3Count] = LPC_UART3->RBR;
            UART3Count++;
            if ( UART3Count == BUFSIZE ) {
                UART3Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* �������ݿ����ж�           */
        UART3Buffer[UART3Count] = LPC_UART3->RBR;
        UART3Count++;
        if ( UART3Count == BUFSIZE ) {
            UART3Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* �ַ���ʱ�ж�               */
        UART3Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* ���ͱ��ּĴ������ж�       */
        uiLSRValue = LPC_UART3->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART3TxEmpty = 1;
        } else {
            UART3TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** �������ƣ�UART4_IRQHandler
** ����������UART4�жϴ�����
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void UART4_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART4->IIR;

    uiIIRValue >>= 1;                                                     /* �жϱ�־�Ĵ���ֵ����1λ    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* ������״̬�ж�             */
        uiLSRValue = LPC_UART4->LSR;
        /* 
         * ����жϡ���ż�жϡ�֡�жϡ�UART RBR�а����������ݺ�֡����жϴ���
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART4Status = uiLSRValue;
            uiDummy = LPC_UART4->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* ����FIFO��Ϊ���ж�         */
            UART4Buffer[UART4Count] = LPC_UART4->RBR;
            UART4Count++;
            if ( UART4Count == BUFSIZE ) {
                UART4Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* �������ݿ����ж�           */
        UART4Buffer[UART4Count] = LPC_UART4->RBR;
        UART4Count++;
        if ( UART4Count == BUFSIZE ) {
            UART4Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* �ַ���ʱ�ж�               */
        UART4Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* ���ͱ��ּĴ������ж�       */
        uiLSRValue = LPC_UART4->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART4TxEmpty = 1;
        } else {
            UART4TxEmpty = 0;
        }
    }
}






/*********************************************************************************************************
** �������ƣ�ModemInit
** ����������Modem���ܳ�ʼ����ֻ��UART1ӵ�д˹��ܣ�
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void ModemInit(uint32_t uiLocation )
{
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P0_20 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_20 |= 0x01;                                       /* UART DTR                     */
        LPC_IOCON->P0_17 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_17 |= 0x01;                                       /* UART CTS                     */
        LPC_IOCON->P0_22 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_22 |= 0x01;                                       /* UART RTS                     */
        LPC_IOCON->P0_19 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_19 |= 0x01;                                       /* UART DSR                     */
        LPC_IOCON->P0_18 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_18 |= 0x01;                                       /* UART DCD                     */
        LPC_IOCON->P0_21 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_21 |= 0x01;                                       /* UART RI                      */
        break;

    case 1:
        LPC_IOCON->P2_5  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_5  |= 0x02;                                       /* UART DTR                     */
        LPC_IOCON->P2_2  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_2  |= 0x02;                                       /* UART CTS                     */
        LPC_IOCON->P2_7  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_7  |= 0x02;                                       /* UART RTS                     */
        LPC_IOCON->P2_4  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_4  |= 0x02;                                       /* UART DSR                     */
        LPC_IOCON->P2_3  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_3  |= 0x02;                                       /* UART DCD                     */
        LPC_IOCON->P2_6  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_6  |= 0x02;                                       /* UART RI                      */
        break;

    case 2:
        LPC_IOCON->P2_5  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_5  |= 0x02;                                       /* UART DTR                     */
        LPC_IOCON->P2_8  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_8  |= 0x03;                                       /* UART CTS                     */
        LPC_IOCON->P0_6  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_6  |= 0x04;                                       /* UART RTS                     */
        LPC_IOCON->P2_4  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_4  |= 0x02;                                       /* UART DSR                     */
        LPC_IOCON->P2_3  &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P2_3  |= 0x02;                                       /* UART DCD                     */
        LPC_IOCON->P2_6  &= ~0x07;                                      /* UART I/O config              */
        break;
    #if 0
    case 3:
        LPC_IOCON->P3_21 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_21 |= 0x03;                                       /* UART DTR                     */
        LPC_IOCON->P3_18 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_18 |= 0x03;                                       /* UART CTS                     */
        LPC_IOCON->P3_30 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_30 |= 0x02;                                       /* UART RTS                     */
        LPC_IOCON->P3_20 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_20 |= 0x03;                                       /* UART DSR                     */
        LPC_IOCON->P3_19 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_19 |= 0x03;                                       /* UART DCD                     */
        LPC_IOCON->P3_22 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_22 |= 0x03;                                       /* UART RI                      */
        break;
    #else
    case 3:
        LPC_IOCON->P3_21 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_21 |= 0x03;                                       /* UART DTR                     */
        LPC_IOCON->P3_18 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_18 |= 0x03;                                       /* UART CTS                     */
        LPC_IOCON->P0_22 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_22 |= 0x01;                                       /* UART RTS                     */
        LPC_IOCON->P3_20 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_20 |= 0x03;                                       /* UART DSR                     */
        LPC_IOCON->P3_19 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P3_19 |= 0x03;                                       /* UART DCD                     */
        LPC_IOCON->P0_21 &= ~0x07;                                      /* UART I/O config              */
        LPC_IOCON->P0_21 |= 0x01;                                       /* UART RI                      */
        break;
    #endif

    }

    LPC_UART1->MCR = 0xC0;                                              /* Enable Auto RTS and Auto CTS */
    return;
}


/*********************************************************************************************************
** �������ƣ�UARTInit
** �������������ڳ�ʼ��
** ���������PortNum    UART�˿ں�
**           bandrate   �趨�Ĵ��ڲ�����
** ����ֵ  ����
*********************************************************************************************************/
uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate )
{
    uint32_t Fdiv;

    if ( PortNum == 0 ) {
        LPC_SC->PCONP |= 0x00000008;
		
		LPC_IOCON->P0_2  &= ~0x07;
		LPC_IOCON->P0_2  |= 1;                                      /* P0.2ΪU0_TXD                 */
		LPC_IOCON->P0_3  &= ~0x07;
		LPC_IOCON->P0_3  |= 1;                                      /* P0.3ΪU0_RXD                 */
        /*
         * 8Ϊ����λ������ż����λ��1λֹͣλ
         */
        LPC_UART0->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*�����ֵ                      */
        LPC_UART0->DLM = Fdiv / 256;
        LPC_UART0->DLL = Fdiv % 256;
        LPC_UART0->LCR = 0x03;                                          /* ������������                 */
        LPC_UART0->FCR = 0xC7;                                          /* ʹ�ܲ���λFIFO   14�ֽڴ���  */

        
        /* 
         * ʹ�ܽ���FIFO��Ϊ���жϡ����ͱ��ּĴ������жϡ�������״̬�ж�
         */
		NVIC_EnableIRQ(UART0_IRQn);
		LPC_UART0->IER = IER_RBR | IER_RLS;//| IER_THRE
		
		//LPC_UART0->IER = IER_RBR;                                       /*��ʹ�ܽ������ݿ����жϼ����ճ�ʱ�ж�*/
        return (TRUE);
    }
    else if ( PortNum == 1 ) {
        LPC_SC->PCONP |= 0x00000010;
        
		LPC_IOCON->P0_15 &= ~0x07;
		LPC_IOCON->P0_15 |= 1;                                      /* P0.15ΪU1_TXD                */
		LPC_IOCON->P0_16 &= ~0x07;
		LPC_IOCON->P0_16 |= 1;                                      /* P0.16ΪU1_RXD                */

        /*
         * 8Ϊ����λ������ż����λ��1λֹͣλ
         */
        LPC_UART1->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*�����ֵ                      */
        LPC_UART1->DLM = Fdiv / 256;
        LPC_UART1->DLL = Fdiv % 256;
        LPC_UART1->LCR = 0x03;                                          /* ������������                 */
        LPC_UART1->FCR = 0xC7;                                          /* ʹ�ܲ���λFIFO               */

        NVIC_EnableIRQ(UART1_IRQn);
		//NVIC_SetPriority(UART1_IRQn,11);
        /* 
         * ʹ�ܽ���FIFO��Ϊ���жϡ����ͱ��ּĴ������жϡ�������״̬�ж�
         */
        LPC_UART1->IER = IER_RBR | IER_RLS;//| IER_THRE
		//LPC_UART1->IER = IER_RBR;                                       /*��ʹ�ܽ������ݿ����жϼ����ճ�ʱ�ж�*/
        return (TRUE);
    }
    else if ( PortNum == 2 ) {
        LPC_SC->PCONP |= 1ul<<24;
        
            LPC_IOCON->P0_10 &= ~0x07;
            LPC_IOCON->P0_10 |= 1;                                      /* P0.10ΪU2_TXD                */
            LPC_IOCON->P0_11 &= ~0x07;
            LPC_IOCON->P0_11 |= 1;                                      /* P0.11ΪU2_RXD                */
        /*
         * 8Ϊ����λ������ż����λ��1λֹͣλ
         */
        LPC_UART2->LCR = 0x83;        
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*�����ֵ                      */
        LPC_UART2->DLM = Fdiv / 256;
        LPC_UART2->DLL = Fdiv % 256;
        LPC_UART2->LCR = 0x03;                                          /* ������������                 */
		LPC_UART2->LCR |= 0x04;											/* ��λֹͣλ                 */
        LPC_UART2->FCR = 0xC7;                                          /* ʹ�ܲ���λFIFO               */

		NVIC_EnableIRQ(UART2_IRQn);
		//NVIC_SetPriority(UART2_IRQn,12);
        /* 
         * ʹ�ܽ���FIFO��Ϊ���жϡ����ͱ��ּĴ������жϡ�������״̬�ж�
         */
		LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;
			//LPC_UART2->IER = IER_RBR;                                       /*��ʹ�ܽ������ݿ����жϼ����ճ�ʱ�ж�*/
        return (TRUE);
    }
    else if ( PortNum == 3 ) {
        LPC_SC->PCONP |= 0x02000000;

            LPC_IOCON->P4_28 &= ~0x07;
            LPC_IOCON->P4_28 |= 2;                                      /* P4.28ΪU3_TXD                */
            LPC_IOCON->P4_29 &= ~0x07;
            LPC_IOCON->P4_29 |= 2;                                      /* P4.29ΪU3_RXD                */

        /*
         * 8Ϊ����λ������ż����λ��1λֹͣλ
         */
        LPC_UART3->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*�����ֵ                      */
        LPC_UART3->DLM = Fdiv / 256;
        LPC_UART3->DLL = Fdiv % 256;
        LPC_UART3->LCR = 0x03;                                          /* ������������                 */
				LPC_UART3->LCR |= 0x04;											/* ��λֹͣλ                 */
        LPC_UART3->FCR = 0xC7;                                          /* ʹ�ܲ���λFIFO               */
        
        NVIC_EnableIRQ(UART3_IRQn);
        /* 
         * ʹ�ܽ���FIFO��Ϊ���жϡ����ͱ��ּĴ������жϡ�������״̬�ж�
         */
        LPC_UART3->IER = IER_RBR  | IER_RLS; //| IER_THRE
		//LPC_UART3->IER = IER_RBR;                                       /*��ʹ�ܽ������ݿ����жϼ����ճ�ʱ�ж�*/
        return (TRUE);
    }
    else if ( PortNum == 4 ) {
        LPC_SC->PCONP |= 0x00000100;
        
            LPC_IOCON->P5_4  &= ~0x07;
            LPC_IOCON->P5_4  |= 4;                                      /* P5.4ΪU4_TXD                 */
            LPC_IOCON->P5_3  &= ~0x07;
            LPC_IOCON->P5_3  |= 4;                                      /* P5.3ΪU4_RXD                 */
       
        /*
         * 8Ϊ����λ������ż����λ��1λֹͣλ
         */
        LPC_UART4->LCR = 0x83;                          
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*�����ֵ                      */
        LPC_UART4->DLM = Fdiv / 256;
        LPC_UART4->DLL = Fdiv % 256;
        LPC_UART4->LCR = 0x03;                                          /* ������������                 */
				LPC_UART4->LCR |= 0x04;											/* ��λֹͣλ                 */
        LPC_UART4->FCR = 0xC7;                                          /* ʹ�ܲ���λFIFO               */
        
        //NVIC_EnableIRQ(UART4_IRQn);
        /* 
         * ʹ�ܽ���FIFO��Ϊ���жϡ����ͱ��ּĴ������жϡ�������״̬�ж�
         */
        LPC_UART4->IER = IER_RBR | IER_RLS; //| IER_THRE
				//LPC_UART4->IER = IER_RBR;                                       /*��ʹ�ܽ������ݿ����жϼ����ճ�ʱ�ж�*/
        return (TRUE);
    }
    return( FALSE );
}


/*********************************************************************************************************
** �������ƣ�UARTSend
** �������������ڷ�������
** ���������PortNum    UART�˿ں�
**           BufferPtr  �������ݻ�����
**           Length     �������ݳ���
** ����ֵ  ����
*********************************************************************************************************/
void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
    if ( portNum == 0 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART0->LSR & (1<<5)));                           /* �ȴ����ͱ�־Ϊ1              */
            LPC_UART0->THR = *BufferPtr;                                /* ��������                     */
            UART0TxEmpty = 0;                                           /* ���ͱ�־��0                  */
            BufferPtr++;                                                /* ָ�����                     */
            Length--;                                                   /* ������Ŀ��1                  */
        }
    }
    else if ( portNum == 1 ) {
        while ( Length != 0 ) {
           while ( !(LPC_UART1->LSR & (1<<5)));                           /* �ȴ����ͱ�־Ϊ1              */
            LPC_UART1->THR = *BufferPtr;                                /* ��������                     */
            UART1TxEmpty = 0;                                           /* ���ͱ�־��0                  */
            BufferPtr++;                                                /* ָ�����                     */
            Length--;                                                   /* ������Ŀ��1                  */
        }
    }
    else if ( portNum == 2 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART2->LSR & (1<<5)));                           /* �ȴ����ͱ�־Ϊ1              */
            LPC_UART2->THR = *BufferPtr;                                /* ��������                     */
            UART2TxEmpty = 0;                                           /* ���ͱ�־��0                  */
            BufferPtr++;                                                /* ָ�����                     */
            Length--;                                                   /* ������Ŀ��1                  */
        }
    }
    else if ( portNum == 3 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART3->LSR & (1<<5)));                           /* �ȴ����ͱ�־Ϊ1              */
            LPC_UART3->THR = *BufferPtr;                                /* ��������                     */
            UART3TxEmpty = 0;                                           /* ���ͱ�־��0                  */
            BufferPtr++;                                                /* ָ�����                     */
            Length--;                                                   /* ������Ŀ��1                  */
        }
    }
    else if ( portNum == 4 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART4->LSR & (1<<5)));                           /* �ȴ����ͱ�־Ϊ1              */
            LPC_UART4->THR = *BufferPtr;                                /* ��������                     */
            UART4TxEmpty = 0;                                           /* ���ͱ�־��0                  */
            BufferPtr++;                                                /* ָ�����                     */
            Length--;                                                   /* ������Ŀ��1                  */
        }
    }
    return;
}

/*********************************************************************************************************
** �������ƣ�UART0DMA_Init
** ��������������1DMA��ʼ��
** ���������
**          
** ����ֵ  ����
*********************************************************************************************************/
void UART_DMA_Init(uint8_t CHnum)
{
	LPC_SC->PCONP |= (1 << 29);                                         /* ����DMA����                  */
	LPC_SC->DMAREQSEL = 0x0000;                                         /* ѡ��DMA����                  */
	LPC_GPDMA->IntTCClear |= 0x0F;                                       /* ���ͨ��1���ն˼����ж�     */
	LPC_GPDMA->IntErrClr  |= 0x0F;                                       /* ���ͨ��0��3��DMA�����ж�   */
	LPC_GPDMA->Config = 0x01;                                           /* ʹ��DMA���ֽ�˳��ΪС��      */
        
    while (!(LPC_GPDMA->Config & 0x01));
    
    NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);                                           /* ʹ��DMA�ж�����              */
	
	switch (CHnum)
	{
		case 0:
		
			LPC_GPDMACH0->CLLI =	0;
			LPC_GPDMACH0->CConfig 	|= (0x01 << 11)	                                /* ��ʼ�����ķ�����Ϊ�Ӵ洢���� */
																					/* �����Լ���������ΪDMA������  */
									|  (0x00 << 1)                                     /* ָ��DMA�����Դ�Ǵ洢��      */
									|  (0x01<<15)
									|  (0x0E<< 6);                                    /* ָ��DMA�����Ŀ����UART2TX   */
			
			LPC_GPDMACH0->CControl	|= (0x02 << 12)		                               /* ָ��Դͻ�������СΪ8λ      */
									|  (0x02 << 15)                                    /* ָ��Ŀ��ͻ�������СΪ8λ    */
									|  (0x00 << 18)                                    /* ָ��Դ�������ݿ��Ϊ8λ      */
									|  (0x00 << 21);                                   /* ָ��Ŀ�괫�����ݿ��Ϊ8λ    */ 
			LPC_GPDMACH0->CSrcAddr  = (uint32_t)UART2Buffer;                 /* ͨ��1Դ��ַ                  */
			LPC_GPDMACH0->CDestAddr = (uint32_t)&LPC_UART2->THR; //0x40010000;               /* ͨ��1Ŀ�ĵ�ַ         */
			
			LPC_GPDMACH0->CControl  |= (1 << 26)		                                /* ָ��Դ��ַ�Զ�����           */
									|  (0 << 27);		                                /* ָ��Ŀ���ַ���Զ�����	    */
			LPC_GPDMACH0->CControl  |= 0x80000000;
		
		break;
		case 1:
		
			LPC_GPDMACH1->CLLI =	0;
			LPC_GPDMACH1->CConfig 	|= (0x01 << 11)	                                /* ��ʼ�����ķ�����Ϊ�Ӵ洢���� */
																					/* �����Լ���������ΪDMA������  */
									|  (0x00 << 1)                                     /* ָ��DMA�����Դ�Ǵ洢��      */
									|  (0x01<<15)
									|  (0x0A << 6);                                    /* ָ��DMA�����Ŀ����UART1TX   */
			
			LPC_GPDMACH1->CControl	|= (0x02 << 12)		                               /* ָ��Դͻ�������СΪ8λ      */
									|  (0x02 << 15)                                    /* ָ��Ŀ��ͻ�������СΪ8λ    */
									|  (0x00 << 18)                                    /* ָ��Դ�������ݿ��Ϊ8λ      */
									|  (0x00 << 21);                                   /* ָ��Ŀ�괫�����ݿ��Ϊ8λ    */ 
			LPC_GPDMACH1->CSrcAddr  = (uint32_t)UART1Buffer;                 /* ͨ��1Դ��ַ                  */
			LPC_GPDMACH1->CDestAddr = (uint32_t)&LPC_UART3->THR; //0x40010000;               /* ͨ��1Ŀ�ĵ�ַ         */
			
			LPC_GPDMACH1->CControl  |= (1 << 26)		                                /* ָ��Դ��ַ�Զ�����           */
									|  (0 << 27);		                                /* ָ��Ŀ���ַ���Զ�����	    */
			LPC_GPDMACH1->CControl  |= 0x80000000;
		
		break;	
		default:break;
	}
		
	
	
	 
}



void UART_DMA_Send (uint8_t Channel,uint32_t uiNum)
{   
    switch (Channel)
	{
		case 0:
		
			UART2DMADone = 0;
			LPC_GPDMACH0->CControl |= (uiNum&0x0FFF);		                                        /* ָ��ͻ���������             */

			/*
			 *  DMA�������� 
			 */
			LPC_GPDMACH0->CConfig  |= 0x01;	 		                                    /* ʹ��DMAͨ��0                */
		break;
		case 1:
		
			UART3DMADone = 0;
			LPC_GPDMACH1->CControl |= (uiNum&0x0FFF);		                                        /* ָ��ͻ���������             */

			/*
			 *  DMA�������� 
			 */
			LPC_GPDMACH1->CConfig  |= 0x01;	 		                                    /* ʹ��DMAͨ��1                 */
		break;
		default:break;
	}
		
	
}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
