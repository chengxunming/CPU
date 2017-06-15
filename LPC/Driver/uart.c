/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           uart.c
** Last modified Date:  2014-4-18
** Last Version:        V1.0
** Descriptions:        uart驱动,包含中断处理及UART0,UART1的DMA发送处理
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
static uint8_t Gpstime_err,Gps_date_ok;                     //GPS时间错误及GPS取时成功标识

typedef struct tag_gps_data
{
    char GPS_time[20];          	//UTC时间，hhmmss(时分秒)格式
    char GPS_valid;              	//定位状态，A=有效定位，V=无效定位
    char GPS_wd[12];          		//纬度ddmm.mmmm(度分)格式
    char dat_sn;					//纬度半球N(北半球)或S(南半球)
    char GPS_jd[12];           		//经度半球E(东经)或W(西经)
    char dat_ew;					//经度半球E(东经)或W(西经)
    char GPS_speed[5];         		//地面速率(000.0~999.9节，前面的0也将被传输)
    char GPS_date[8];          		//UTC日期，ddmmyy(日月年)格式

}t_gps_data;

t_gps_data gps_data;

#define GPS_Frame_Buf_Size 100
uint8_t GPS_Frame_Buf[100];







/*******************************************************************************
1.中断负责把GPS串口数据保存到Bufd.recv，在合适的地方调用此函数开始解析。
2.调用本函数会自动关闭接收,然后处理，期间的GPS数据可以忽略。
3.在合适的地方打开接收。
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
** 函数名称：UART0_IRQHandler/RS232
** 函数描述：UART0中断处理函数
** 输入参数：无
** 返回值  ：无
** 
*********************************************************************************************************/
void UART0_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART0->IIR;

    uiIIRValue >>= 1;                                                     /* 中断标志寄存器值右移1位    */
    uiIIRValue &= 0x07;
	
	switch(uiIIRValue)
	{
		case IIR_RDA:													//接收数据有效中断(到达触发级14)
			
			if((LPC_UART0->LSR & 0x0E)==0)								//无OE、PE、FE传输错误(防止未及时处理此中断产生的溢出)
			{
				do
				{
					Bufd.recv[Bufd.rpos++]	=LPC_UART0->RBR;
				}
				while((LPC_UART0->LSR & 0x01)==0x01);				//判断数据就绪位
				
			}
			else
			{
				uiDummy=LPC_UART0->LSR;									//清除错误指示
				do{uiDummy=LPC_UART0->RBR;}while((LPC_UART0->LSR & 0x01)==0x01);	//清除多余数据
			}
			
			break;
			
		case IIR_CTI:													//接收超时中断
			
			//LED_POWER=!LED_POWER;
			if((LPC_UART0->LSR & 0x0E)==0)							//判断是否有OE、PE、FE底层位传输错误
			{
				do
				{
					Bufd.recv[Bufd.rpos++]	=LPC_UART0->RBR;
				}
				while ((LPC_UART0->LSR & 0x01)==0x01);				//判断数据就绪位
				
				get_gps_useful_data();
				
				if(GPS_Data_ok)							//GPS数据已有效
				{
					Bufd.rpos	=0;                                 //缓冲区指针复位

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

					if ((Gps.updaen)&&(Gpstime_err==0))								//如GPS更新使能
					{
						uint16_t y,m,d,w;

						Rtc.hour	=Gps.hour;						//发送给暂存变量
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
							//w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 -y / 100 +y / 400) % 7 + 1;  //基姆拉尔森计算公式
							w = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400+1)%7;

							Rtc.week = w;
						}


						Gps.wrtlog	=1;								//GPS更新时钟并写日志

						GPS_update_off();							//更新完后关闭使能

					}
				}	

			}
			else
			{
				uiDummy=LPC_UART0->LSR;									//清除错误指示		
				do{uiDummy=LPC_UART0->RBR;}while((LPC_UART0->LSR & 0x01)==0x01);	//清除多余数据
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
** 函数名称：UART1_IRQHandler
** 函数描述：UART1中断处理函数/S2E
** 输入参数：无
** 返回值  ：无
** 
*********************************************************************************************************/
void UART1_IRQHandler (void) 
{
    uint8_t uiIIRValue = 0, uiLSRValue = 0;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART1->IIR;

    uiIIRValue >>= 1;                                                     /* 中断标志寄存器值右移1位    */

    uiIIRValue &= 0x07;
	
	switch(uiIIRValue)
	{
		case IIR_RDA:													//接收数据有效中断(到达触发级14)
			
			if((LPC_UART1->LSR & 0x0E)==0)								//无OE、PE、FE传输错误(防止未及时处理此中断产生的溢出)
			{
				do
				{
					Netrev.buf[Netrev.pos++]	=LPC_UART1->RBR;
					
					if((ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_MAXNUM))		//如果是配置下载传输
					{
						memset(&Netrev,0,sizeof(NETRECV));				//复位Netrev结构体，方便中断再次接收(Netrev.ok=0)
						
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
						
						return;
					}	
					else if((!ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_NUM))
					{
						memset(&Netrev,0,sizeof(NETRECV));				//复位Netrev结构体，方便中断再次接收(Netrev.ok=0)
						
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
						
						return;
					}
				}
				while((LPC_UART1->LSR & 0x01)==0x01);				//判断数据就绪位
				
				if	((Netrev.buf[Netrev.pos-1]==0x7E)&&
					(Netrev.buf[Netrev.pos-2]==0xE7))					//如果触发级收到的最后一个字节是帧尾，则缓冲区完成标志置1
					Netrev.ok	=1;
				else
					Netrev.ok	=0;										//否则置0，即使之前已经存在有效帧
			}
			else
			{
				uiDummy=LPC_UART1->LSR;									//清除错误指示
				do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
			}
			
			break;
			
		case IIR_CTI:													//接收超时中断
			
			//LED_POWER=!LED_POWER;
			if((LPC_UART1->LSR & 0x0E)==0)							//判断是否有OE、PE、FE底层位传输错误
			{
				do
				{
					Netrev.buf[Netrev.pos++]	=LPC_UART1->RBR;			//RBR接收至接收缓冲区	
					//Netrev.pos=Netrev.pos+1;
					
					if((ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_MAXNUM))		//如果是配置下载传输
					{
						memset(&Netrev,0,sizeof(NETRECV));				//复位Netrev结构体，方便中断再次接收(Netrev.ok=0)
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
						return;
					}	
					else if((!ConfigDownLoadStartFlg)&&(Netrev.pos==NET_REV_NUM))
					{
						memset(&Netrev,0,sizeof(NETRECV));				//复位Netrev结构体，方便中断再次接收(Netrev.ok=0)
						do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
						return;
					}
				}
				while((LPC_UART1->LSR & 0x01)==0x01);			//判断数据就绪位
				
				if	((Netrev.buf[Netrev.pos-1]==0x7E)&&
					(Netrev.buf[Netrev.pos-2]==0xE7))					//如果触发级收到的最后一个字节是帧尾，则缓冲区完成标志置1
					Netrev.ok	=1;
				else
				{
					memset(&Netrev,0,sizeof(NETRECV));
					Netrev.ok	=0;										//否则置0，即使之前已经存在有效帧
				}
			}
			else
			{
				uiDummy=LPC_UART1->LSR;									//清除错误指示		
				do{uiDummy=LPC_UART1->RBR;}while((LPC_UART1->LSR & 0x01)==0x01);	//清除多余数据
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
** 函数名称：UART2_IRQHandler
** 函数描述：UART2中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART2_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART2->IIR;

    uiIIRValue >>= 1;                                                     /* 中断标志寄存器值右移1位    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* 接收线状态中断             */
        uiLSRValue = LPC_UART2->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART2Status = uiLSRValue;
            uiDummy = LPC_UART2->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* 接收FIFO不为空中断         */
            UART2Buffer[UART2Count] = LPC_UART2->RBR;
            UART2Count++;
            if ( UART2Count == BUFSIZE ) {
                UART2Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* 接收数据可用中断           */
        UART2Buffer[UART2Count] = LPC_UART2->RBR;
        UART2Count++;
        if ( UART2Count == BUFSIZE ) {
            UART2Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* 字符超时中断               */
        UART2Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* 发送保持寄存器空中断       */
        uiLSRValue = LPC_UART2->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART2TxEmpty = 1;
        } else {
            UART2TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** 函数名称：UART3_IRQHandler
** 函数描述：UART3中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART3_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART3->IIR;

    uiIIRValue >>= 1;                                                     /* 中断标志寄存器值右移1位    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* 接收线状态中断             */
        uiLSRValue = LPC_UART3->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART3Status = uiLSRValue;
            uiDummy = LPC_UART3->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* 接收FIFO不为空中断         */
            UART3Buffer[UART3Count] = LPC_UART3->RBR;
            UART3Count++;
            if ( UART3Count == BUFSIZE ) {
                UART3Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* 接收数据可用中断           */
        UART3Buffer[UART3Count] = LPC_UART3->RBR;
        UART3Count++;
        if ( UART3Count == BUFSIZE ) {
            UART3Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* 字符超时中断               */
        UART3Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* 发送保持寄存器空中断       */
        uiLSRValue = LPC_UART3->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART3TxEmpty = 1;
        } else {
            UART3TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** 函数名称：UART4_IRQHandler
** 函数描述：UART4中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART4_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART4->IIR;

    uiIIRValue >>= 1;                                                     /* 中断标志寄存器值右移1位    */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {                                        /* 接收线状态中断             */
        uiLSRValue = LPC_UART4->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART4Status = uiLSRValue;
            uiDummy = LPC_UART4->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {                                     /* 接收FIFO不为空中断         */
            UART4Buffer[UART4Count] = LPC_UART4->RBR;
            UART4Count++;
            if ( UART4Count == BUFSIZE ) {
                UART4Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {                                   /* 接收数据可用中断           */
        UART4Buffer[UART4Count] = LPC_UART4->RBR;
        UART4Count++;
        if ( UART4Count == BUFSIZE ) {
            UART4Count = 0;
        }
    }
    else if ( uiIIRValue == IIR_CTI ) {                                   /* 字符超时中断               */
        UART4Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {                                  /* 发送保持寄存器空中断       */
        uiLSRValue = LPC_UART4->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART4TxEmpty = 1;
        } else {
            UART4TxEmpty = 0;
        }
    }
}






/*********************************************************************************************************
** 函数名称：ModemInit
** 函数描述：Modem功能初始化（只有UART1拥有此功能）
** 输入参数：无
** 返回值  ：无
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
** 函数名称：UARTInit
** 函数描述：串口初始化
** 输入参数：PortNum    UART端口号
**           bandrate   设定的串口波特率
** 返回值  ：无
*********************************************************************************************************/
uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate )
{
    uint32_t Fdiv;

    if ( PortNum == 0 ) {
        LPC_SC->PCONP |= 0x00000008;
		
		LPC_IOCON->P0_2  &= ~0x07;
		LPC_IOCON->P0_2  |= 1;                                      /* P0.2为U0_TXD                 */
		LPC_IOCON->P0_3  &= ~0x07;
		LPC_IOCON->P0_3  |= 1;                                      /* P0.3为U0_RXD                 */
        /*
         * 8为数据位，无奇偶检验位，1位停止位
         */
        LPC_UART0->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*求除数值                      */
        LPC_UART0->DLM = Fdiv / 256;
        LPC_UART0->DLL = Fdiv % 256;
        LPC_UART0->LCR = 0x03;                                          /* 锁定除数访问                 */
        LPC_UART0->FCR = 0xC7;                                          /* 使能并复位FIFO   14字节触发  */

        
        /* 
         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
         */
		NVIC_EnableIRQ(UART0_IRQn);
		LPC_UART0->IER = IER_RBR | IER_RLS;//| IER_THRE
		
		//LPC_UART0->IER = IER_RBR;                                       /*仅使能接收数据可用中断及接收超时中断*/
        return (TRUE);
    }
    else if ( PortNum == 1 ) {
        LPC_SC->PCONP |= 0x00000010;
        
		LPC_IOCON->P0_15 &= ~0x07;
		LPC_IOCON->P0_15 |= 1;                                      /* P0.15为U1_TXD                */
		LPC_IOCON->P0_16 &= ~0x07;
		LPC_IOCON->P0_16 |= 1;                                      /* P0.16为U1_RXD                */

        /*
         * 8为数据位，无奇偶检验位，1位停止位
         */
        LPC_UART1->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*求除数值                      */
        LPC_UART1->DLM = Fdiv / 256;
        LPC_UART1->DLL = Fdiv % 256;
        LPC_UART1->LCR = 0x03;                                          /* 锁定除数访问                 */
        LPC_UART1->FCR = 0xC7;                                          /* 使能并复位FIFO               */

        NVIC_EnableIRQ(UART1_IRQn);
		//NVIC_SetPriority(UART1_IRQn,11);
        /* 
         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
         */
        LPC_UART1->IER = IER_RBR | IER_RLS;//| IER_THRE
		//LPC_UART1->IER = IER_RBR;                                       /*仅使能接收数据可用中断及接收超时中断*/
        return (TRUE);
    }
    else if ( PortNum == 2 ) {
        LPC_SC->PCONP |= 1ul<<24;
        
            LPC_IOCON->P0_10 &= ~0x07;
            LPC_IOCON->P0_10 |= 1;                                      /* P0.10为U2_TXD                */
            LPC_IOCON->P0_11 &= ~0x07;
            LPC_IOCON->P0_11 |= 1;                                      /* P0.11为U2_RXD                */
        /*
         * 8为数据位，无奇偶检验位，1位停止位
         */
        LPC_UART2->LCR = 0x83;        
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*求除数值                      */
        LPC_UART2->DLM = Fdiv / 256;
        LPC_UART2->DLL = Fdiv % 256;
        LPC_UART2->LCR = 0x03;                                          /* 锁定除数访问                 */
		LPC_UART2->LCR |= 0x04;											/* 两位停止位                 */
        LPC_UART2->FCR = 0xC7;                                          /* 使能并复位FIFO               */

		NVIC_EnableIRQ(UART2_IRQn);
		//NVIC_SetPriority(UART2_IRQn,12);
        /* 
         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
         */
		LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;
			//LPC_UART2->IER = IER_RBR;                                       /*仅使能接收数据可用中断及接收超时中断*/
        return (TRUE);
    }
    else if ( PortNum == 3 ) {
        LPC_SC->PCONP |= 0x02000000;

            LPC_IOCON->P4_28 &= ~0x07;
            LPC_IOCON->P4_28 |= 2;                                      /* P4.28为U3_TXD                */
            LPC_IOCON->P4_29 &= ~0x07;
            LPC_IOCON->P4_29 |= 2;                                      /* P4.29为U3_RXD                */

        /*
         * 8为数据位，无奇偶检验位，1位停止位
         */
        LPC_UART3->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*求除数值                      */
        LPC_UART3->DLM = Fdiv / 256;
        LPC_UART3->DLL = Fdiv % 256;
        LPC_UART3->LCR = 0x03;                                          /* 锁定除数访问                 */
				LPC_UART3->LCR |= 0x04;											/* 两位停止位                 */
        LPC_UART3->FCR = 0xC7;                                          /* 使能并复位FIFO               */
        
        NVIC_EnableIRQ(UART3_IRQn);
        /* 
         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
         */
        LPC_UART3->IER = IER_RBR  | IER_RLS; //| IER_THRE
		//LPC_UART3->IER = IER_RBR;                                       /*仅使能接收数据可用中断及接收超时中断*/
        return (TRUE);
    }
    else if ( PortNum == 4 ) {
        LPC_SC->PCONP |= 0x00000100;
        
            LPC_IOCON->P5_4  &= ~0x07;
            LPC_IOCON->P5_4  |= 4;                                      /* P5.4为U4_TXD                 */
            LPC_IOCON->P5_3  &= ~0x07;
            LPC_IOCON->P5_3  |= 4;                                      /* P5.3为U4_RXD                 */
       
        /*
         * 8为数据位，无奇偶检验位，1位停止位
         */
        LPC_UART4->LCR = 0x83;                          
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;                    /*求除数值                      */
        LPC_UART4->DLM = Fdiv / 256;
        LPC_UART4->DLL = Fdiv % 256;
        LPC_UART4->LCR = 0x03;                                          /* 锁定除数访问                 */
				LPC_UART4->LCR |= 0x04;											/* 两位停止位                 */
        LPC_UART4->FCR = 0xC7;                                          /* 使能并复位FIFO               */
        
        //NVIC_EnableIRQ(UART4_IRQn);
        /* 
         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
         */
        LPC_UART4->IER = IER_RBR | IER_RLS; //| IER_THRE
				//LPC_UART4->IER = IER_RBR;                                       /*仅使能接收数据可用中断及接收超时中断*/
        return (TRUE);
    }
    return( FALSE );
}


/*********************************************************************************************************
** 函数名称：UARTSend
** 函数描述：串口发送数据
** 输入参数：PortNum    UART端口号
**           BufferPtr  发送数据缓冲区
**           Length     发送数据长度
** 返回值  ：无
*********************************************************************************************************/
void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
    if ( portNum == 0 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART0->LSR & (1<<5)));                           /* 等待发送标志为1              */
            LPC_UART0->THR = *BufferPtr;                                /* 发送数据                     */
            UART0TxEmpty = 0;                                           /* 发送标志清0                  */
            BufferPtr++;                                                /* 指针后移                     */
            Length--;                                                   /* 发送数目减1                  */
        }
    }
    else if ( portNum == 1 ) {
        while ( Length != 0 ) {
           while ( !(LPC_UART1->LSR & (1<<5)));                           /* 等待发送标志为1              */
            LPC_UART1->THR = *BufferPtr;                                /* 发送数据                     */
            UART1TxEmpty = 0;                                           /* 发送标志清0                  */
            BufferPtr++;                                                /* 指针后移                     */
            Length--;                                                   /* 发送数目减1                  */
        }
    }
    else if ( portNum == 2 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART2->LSR & (1<<5)));                           /* 等待发送标志为1              */
            LPC_UART2->THR = *BufferPtr;                                /* 发送数据                     */
            UART2TxEmpty = 0;                                           /* 发送标志清0                  */
            BufferPtr++;                                                /* 指针后移                     */
            Length--;                                                   /* 发送数目减1                  */
        }
    }
    else if ( portNum == 3 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART3->LSR & (1<<5)));                           /* 等待发送标志为1              */
            LPC_UART3->THR = *BufferPtr;                                /* 发送数据                     */
            UART3TxEmpty = 0;                                           /* 发送标志清0                  */
            BufferPtr++;                                                /* 指针后移                     */
            Length--;                                                   /* 发送数目减1                  */
        }
    }
    else if ( portNum == 4 ) {
        while ( Length != 0 ) {
            while ( !(LPC_UART4->LSR & (1<<5)));                           /* 等待发送标志为1              */
            LPC_UART4->THR = *BufferPtr;                                /* 发送数据                     */
            UART4TxEmpty = 0;                                           /* 发送标志清0                  */
            BufferPtr++;                                                /* 指针后移                     */
            Length--;                                                   /* 发送数目减1                  */
        }
    }
    return;
}

/*********************************************************************************************************
** 函数名称：UART0DMA_Init
** 函数描述：串口1DMA初始化
** 输入参数：
**          
** 返回值  ：无
*********************************************************************************************************/
void UART_DMA_Init(uint8_t CHnum)
{
	LPC_SC->PCONP |= (1 << 29);                                         /* 开启DMA外设                  */
	LPC_SC->DMAREQSEL = 0x0000;                                         /* 选择DMA请求                  */
	LPC_GPDMA->IntTCClear |= 0x0F;                                       /* 清除通道1的终端计数中断     */
	LPC_GPDMA->IntErrClr  |= 0x0F;                                       /* 清除通道0～3的DMA错误中断   */
	LPC_GPDMA->Config = 0x01;                                           /* 使能DMA，字节顺序为小端      */
        
    while (!(LPC_GPDMA->Config & 0x01));
    
    NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);                                           /* 使能DMA中断请求              */
	
	switch (CHnum)
	{
		case 0:
		
			LPC_GPDMACH0->CLLI =	0;
			LPC_GPDMACH0->CConfig 	|= (0x01 << 11)	                                /* 初始化流的方向是为从存储器到 */
																					/* 外设以及流控制器为DMA控制器  */
									|  (0x00 << 1)                                     /* 指定DMA传输的源是存储器      */
									|  (0x01<<15)
									|  (0x0E<< 6);                                    /* 指定DMA传输的目标是UART2TX   */
			
			LPC_GPDMACH0->CControl	|= (0x02 << 12)		                               /* 指定源突发传输大小为8位      */
									|  (0x02 << 15)                                    /* 指定目的突发传输大小为8位    */
									|  (0x00 << 18)                                    /* 指定源传输数据宽度为8位      */
									|  (0x00 << 21);                                   /* 指定目标传输数据宽度为8位    */ 
			LPC_GPDMACH0->CSrcAddr  = (uint32_t)UART2Buffer;                 /* 通道1源地址                  */
			LPC_GPDMACH0->CDestAddr = (uint32_t)&LPC_UART2->THR; //0x40010000;               /* 通道1目的地址         */
			
			LPC_GPDMACH0->CControl  |= (1 << 26)		                                /* 指定源地址自动增加           */
									|  (0 << 27);		                                /* 指定目标地址不自动增加	    */
			LPC_GPDMACH0->CControl  |= 0x80000000;
		
		break;
		case 1:
		
			LPC_GPDMACH1->CLLI =	0;
			LPC_GPDMACH1->CConfig 	|= (0x01 << 11)	                                /* 初始化流的方向是为从存储器到 */
																					/* 外设以及流控制器为DMA控制器  */
									|  (0x00 << 1)                                     /* 指定DMA传输的源是存储器      */
									|  (0x01<<15)
									|  (0x0A << 6);                                    /* 指定DMA传输的目标是UART1TX   */
			
			LPC_GPDMACH1->CControl	|= (0x02 << 12)		                               /* 指定源突发传输大小为8位      */
									|  (0x02 << 15)                                    /* 指定目的突发传输大小为8位    */
									|  (0x00 << 18)                                    /* 指定源传输数据宽度为8位      */
									|  (0x00 << 21);                                   /* 指定目标传输数据宽度为8位    */ 
			LPC_GPDMACH1->CSrcAddr  = (uint32_t)UART1Buffer;                 /* 通道1源地址                  */
			LPC_GPDMACH1->CDestAddr = (uint32_t)&LPC_UART3->THR; //0x40010000;               /* 通道1目的地址         */
			
			LPC_GPDMACH1->CControl  |= (1 << 26)		                                /* 指定源地址自动增加           */
									|  (0 << 27);		                                /* 指定目标地址不自动增加	    */
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
			LPC_GPDMACH0->CControl |= (uiNum&0x0FFF);		                                        /* 指定突发传输次数             */

			/*
			 *  DMA传输启动 
			 */
			LPC_GPDMACH0->CConfig  |= 0x01;	 		                                    /* 使能DMA通道0                */
		break;
		case 1:
		
			UART3DMADone = 0;
			LPC_GPDMACH1->CControl |= (uiNum&0x0FFF);		                                        /* 指定突发传输次数             */

			/*
			 *  DMA传输启动 
			 */
			LPC_GPDMACH1->CConfig  |= 0x01;	 		                                    /* 使能DMA通道1                 */
		break;
		default:break;
	}
		
	
}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
