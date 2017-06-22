#include "bsp_include.h" 
#include "bsp.h" 
#include "I2CINT.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void)  
{
	__set_PRIMASK(0);//开中断
	SystemInit();                                   /* initialize MCU clocks 100MHz*/
	delay_init();
	
	//管脚配置
	LPC_SC->PCONP |= (0x01 << 15);                                        //开启GPIO功率控制
	//设置所有管脚输入默认上拉使能
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
//    CAN_Init(0, BPS_500K);                                             /* CAN初始化                     */
//    NVIC_EnableIRQ(CAN_IRQn);                                          /* 使CAN中断                     */
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
//     *  写报文信息(报文数据长度、是否标准帧、ID、发送数据所在的数组) 
//     */
//    while(!writedetail(5,0, 0x00000001,testcanMsg));

//    while (1) {
//        CANSend(0, 2);
//        delay_ms(500);
//    }
}
