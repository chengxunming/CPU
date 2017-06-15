#include "bsp_include.h" 




/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void)  
{

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

	while (1) 
	{
		
	}
}
