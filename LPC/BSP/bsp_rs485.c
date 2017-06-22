#include "bsp_rs485.h"
#include "bsp_include.h"
#include "bsp.h"

#define RS485_RX	0
#define RS485_TX	1

static void RS485_DIR_GPIO_Init(void)
{
	//485 DIR U2
	LPC_GPIO1->DIR |= 1ul<<31;                                            /*����P1.31Ϊ���    */
    LPC_GPIO1->SET |= 1ul<<31;                                            /*����P1.31Ϊ�ߵ�ƽ  */
	//485 DIR U3
	LPC_GPIO1->DIR |= 1ul<<30;                                            /*����P1.30Ϊ���    */
    LPC_GPIO1->SET |= 1ul<<30;                                            /*����P1.30Ϊ�ߵ�ƽ  */
	//485 DIR U4
	LPC_GPIO1->DIR |= 1ul<<29;                                            /*����P1.29Ϊ���    */
    LPC_GPIO1->SET |= 1ul<<29;                                            /*����P1.29Ϊ�ߵ�ƽ  */
}

void Set485_Dir(uint8_t uartno,uint8_t dir)
{
	switch (uartno)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<31;                                            /*����P1.31Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<31;                                            /*����P1.31Ϊ�͵�ƽ  */
			break;
		case 3:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<30;                                            /*����P1.30Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<30;                                            /*����P1.30Ϊ�͵�ƽ  */
			break;
		case 4:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<29;                                            /*����P1.29Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<29;                                            /*����P1.29Ϊ�͵�ƽ  */
			break;
	default:
		break;
	}
}
//RS485��������
void RS485_Config(void)
{
	RS485_DIR_GPIO_Init();
	
	UARTInit(2,9600);				//UART2��ʼ����RS485ʹ��
	UARTInit(3,9600);				//UART3��ʼ����RS485ʹ��
	UARTInit(4,9600);				//UART4��ʼ����RS485ʹ��
}
//RS485����һ֡����
void RS485_Send( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
	Set485_Dir(portNum,RS485_TX);
	UARTSend(portNum,BufferPtr,Length);
	Set485_Dir(portNum,RS485_RX);
}

void RS485_SendTest(void)
{
	uint8_t sendbuf[5];
	UART2Count=0;
	UART3Count=0;
	UART4Count=0;
	memset(UART2Buffer,0,16);
	memset(UART3Buffer,0,16);
	memset(UART4Buffer,0,16);
	
	sendbuf[0]=CPU_CAN_ID;
	sendbuf[1]=FUN_TX;
	sendbuf[2]=2;
	sendbuf[3]=0xff;
	sendbuf[4]=DataCheckSum(sendbuf,4);
	RS485_Send(2,sendbuf,5);
	delay_ms(5);
	
	sendbuf[0]=CPU_CAN_ID;
	sendbuf[1]=FUN_TX;
	sendbuf[2]=3;
	sendbuf[3]=0xff;
	sendbuf[4]=DataCheckSum(sendbuf,4);
	RS485_Send(3,sendbuf,5);
	delay_ms(5);
	
	sendbuf[0]=CPU_CAN_ID;
	sendbuf[1]=FUN_TX;
	sendbuf[2]=4;
	sendbuf[3]=0xff;
	sendbuf[4]=DataCheckSum(sendbuf,4);
	RS485_Send(4,sendbuf,5);
	delay_ms(5);
}

uint8_t RS485_RevResult(void)
{
	uint8_t state=0;
	if( (UART2Count==10) && (UART3Count==10) && (UART4Count==10) )
	{
		if( (UART2Buffer[2]==3) && (UART2Buffer[7]==4) )
		{
			state=1;
		}
		else
		{
			state=2;
			return state;
		}
		
		if( (UART3Buffer[2]==2) && (UART3Buffer[7]==4) )
		{
			state=1;
		}
		else
		{
			state=2;
			return state;
		}
		
		if( (UART4Buffer[2]==2) && (UART4Buffer[7]==3) )
		{
			state=1;
		}
		else
		{
			state=2;
			return state;
		}
	}
	else 
	{
		state=2;
	}
	
	return state;
}



