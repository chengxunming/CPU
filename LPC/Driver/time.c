#include "time.h"
#include "bsp_include.h"
/*********************************************************************************************************
** 函数名称：timer0Init  1ms
** 函数描述：定时器0初始化程序
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void timer0Init (void)
{
    LPC_SC->PCONP |= (1ul << 1);                                      /* 打开定时器0功率控制位       */
	LPC_TIM0->TCR  = 0x02;
    LPC_TIM0->IR   = 0x01;                                                /*清除中断                   */
    LPC_TIM0->CTCR = 0x00;
    LPC_TIM0->TC   = 0x00;
    LPC_TIM0->PR   = 0x00;
    LPC_TIM0->MR0  = PeripheralClock/1000;                              	/* 0.001S中断1次                  */
    LPC_TIM0->MCR  = 0x03;                                              /* 匹配后产生中断               */
    
    NVIC_EnableIRQ(TIMER0_IRQn);                                        /* 设置中断并使能               */
    NVIC_SetPriority(TIMER0_IRQn, 3);
    LPC_TIM0->TCR  = 0x01;                                              /* 启动定时器                   */
}

/*********************************************************************************************************
** 函数名称：timer1Init   1S
** 函数描述：定时器1初始化程序
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void timer1Init (void)
{
    LPC_SC->PCONP |= (1ul << 2);                                      /* 打开定时器1功率控制位       */
	LPC_TIM1->TCR  = 0x02;
    LPC_TIM1->IR   = 1;
    LPC_TIM1->CTCR = 0;
    LPC_TIM1->TC   = 0;
    LPC_TIM1->PR   = 0;
    LPC_TIM1->MR0  = PeripheralClock/1;                           	    /* 1S中断1次                  */
    LPC_TIM1->MCR  = 0x03;                                              /* 匹配后产生中断               */
    
    NVIC_EnableIRQ(TIMER1_IRQn);                                        /* 设置中断并使能               */
    NVIC_SetPriority(TIMER1_IRQn, 0);
    LPC_TIM1->TCR  = 0x01;                                              /* 启动定时器                   */
}


/*********************************************************************************************************
** 函数名称：timer2Init  200ms
** 函数描述：定时器2初始化程序/心跳专用
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void timer2Init (void)
{
    LPC_SC->PCONP |= (1ul << 22);                                      /* 打开定时器0功率控制位       */
	LPC_TIM2->TCR  = 0x02;
    LPC_TIM2->IR   = 0x01;                                                /*清除中断                   */
    LPC_TIM2->CTCR = 0x00;
    LPC_TIM2->TC   = 0x00;
    LPC_TIM2->PR   = 0x00;
    LPC_TIM2->MR0  = PeripheralClock/5;                              	/* 0.2S中断1次                  */
    LPC_TIM2->MCR  = 0x03;                                              /* 匹配后产生中断               */
    
    NVIC_EnableIRQ(TIMER2_IRQn);                                        /* 设置中断并使能               */
    NVIC_SetPriority(TIMER2_IRQn, 7);
    LPC_TIM2->TCR  = 0x00;                                              /* 启动定时器                   */
}


/*********************************************************************************************************
** 函数名称：TIMER0_IRQHandler
** 函数描述：TIMER0 中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void TIMER0_IRQHandler (void)
{
	LPC_TIM0->IR         = 0x01;                                        /* 清除中断标志                 */
	
	Time_NetRespone_Count++;

	//CAN通信测试 200ms超时
	if(ev_CanRespone==CAN_EV_ResReady)
	{
		Time_CanRespone_Count++;
		if(Time_CanRespone_Count>=200)
		{
			Time_CanRespone_Count=0;
			ev_CanRespone=CAN_EV_ResTimeOut;
		}
	}
	
	//RS232通信测试 200ms超时
	if(ev_RS232Respone==RS232_EV_ResReady)
	{
		Time_RS232Respone_Count++;
		if(Time_RS232Respone_Count>=200)
		{
			Time_RS232Respone_Count=0;
			ev_RS232Respone=RS232_EV_ResTimeOut;
		}
	}
	
}
/*********************************************************************************************************
** 函数名称：TIMER1_IRQHandler
** 函数描述：TIMER1 中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void TIMER1_IRQHandler (void)
{
	LPC_TIM1->IR         = 0x01;                                        /* 清除中断标志                 */
	
	Flag_PCF8563_OK=true;
	NVIC_DisableIRQ(TIMER1_IRQn);
	
}
/*********************************************************************************************************
** 函数名称：TIMER2_IRQHandler
** 函数描述：TIMER2 中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void TIMER2_IRQHandler (void)
{
	LPC_TIM2->IR         = 0x01;                                        /* 清除中断标志                 */
}

