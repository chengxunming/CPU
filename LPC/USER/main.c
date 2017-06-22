#include "bsp_include.h" 
#include "bsp.h" 
#include "I2CINT.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void)  
{
	__set_PRIMASK(0);//���ж�
	SystemInit();                                   /* initialize MCU clocks 100MHz*/
	delay_init();
	
	//�ܽ�����
	LPC_SC->PCONP |= (0x01 << 15);                                        //����GPIO���ʿ���
	//�������йܽ�����Ĭ������ʹ��
	LPC_GPIO0->DIR = 0x0000;
	LPC_GPIO1->DIR = 0x0000;
	LPC_GPIO2->DIR = 0x0000;
	LPC_GPIO3->DIR = 0x0000;
	LPC_GPIO4->DIR = 0x0000;
	
	CAN_Config();
////	NET_UDP_SocketConfig();
//	RS485_Config();
//	RS232_Config();
//	I2C0Init(100000);
//	PCF8563_INT_GPIO_Init();
	LED_Init();
	timer0Init();
	
	delay_ms(2000);
	CPU_CanTest();
//	CPU_NetTest();
//	CPU_RS232Test();
//	CPU_RS485Test();
//	CPU_PCF8563Test();
	LED_AllOn();
	CPU_TestEnd();
	while (1) 
	{

	}

//	unsigned char testcanMsg[8];
//    SystemInit();
//	delay_init();
//	
//    CAN_Init(0, BPS_500K);                                             /* CAN��ʼ��                     */
//    NVIC_EnableIRQ(CAN_IRQn);                                          /* ʹCAN�ж�                     */
//	
//	delay_ms(3000);
//	LED_AllOn();
//			memset(testcanMsg,0,8);
//		testcanMsg[0]=0x01;
//		testcanMsg[1]=0x03;
//		testcanMsg[2]=0xff;
//		testcanMsg[3]=0xff;
//		testcanMsg[4]=DataCheckSum(testcanMsg,4);
//    /*
//     *  д������Ϣ(�������ݳ��ȡ��Ƿ��׼֡��ID�������������ڵ�����) 
//     */
//    while(!writedetail(5,0, 0x00000001,testcanMsg));

//    while (1) {
//        CANSend(0, 2);
//        delay_ms(500);
//    }
}
