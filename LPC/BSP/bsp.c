#include "bsp.h"

#define SOCK_UDP_FD        	 0
#define SOCK_UDP_PORT        49489

bool Flag_PCF8563_OK=false;
CAN_EV_TYPE ev_CanRespone=CAN_EV_None;
NET_EV_TYPE ev_NetRespone=NET_EV_None;
RS232_EV_TYPE ev_RS232Respone=RS232_EV_None;

uint8_t Time_CanRespone_Count=0;
uint32_t Time_NetRespone_Count=0;
uint32_t Time_RS232Respone_Count=0;


PCF8563_DATE     TimeAndDate;
Frame_DefType send_Frame,send_Frame2;
static bool Flag_CAN2_Respone=false;

//初始化PCF8563
static void PCF8563Init(void)
{
	
	PCF8563_SS(Stop);                                                   /* 停止PCF8563                  */

    TimeAndDate.year     = 2017;                                        /* 初始化年                     */
    TimeAndDate.month    = 6;                                           /* 初始化月                     */
    TimeAndDate.date     = 16;                                           /* 初始化日                     */
    TimeAndDate.week     = 5;                                           /* 初始化周                     */
    TimeAndDate.hour     = 8;                                          /* 初始化时                     */
    TimeAndDate.minute   = 0;                                          /* 初始化分                     */
    TimeAndDate.second   = 0;                                          /* 初始化秒                     */

    PCF8563_Set(&TimeAndDate);                                          /* 初始化日期                   */
    PCF8563_Set_Timer(TimerOn|TimerClk64Hz,1);                          /* 设置定时器为128Hz    */
    PCF8563_INT_State(SetINT|TIE|TITP);                                 /* 使能定时器中断               */
	PCF8563_INT_State(ClearINT);                                        //清除中断标志

	PCF8563_INT_Enable();
	
    PCF8563_SS(Start);                                                  /* 开启PCF8563                  */
}
//CAN2通信测试响应数据帧
static void CAN2_Respone_SendCheck(void)
{
	if(Flag_CAN2_Respone==true)
	{
		Flag_CAN2_Respone=false;
		send_Frame2.module_id=TEST_CAN2_ID;
		send_Frame2.fun_code=FUN_TX;
		send_Frame2.dataH=CIRCUIT_CAN;
		send_Frame2.dataL=0xff;
		
		send_Frame2.check_sum=DataCheckSum((uint8_t *)(&send_Frame2),4);
		
		Send_CAN_DataFrame(CAN_ID_2,send_Frame.module_id,(uint8_t *)(&send_Frame2),(uint8_t)sizeof(send_Frame2));//发送一帧
	}
}
//CAN接收消息处理
void CAN1_RxMessage_Deal(Frame_DefType * FrameRx)
{
	switch(FrameRx->fun_code)
	{
		case FUN_TX:
			if(FrameRx->module_id==TEST_CAN2_ID)
			{
				ev_CanRespone=CAN_EV_ResOk;
			}
			else ev_CanRespone=CAN_EV_ResFail;
			break;
		case FUN_RESULT:
			
			break;
		case FUN_END:
			
			break;
		default:
			ev_CanRespone=CAN_EV_ResFail;
			break;
	}
}
void CAN2_RxMessage_Deal(Frame_DefType * FrameRx)
{
	switch(FrameRx->fun_code)
	{
		case FUN_TX:
			if(FrameRx->module_id==TEST_CAN1_ID)
			{
				Flag_CAN2_Respone=true;
			}
			else ev_CanRespone=CAN_EV_ResFail;
			break;
		case FUN_RESULT:
			
			break;
		case FUN_END:
			
			break;
		default:
			ev_CanRespone=CAN_EV_ResFail;
			break;
	}
}

//CPU板CAN接口测试
void CPU_CanTest(void)
{
	uint8_t can_test_num=0;
	while(1)
	{
		can_test_num++; //can测试次数累加
		
		send_Frame.module_id=TEST_CAN1_ID;
		send_Frame.fun_code=FUN_TX;
		send_Frame.dataH=CIRCUIT_CAN;
		send_Frame.dataL=0xff;
		
		send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
		Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧
		
		Time_CanRespone_Count=0;
		ev_CanRespone=CAN_EV_ResReady;
		while(ev_CanRespone==CAN_EV_ResReady)
		{
			CAN2_Respone_SendCheck();
		}
		if(ev_CanRespone!=CAN_EV_ResTimeOut)
		{
//			fault_canTest=false;
			break;
		}
		else if(can_test_num>=3)
		{
//			fault_canTest=true;
			break;
		}
	}
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_RESULT;
	send_Frame.dataH=CIRCUIT_CAN;
	if( ev_CanRespone==CAN_EV_ResOk )send_Frame.dataL=0x01;
	else send_Frame.dataL=0x02;
	
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CPU_CAN_ID,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧
	
}


//CPU板网络测试
void CPU_NetTest(void)
{
	uint8_t test_num=0;
	int32_t state=-2;
	while(1)
	{
		test_num++;
		
		send_Frame.module_id=CPU_CAN_ID;
		send_Frame.fun_code=FUN_TX;
		send_Frame.dataH=CIRCUIT_W5500;
		send_Frame.dataL=0xff;
		
		send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
		W5500_UDP_Send(SOCK_UDP_FD,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));
		state=W5500_UDP_Rev(SOCK_UDP_FD,200);
		if(state==1)
		{
			break;
		}
		
		if(test_num>=3)
		{
			state=-1;
			break;
		}
	}
	if(state==1)
	{
		if( (gDATALEN==5) && (DataCheckSum(gDATABUF,4)==gDATABUF[4]) )
		{
			
			if( (gDATABUF[0]==CPU_CAN_ID) && (gDATABUF[1]==FUN_TX) && (gDATABUF[2]==CIRCUIT_W5500) && (gDATABUF[3]==0xff) )
			{
				state=1;
			}
			else 
			{
				state=2;
			}
		}
		else
		{
			state=2;
		}
	}
	else
	{
		state=2;
	}
	
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_RESULT;
	send_Frame.dataH=CIRCUIT_W5500;
	send_Frame.dataL=(uint8_t)state;
	
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CPU_CAN_ID,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧	
	
}
//CPU板RS232测试
void CPU_RS232Test(void)
{
	uint8_t test_num=0;
	
	while(1)
	{
		test_num++; //can测试次数累加
		
		send_Frame.module_id=CPU_CAN_ID;
		send_Frame.fun_code=FUN_TX;
		send_Frame.dataH=0;
		send_Frame.dataL=0xff;
		
		send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
		RS485_Send(0,(uint8_t *)(&send_Frame),(uint32_t)sizeof(send_Frame));//发送一帧
		
		Time_RS232Respone_Count=0;
		ev_RS232Respone=RS232_EV_ResReady;
		while(ev_RS232Respone==RS232_EV_ResReady)
		{
			RS232_1_Respone_SendCheck();
		}
		if(ev_RS232Respone!=RS232_EV_ResTimeOut)
		{
			break;
		}
		
		if(test_num>=3)
		{
			break;
		}
	}
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_RESULT;
	send_Frame.dataH=CIRCUIT_RS232;
	if( ev_RS232Respone==RS232_EV_ResOk )send_Frame.dataL=0x01;
	else send_Frame.dataL=0x02;
	
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CPU_CAN_ID,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧
}
//CPU板RS485测试
void CPU_RS485Test(void)
{
	uint8_t test_num=0;
	uint8_t state;
	while(1)
	{
		test_num++;
		
		RS485_SendTest();
		
		state=RS485_RevResult();
		
		if(state==1)
		{
			break;
		}
		
		if(test_num>=3)
		{
			break;
		}
		delay_ms(200);
	}
	
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_RESULT;
	send_Frame.dataH=CIRCUIT_RS485;
	send_Frame.dataL=state;
	
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CPU_CAN_ID,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧
}
//RTC时钟电路测试
void CPU_PCF8563Test(void)
{
	uint8_t state_PCF8563_test=1;
	PCF8563_DATE timeRead;
	
	Flag_PCF8563_OK=false;
	PCF8563_INT_Count=0;
	PCF8563Init();
	timer1Init();
	while(Flag_PCF8563_OK==false);
	
	PCF8563_INT_Disable();
	PCF8563_Read(&timeRead);
	
	if(timeRead.year!=TimeAndDate.year)
	{
		state_PCF8563_test=2;
	}
	else if(timeRead.month!=TimeAndDate.month)
	{
		state_PCF8563_test=2;
	}
	else if(timeRead.date!=TimeAndDate.date)
	{
		state_PCF8563_test=2;
	}
	else if(timeRead.week!=TimeAndDate.week)
	{
		state_PCF8563_test=2;
	}
	else if(timeRead.hour!=TimeAndDate.hour)
	{
		state_PCF8563_test=2;
	}
	else if(timeRead.minute!=TimeAndDate.minute)
	{
		state_PCF8563_test=2;
	}
	
	if(PCF8563_INT_Count<125)
	{
		state_PCF8563_test=2;
	}
	
	
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_RESULT;
	send_Frame.dataH=CIRCUIT_RTC;
	send_Frame.dataL=state_PCF8563_test;
		
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧		
}

//发送测试结束帧
void CPU_TestEnd(void)
{
	send_Frame.module_id=CPU_CAN_ID;
	send_Frame.fun_code=FUN_END;
	send_Frame.dataH=0xff;
	send_Frame.dataL=0xff;
	send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
	Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//发送一帧
}

//计算和校验
uint8_t DataCheckSum(uint8_t *data,uint8_t len)
{
	uint8_t sum=0;
	uint8_t i;
	for(i=0;i<len;i++)
	{
		sum+=data[i];
	}
	sum=0xff-sum;
	return sum;
}

//网络测试socket创建，主函数调用
void NET_UDP_SocketConfig(void)
{
	W5500_Config();
	W5500_UDP_SocketCreat(SOCK_UDP_FD,SOCK_UDP_PORT);
}

void CAN_Config(void)
{
	CAN_Init(CAN_ID_1,BPS_500K);
	CAN_wrFilter(CAN_ID_1,CPU_CAN_ID,STANDARD_FORMAT);
	CAN_wrFilter(CAN_ID_1,TEST_CAN2_ID,STANDARD_FORMAT);
	
	CAN_Init(CAN_ID_2,BPS_500K);
	CAN_wrFilter(CAN_ID_2,TEST_CAN1_ID,STANDARD_FORMAT);
	
	NVIC_EnableIRQ(CAN_IRQn);
}
