#include "bsp_rs232.h"
#include "bsp.h"

static bool Flag_RS232_1_Respone=false;

void RS232_Config(void)
{
	UARTInit(0,9600);				//UART0初始化，RS232使用
	UARTInit(1,9600);				//UART1初始化，RS232使用
}

//RS485发送一帧数据
void RS232_Send( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
	UARTSend(portNum,BufferPtr,Length);
}

void RS232_1_Respone_SendCheck(void)
{
	Frame_DefType send_Frame;
	if(Flag_RS232_1_Respone==true)
	{
		Flag_RS232_1_Respone=false;
		send_Frame.module_id=CPU_CAN_ID;
		send_Frame.fun_code=FUN_TX;
		send_Frame.dataH=1;
		send_Frame.dataL=0xff;
		
		send_Frame.check_sum=DataCheckSum((uint8_t *)(&send_Frame),4);
		
		RS232_Send(1,(uint8_t *)(&send_Frame),(uint32_t)sizeof(send_Frame));//发送一帧
	}
}
//UART0串口接收数据处理
void get_UAET0_useful_data(uint8_t *BufferPtr, uint32_t Length)
{
	uint8_t sumCheck=0;
	if(Length!=5)
	{
		ev_RS232Respone=RS232_EV_ResFail;
		return;
	}
	
	sumCheck=DataCheckSum(BufferPtr,4);
	if(sumCheck!=BufferPtr[4])
	{
		ev_RS232Respone=RS232_EV_ResFail;
		return;
	}
	
	if( (BufferPtr[0]==1) && (BufferPtr[1]==1) && (BufferPtr[2]==1) && (BufferPtr[3]==0xff) )
	{
		ev_RS232Respone=RS232_EV_ResOk;
	}
	else
	{
		ev_RS232Respone=RS232_EV_ResFail;
	}	
}
//UART1串口接收数据处理
void get_UAET1_useful_data(uint8_t *BufferPtr, uint32_t Length)
{
	uint8_t sumCheck=0;
	if(Length!=5)
	{
		ev_RS232Respone=RS232_EV_ResFail;
		return;
	}
	
	sumCheck=DataCheckSum(BufferPtr,4);
	if(sumCheck!=BufferPtr[4])
	{
		ev_RS232Respone=RS232_EV_ResFail;
		return;
	}
	
	if( (BufferPtr[0]==1) && (BufferPtr[1]==1) && (BufferPtr[2]==0) && (BufferPtr[3]==0xff) )
	{
		Flag_RS232_1_Respone=true;
	}
	else
	{
		ev_RS232Respone=RS232_EV_ResFail;
	}	
}
