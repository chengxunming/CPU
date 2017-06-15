#include "delay.h"

//Systickʱ�� �Ĵ���
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CTRL--->���ƼĴ���
		BIT0 : Systickʹ��λ,		0-->�ر�Systick����				1-->����Systick����
		BIT1 : Systick�ж�ʹ��λ,	0-->�ر�Systick�ж�				1-->����Systick�ж�
		BIT2 : Systick ʱ��Դѡ��λ, 0-->ʹ��CCLK/8��ΪSystickʱ��	1-->ʹ��CCLK��ΪSystickʱ��
		BIT16: Systick�����Ƚϱ�־,������ϴζ�ȡ���Ĵ�����,SysTick�Ѿ�������0�����λΪ1;�����ȡ��λ,��λ���Զ�����
		 
LOAD--->���ؼĴ��� 
		Systick��һ���ݼ��Ķ�ʱ��,����ʱ���ݼ���0ʱ,���ؼĴ����е�ֵ�ͻᱻ��װ��,������ʼ�ݼ�,LOAD���ؼĴ����Ǹ�24λ�ļĴ���,������0xFFFFFF
		
VAL --->��ǰֵ�Ĵ���
		24λ�ļĴ���,��ȡʱ���ص�ǰ��������ֵ,д����ʹ֮����,ͬʱ���������SysTick���Ƽ�״̬�Ĵ����е�COUNTFLAG��־
		
CALIB--->У׼ֵ�Ĵ���
		BIT0-23 : TENMS,У׼ֵ,��ֵΪ0���޷�ʹ��У׼����
		BIT30 	: SKEW		0-->У׼ֵ��׼ȷ��1ms    1-->У׼ֵ����׼ȷ��1ms
		BIT31 	: NOREF 	0-->�ⲿ�ο�ʱ�ӿ���		1-->û���ⲿ�ο�ʱ�ӣ�STCLK�����ã�
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/    


static uint32_t fac_ms,fac_us;

void delay_init(void)
{
	uint32_t SysClkMhz_Val;
	
	SysTick->CTRL &= ~( ((uint32_t)SysTick_CTRL_ENABLE_Msk) | ((uint32_t)SysTick_CTRL_TICKINT_Msk) );//�ر�Systick���ܺ�Systick�ж�
	
	SysTick->CTRL |= (uint32_t)SysTick_CTRL_CLKSOURCE_Msk;
	SysClkMhz_Val=SystemCoreClock/1000000;
	
	fac_us=SysClkMhz_Val;
	fac_ms=fac_us*1000;
}

//��ʱnms
//nms��λΪms
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;	
	uint16_t count_100ms;
	
	SysTick->RELOAD=fac_ms-1;//ʱ�����1ms(SysTick->LOADΪ24bit)
	for(count_100ms=0;count_100ms<nms;count_100ms++)
	{
		SysTick->CURR =0x00;           //��ռ�����
		SysTick->CTRL |= ((uint32_t)SysTick_CTRL_ENABLE_Msk);      //��ʼ���� 
		do
		{
			temp=SysTick->CTRL;
		}
		while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
		SysTick->CTRL &= ~((uint32_t)SysTick_CTRL_ENABLE_Msk);     //�رռ�����	
	}
	SysTick->CURR =0x00;       //��ռ�����
	
		  	    
}   
//��ʱnus
//nusΪҪ��ʱ��us��.	���Ϊ167000us	    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	
	
	SysTick->RELOAD=nus*fac_us-1; //ʱ�����	  		 
	SysTick->CURR=0x00;        //��ռ�����
	SysTick->CTRL |= ((uint32_t)SysTick_CTRL_ENABLE_Msk);      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~((uint32_t)SysTick_CTRL_ENABLE_Msk);     //�رռ�����
	SysTick->CURR =0x00;       //��ռ�����	 
}



