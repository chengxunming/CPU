#include "bsp_include.h" 




/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void)  
{

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

	while (1) 
	{
		
	}
}
