#include "bsp.h"

#define SOCK_UDP_FD        	 0
#define SOCK_UDP_PORT        49489

bool Flag_PCF8563_OK=false;
CAN_EV_TYPE ev_CanRespone=CAN_EV_None;
NET_EV_TYPE ev_NetRespone=NET_EV_None;

uint8_t Time_CanRespone_Count=0;
uint8_t Time_NetRespone_Count=0;

PCF8563_DATE     TimeAndDate;
Frame_DefType send_Frame,send_Frame2;
static bool Flag_CAN2_Respone=false;

//��ʼ��PCF8563
static void PCF8563Init(void)
{
	
	PCF8563_SS(Stop);                                                   /* ֹͣPCF8563                  */

    TimeAndDate.year     = 2017;                                        /* ��ʼ����                     */
    TimeAndDate.month    = 6;                                           /* ��ʼ����                     */
    TimeAndDate.date     = 16;                                           /* ��ʼ����                     */
    TimeAndDate.week     = 5;                                           /* ��ʼ����                     */
    TimeAndDate.hour     = 8;                                          /* ��ʼ��ʱ                     */
    TimeAndDate.minute   = 0;                                          /* ��ʼ����                     */
    TimeAndDate.second   = 0;                                          /* ��ʼ����                     */

    PCF8563_Set(&TimeAndDate);                                          /* ��ʼ������                   */
    PCF8563_Set_Timer(TimerOn|TimerClk64Hz,1);                          /* ���ö�ʱ��Ϊ128Hz    */
    PCF8563_INT_State(SetINT|TIE|TITP);                                 /* ʹ�ܶ�ʱ���ж�               */
	PCF8563_INT_State(ClearINT);                                        //����жϱ�־

	PCF8563_INT_Enable();
	
    PCF8563_SS(Start);                                                  /* ����PCF8563                  */
}
//CAN2ͨ�Ų�����Ӧ����֡
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
		
		Send_CAN_DataFrame(CAN_ID_2,send_Frame.module_id,(uint8_t *)(&send_Frame2),(uint8_t)sizeof(send_Frame2));//����һ֡
	}
}
//CAN������Ϣ����
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

//CPU��CAN�ӿڲ���
void CPU_CanTest(void)
{
	uint8_t can_test_num=0;
	while(1)
	{
		can_test_num++; //can���Դ����ۼ�
		
		send_Frame.module_id=TEST_CAN1_ID;
		send_Frame.fun_code=FUN_TX;
		send_Frame.dataH=CIRCUIT_CAN;
		send_Frame.dataL=0xff;
		
		send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
		Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//����һ֡
		
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
		
	Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//����һ֡
	
}

//�������socket����������������
void NET_UDP_SocketConfig(void)
{
	W5500_Config();
	W5500_UDP_SocketCreat(SOCK_UDP_FD,SOCK_UDP_PORT);
}
//����ػ����ԣ�����������
void NET_UDP_LoopBack(void)
{
	W5500_UDP_LoopBack(SOCK_UDP_FD,SOCK_UDP_PORT);
}
//CPU���������
void CPU_NetTest(void)
{
	
}
//RTCʱ�ӵ�·����
void CPU_PCF8563_Test(void)
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
		
	Send_CAN_DataFrame(CAN_ID_1,send_Frame.module_id,(uint8_t *)(&send_Frame),(uint8_t)sizeof(send_Frame));//����һ֡		
}

//�����У��
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
