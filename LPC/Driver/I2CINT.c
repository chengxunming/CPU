/****************************************Copyright (c)****************************************************
** File name:                  I2CINT.c
** Latest modified Date:       2014-4-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Qinqin
** Created date:               2014-4-8
** Version:
** Descriptions:               LPC17系列芯片硬件I2C软件包。主程序要配置好I2C总线接口(I2C引脚功能和
**                             I2C中断，并已使能I2C主模式)
**                             包含原MCU TWI.C文件的功能
*********************************************************************************************************/
#include "../User_code/global.h"
                                                                                  /* 定义用于和I2C中断传递信息的   */                                                                       /* 全局变量                     */
volatile uint8_t     I2C0_sla;                                             /* I2C器件从地址                 */
volatile uint32_t    I2C0_suba;                                           /* I2C器件内部子地址             */
volatile uint8_t     I2C0_suba_num ;                                  /* I2C子地址字节数               */
volatile uint8_t     *I2C0_buf;                                           /* 数据缓冲区指针                */
volatile uint32_t    I2C0_num;                                             /* 要读取/写入的数据个数         */
volatile uint8_t     I2C0_end;                                             /* I2C总线结束标志：结束总线是置1*/
volatile uint8_t     I2C0_suba_en;                                     /*  子地址控制。
																				  0--子地址已经处理或者不需要子地址
                                                                                  1--读取操作
                                                                                  2--写操作                        */
//原TWI.C中变量
void cntdn_black(void);
void cntdn8(void);


//变量仅本文件使用
static	uint8_t	TWIIndex;					//缓冲数组指针
static	uint8_t ComModule;					//当前通讯模块
static	uint8_t	TWICheckSum;				//校验和
static	uint8_t	PatternIndex;				//协议内容指针

static  volatile bool 	TWIfinishflg;				//TWI通讯结束标志


//全局变量定义
uint8_t	FailTWI[4];					//TWI故障模块

//PCU
uint8_t	TPCUSize;					//MCU发送至PCU数据长度
uint8_t	RPCUSize;					//MCU接收PCU数据长度
uint8_t	TPCU[MAX_PCU_SIZE_T];		//MCU发送至PCU缓冲数组
signed char	RPCU[MAX_PCU_SIZE_R];	//MCU接收PCU缓冲数组


//EIU
TWIEIU	TEIU;						//EIU通讯结构体声明

//MCM
uint8_t	TMCMSize;					//MCM
uint8_t	RMCMSize;
uint8_t	TMCM[MAX_MCM_SIZE_T];
uint8_t	RMCM[MAX_MCM_SIZE_R];


uint8_t CountDownCode[17]={0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x09,0x11,0xC1,0x63,0x85,0x61,0x71,0xFD};


/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            软件延时，提供给I2C总线等待超时使用
** input parameters:        dly        延时参数，值越大，延时越久
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
uint8_t  Wait_I2c_End(uint32_t  dly)
{  
	uint32_t  i;
	if( I2C0_end==1 ) return (1);
	for(; dly>0; dly--)
	for(i=0; i<5000; i++)
	{
		if( I2C0_end==1 ) return (1);
	}
	return (0);
	
    
}

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        向无子地址器件发送1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  ISendByte(uint8_t sla, uint8_t dat)
{                                                                       /* 参数设置                     */

	I2C0_sla     = sla;                                                  /* 写操作的器件地址             */
	I2C0_buf     = &dat;                                                 /* 待发送的数据                 */
	I2C0_num     = 1;                                                    /* 发送1字节数据                */
	I2C0_suba_en = 0;                                                    /* 无子地址                     */
	I2C0_end     = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */

	return( Wait_I2c_End(20));	
}

/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        向无子地址器件读取1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  IRcvByte(uint8_t sla, uint8_t *dat)
{
	I2C0_sla     = sla+1;                                                  /* 读操作的器件地址             */
	I2C0_buf     = dat;                                                 /* 待发送的数据                 */
	I2C0_num     = 1;                                                    /* 发送1字节数据                */
	I2C0_suba_en = 0;                                                    /* 无子地址                     */
	I2C0_end     = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */

	return( Wait_I2c_End(20));
}

/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        从有子地址器件任意地址开始读取N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           数据接收缓冲区指针
**                      num         读取的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t I2C_ReadNByte (uint8_t sla, uint32_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
	if (num > 0)                                                        /* 判断num个数的合法性          */
	{                                                                   /* 参数设置                     */
		if (suba_type == 1)
		{                                                               /* 子地址为单字节               */
			I2C0_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
			I2C0_suba        = suba;                                     /* 器件子地址                   */
			I2C0_suba_num    = 1;                                        /* 器件子地址为1字节            */
		}
		if (suba_type == 2)
		{                                                               /* 子地址为2字节                */
			I2C0_sla         = sla + 1;                                  /* 读器件的从地址，R=1          */
			I2C0_suba        = suba;                                     /* 器件子地址                   */
			I2C0_suba_num    = 2;                                        /* 器件子地址为2字节            */
		}
		if (suba_type == 3)
		{                                                               /* 子地址结构为8+X              */
			I2C0_sla         = sla + ((suba >> 7 )& 0x0e) + 1;           /* 读器件的从地址，R=1          */
			I2C0_suba        = suba & 0x0ff;                             /* 器件子地址                   */
			I2C0_suba_num    = 1;                                        /* 器件子地址为8+x              */
		}
		I2C0_buf     = s;                                                /* 数据接收缓冲区指针           */
		I2C0_num     = num;                                              /* 要读取的个数                 */
		I2C0_suba_en = 1;                                                /* 有子地址读                   */
		I2C0_end     = 0;
		LPC_I2C0->CONCLR = (1 << 2)|                                    /* 清除AA                       */
						   (1 << 3)|                                    /* 清除SI                       */
						   (1 << 5);                                    /* 清除STA                      */
		LPC_I2C0->CONSET = (1 << 5)|                                    /* 置位STA                      */
						   (1 << 6);                                    /* I2C使能                      */
		return( Wait_I2c_End(20));                                      /* 等待I2C操作完成              */
	}
	return (FALSE);
	
	
	
}

/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        向有子地址器件写入N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           将要写入的数据的指针
**                      num         将要写入的数据的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
    if (num > 0)                                                        /* 判断num个数的合法性          */
    {                                                                   /* 设置参数                     */
        if (suba_type == 1)
        {                                                               /* 子地址为单字节               */
            I2C0_sla         = sla;                                      /* 读器件的从地址               */
            I2C0_suba        = suba;                                     /* 器件子地址                   */
            I2C0_suba_num    = 1;                                        /* 器件子地址为1字节            */
        }
        if (suba_type == 2)
        {                                                               /* 子地址为2字节                */
            I2C0_sla         = sla;                                      /* 读器件的从地址               */
            I2C0_suba        = suba;                                     /* 器件子地址                   */
            I2C0_suba_num    = 2;                                        /* 器件子地址为2字节            */
        }
        if (suba_type == 3)
        {                                                               /* 子地址结构为8+X              */
            I2C0_sla         = sla + ((suba >> 7 )& 0x0e);               /* 读器件的从地址               */
            I2C0_suba        = suba & 0x0ff;                             /* 器件子地址                   */
            I2C0_suba_num    = 1;                                        /* 器件子地址为8+X              */
        }
        I2C0_buf     = s;                                                /* 数据                         */
        I2C0_num     = num;                                              /* 数据个数                     */
        I2C0_suba_en = 2;                                                /* 有子地址，写操作             */
        I2C0_end     = 0;
        LPC_I2C0->CONCLR = (1 << 2)|                                    /* 清除AA标志位                 */
                           (1 << 3)|                                    /* 清除SI标志位                 */
                           (1 << 5);                                    /* 清除STA标志位                */
                                                                        /* 置位STA,启动I2C总线          */
        LPC_I2C0->CONSET = (1 << 5)|                                    /* STA                          */
                           (1 << 6);                                    /* I2CEN                        */
        return( Wait_I2c_End(20));                                      /* 等待I2C操作完成              */
    }
    return (FALSE);
}



/*********************************************************************************************************
** Function name:       I2C0_IRQHandler
** Descriptions:        硬件I2C中断服务程序。
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
void  I2C0_IRQHandler(void)
{   /*
     *读取I2C状态寄存器I2DAT,按照全局变量的设置进行操作及设置软件标志,清除中断逻辑,中断返回
     */
    switch (LPC_I2C0->STAT & 0xF8)
    {
           /*
             根据状态码进行相应的处理
           */
      	case 0x00:   
        LPC_I2C0->CONCLR = 0x28; 
        I2C0_end = 0xFF; 
        break; 
		case 0x08:    /* 已发送起始条件,主发送和主接收都有,装入SLA+W或者SLA+R */
        if(I2C0_suba_en == 1)/* SLA+R */                                 /* 指定子地址读                 */
        {
           LPC_I2C0->DAT = I2C0_sla & 0xFE;                              /* 先写入地址                   */
        }
        else                                                            /* SLA+W                        */
        {   LPC_I2C0->DAT = I2C0_sla;                                    /* 否则直接发送从机地址         */
        }
                                                                        /* 清零SI位                     */
        LPC_I2C0->CONCLR = (1 << 3)|                                    /* SI                           */
                           (1 << 5);                                    /* STA                          */
        break;
        case 0x10:    /*已发送重复起始条件 */                           /* 主发送和主接收都有           */
                                                                        /* 装入SLA+W或者SLA+R           */
        LPC_I2C0->DAT = I2C0_sla;                                        /* 重起总线后，重发从地址       */
        LPC_I2C0->CONCLR = 0x28;                                        /* 清零SI,STA                   */
        break;
        case 0x18:
        case 0x28:                                                      /* 已发送I2DAT中的数据,已接收ACK*/
        if (I2C0_suba_en == 0)
        {
            if (I2C0_num > 0)
            {   LPC_I2C0->DAT = *I2C0_buf++;
                LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
                I2C0_num--;
            }
            else                                                        /* 没有数据发送了               */
            {                                                           /* 停止总线                     */
                LPC_I2C0->CONSET = (1 << 4);                            /* STO                          */
                LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
                I2C0_end = 1;                                            /* 总线已经停止                 */
            }
        }
        if(I2C0_suba_en == 1)                                            /* 若是指定地址读,则重新启动总线*/
        {
            if (I2C0_suba_num == 2)
            {   LPC_I2C0->DAT = ((I2C0_suba >> 8) & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
                I2C0_suba_num--;
                break;
            }
            if(I2C0_suba_num == 1)
            {   LPC_I2C0->DAT = (I2C0_suba & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
                I2C0_suba_num--;
                break;
            }
            if (I2C0_suba_num == 0)
            {
                LPC_I2C0->CONCLR = 0x08;
                LPC_I2C0->CONSET = 0x20;
                I2C0_suba_en = 0;                                        /* 子地址己处理                 */
                break;
            }
        }
        if (I2C0_suba_en == 2)                                           /* 指定子地址写,子地址尚未指定, */
                                                                        /* 则发送子地址                 */
        {
            if (I2C0_suba_num > 0)
            {    if (I2C0_suba_num == 2)
                {   LPC_I2C0->DAT = ((I2C0_suba >> 8) & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C0_suba_num--;
                    break;
                }
                if (I2C0_suba_num == 1)
                {   LPC_I2C0->DAT    = (I2C0_suba & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C0_suba_num--;
                    I2C0_suba_en  = 0;
                    break;
                }
            }
        }
        break;
        case 0x40:                                                      /* 已发送SLA+R,已接收ACK        */
        if (I2C0_num <= 1)                                               /* 如果是最后一个字节           */
        {    LPC_I2C0->CONCLR = 1 << 2;                                       /* 下次发送非应答信号           */
        }
        else
        {    LPC_I2C0->CONSET = 1 << 2;                                 /* 下次发送应答信号             */
        }
        LPC_I2C0->CONCLR = 0x28;                                        /* 清零SI,STA                   */
        break;
        case 0x20:                                                      /* 已发送SLA+W,已接收非应答     */
        case 0x30:                                                      /* 已发送I2DAT中的数据，已接收  */
                                                                        /* 非应答                       */
        case 0x38:                                                      /* 在SLA+R/W或数据字节中丢失仲  */
                                                                        /* 裁                           */
        case 0x48:                                                      /* 已发送SLA+R,已接收非应答     */
        LPC_I2C0->CONCLR = 0x28;
        I2C0_end = 0xFF;
        break;
        case 0x50:                                                      /* 已接收数据字节，已返回ACK    */
        *I2C0_buf++ = LPC_I2C0->DAT;
        I2C0_num--;
        if (I2C0_num == 1)                                               /* 接收最后一个字节             */
        {
           LPC_I2C0->CONCLR = 0x2C;                                     /* STA,SI,AA = 0                */
        }
        else
        {   LPC_I2C0->CONSET = 0x04;                                    /* AA=1                         */
            LPC_I2C0->CONCLR = 0x28;
        }
        break;
        case 0x58:                                                      /* 已接收数据字节，已返回非应答 */
        *I2C0_buf++ = LPC_I2C0->DAT;                                     /* 读取最后一字节数据           */
        LPC_I2C0->CONSET = 0x10;                                        /* 结束总线                     */
        LPC_I2C0->CONCLR = 0x28;
        I2C0_end = 1;
        break;
        default:
        break;
    }
}

void overtimetwi_start(void)
{
	LPC_SC->PCONP |= (1 << 22);                                      /* 打开定时器3功率控制位       */  
	LPC_TIM2->TCR  = 0x02;
    LPC_TIM2->IR   = 0x01;                                                /*清除中断                   */
    LPC_TIM2->CTCR = 0x00;
    LPC_TIM2->TC   = 0x00;
    LPC_TIM2->PR   = 0x00;
    LPC_TIM2->MR0  = (PeripheralClock/28);                              /* 约35ms中断1次                  */
    LPC_TIM2->MCR  = 0x03;                                              /* 匹配后产生中断               */
    
   NVIC_EnableIRQ(TIMER2_IRQn);                                        /* 设置中断并使能               */
    NVIC_SetPriority(TIMER2_IRQn, 3);
    LPC_TIM2->TCR  = 0x01;                                              /* 启动定时器                   */
	
	/*TCCR3B 	= 0x00;													//关闭定时器3
	uint8_t sreg	=SREG;
	_CLI();
	TCNT3	= 0xFDDE;												//35MS
	SREG	= sreg;													//原子操作
	TIFR3   = 0x01;													//清除中断标志位
	TIMSK3 	= (1<<TOIE3);											//定时器3溢出中断使能
	TCCR3B 	= 0x05;													//启动定时器3，分频器值1024*/
}

void overtimetwi_close(void)
{
	LPC_TIM2->TCR  = 0x01;                                              /* 关闭定时器3                   */
	LPC_TIM2->MCR  = 0x00;                                              /* 屏蔽匹配后产生中断            */
	//TCCR3B = 0; 														//关闭定时器3
	//TIMSK3 = 0;															//定时器3溢出中断使能关闭
}



/*---------------------------------TWI发送开始--------------------------------*/
void TWI_SendStart(void) 
{ 
    
  	LPC_I2C1->CONCLR = (1 << 2)|                                    /* 清除AA                       */
	                   (1 << 3)|                                    /* 清除SI                       */
					   (1 << 5);                                    /* 清除STA                      */
	LPC_I2C1->CONSET = (1 << 5)|                                    /* 置位STA                      */
					   (1 << 6);                                    /* I2C使能                      */ 
	//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT)|(1<<TWSTA);
} 


/*-----------------------发送停止条件，保持TWEA以便从接收---------------------*/
void TWI_SendStop(void) 
{ 
	LPC_I2C1->CONCLR = (1<<2) |
	(1 << 3)|					
	(1 << 5);                                    /* 清除STA                      */
	LPC_I2C1->CONSET = (1 << 2)|                                    /* 置位AA                       */
					   (1 << 4)|                                    /* 置位STO                      */
					   (1 << 6);                                    /* I2C使能                      */ 
	
	//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT)|(1<<TWEA)|(1<<TWSTO); 
} 


/*--------------------------------TWI单字节发送-------------------------------*/
void TWI_SendByte(const uint8_t data) 
{ 
	
	//LPC_I2C1->DAT	=data;  																// 装载数据到 LPC_I2C1->DAT 
	LPC_I2C1->DAT =data;
	LPC_I2C1->CONSET =  0x04;                                 /* 置位AA                       */
	LPC_I2C1->CONCLR = 0x28; 
					   
	
					                                       /* I2C使能                      */ 
	
	//TWCR	=TWCR & TWCR_CMD_MASK |(1<<TWINT);  									// 发送开始
} 


//TWI通讯启动，并等待，有时控35MS,约13MS完成TWI通讯
void TWIStartAndWait(void)
{
	uint8_t retrycount=0;
	do
	{
		TWI_SendStart();														//启动TWI通讯
		
		overtimetwi_start();
		
		TWIfinishflg	=0;
		while(TWIfinishflg==0)													//等待TWI通讯结束
		{
			if(OverTimeFlg)
			{			
				OverTimeFlg =0;													//超时退出等待
				break;
			}
		}
		
		overtimetwi_close();
		
		if(TWIfinishflg==0)														//如果未成功
		{
			retrycount++;
			if(retrycount==4)													//重试3次不成功，则退出
			{
				FailTWI[BUST]	=1;												//可能由于总线锁死，无法通讯
				break;
			}
			else
			{
				TWIBus_init();
			}
		}
		else
			break;																//成功则直接退出
	}
	while(1);
}
	
	
/****************************************************

* 名称：	TWIBus_init

* 功能：	TWI总线初始化函数

* 入口参数：无

* 出口：	无

* 备注:		MCU与PCU,EIU,MCM通讯前进行TWI总线初始化

****************************************************/

void TWIBus_init(void) 
{ 
	
	NVIC_DisableIRQ(I2C1_IRQn);
	LPC_I2C1->CONCLR = 0x6C;
    LPC_I2C1->CONSET = 0x40;                                            /* 使能主机模式I2C*/
	NVIC_EnableIRQ(I2C1_IRQn);                                          /* 使能I2C中断                  */
}




//PCU发送数组准备
void PCU_buf(void)
{
	TPCU[0]	=(PCU.ModuleRst<<0)|(PCU.TVFrd<<2)|(PCU.Illum<<4)|(PCU.LampLevel<<5)|(PCU.LampSwitch<<7);
	
	
	PCU.ModuleRst	=NO_RST;		//对变量复位
}


//MCM发送数组准备
void MCM_buf(void)
{
	TMCM[0]	=(MCM.AllRed<<0)|(MCM.KeypadStatus<<1)|(MCM.TSCFault<<2)|(MCM.LocalRemote<<3)|(MCM.RunMode<<5);
	TMCM[1]	=MCM.CurrentStage;
}


//PCU初始化参数赋固定值
void PCU_initial(void)
{
	PCU.ModuleRst	=NO_RST;												//外灯打开，无暗灯，自动照明，读电压频率温度，无复位
	PCU.TVFrd		=RD_ALL;
	PCU.Illum		=AT_ILLUM;
	PCU.LampLevel	=SIGLEVEL1;
	PCU.LampSwitch	=LAMPON;
	
	PCU_buf();
	
	TPCUSize=1;
}


void EIU_initial(void)
{
	TEIU.tsize1=TEIU.tsize2=TEIU.tsize3=TEIU.tsize4=0;						//数据长度为0（无数据） 
}


void MCM_initial(void)
{
	MCM.AllRed			=0;													//NORMAL模式，本地MCM，无错误，键盘关闭
	MCM.KeypadStatus	=KP_OFF;
	MCM.TSCFault		=KP_NOFAULT;
	MCM.LocalRemote		=KP_LOCAL;
	MCM.RunMode			=KP_MODE_NORMAL;
	MCM.CurrentStage	=0;
	
	MCM_buf();

	TMCMSize=2;
	RMCMSize=1;
}


void select_commodule(const uint8_t module)
{
	if(module==COMPCU)
		ComModule=COMPCU;
	else if(module==COMEIU)									//每秒都轮询EIU和MCM，即使EIU和MCM有故障
		ComModule=COMEIU;
	else if(module==COMMCM)
		ComModule	=COMMCM;
}

/****************************************************

* 名称：	TWI_initial

* 功能：	TWI总线设备初始化函数

* 入口参数：无

* 出口：	无

* 备注:		MCU与PCU,EIU,MCM首次通讯

****************************************************/

void TWI_initial(void)
{
	PCU_initial();															//各模块通讯参数初始赋值
	EIU_initial();
	MCM_initial();
	
	select_commodule(COMPCU);												//按优先顺序选择当前通讯模块
	
//	TWIStartAndWait();
}


void TWI_buf(void)
{
	PCU_buf();																//PCU模块缓冲数组赋值
	
	cntdn8();		                                                        //两阶段流8块倒计时缓冲数组赋值EIU模块
	
	MCM_buf();																//MCM模块缓冲数组赋值
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
				LPC_GPIO1->SET |= 1ul<<31;                                            /*设置P1.31为高电平  */
			else
				LPC_GPIO1->CLR |= 1ul<<31;                                            /*设置P1.31为低电平  */
			break;
		case 3:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<30;                                            /*设置P1.30为高电平  */
			else
				LPC_GPIO1->CLR |= 1ul<<30;                                            /*设置P1.30为低电平  */
			break;
		case 4:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<29;                                            /*设置P1.29为高电平  */
			else
				LPC_GPIO1->CLR |= 1ul<<29;                                            /*设置P1.29为低电平  */
			break;
	default:
		break;
	}
}

void Cntd_send(void)
{
	uint8_t SendA[16],SendB[16];
	
	//A通道发送第一个路口四个方向的倒计时数据
	for(uint8_t i=0;i<4;i++)
	{
		SendA[i*3]=0x50+i;
		if(TEIU.tbuf1[i*2])
		{
			SendA[i*3+1]=(CountDownCode[TEIU.tbuf1[i*2]/10]&0xFE)|((TEIU.tbuf1[i*2+1]>>1)&0x01);
			SendA[i*3+2]=(CountDownCode[TEIU.tbuf1[i*2]%10]&0xFE)|(TEIU.tbuf1[i*2+1]&0x01);
		}
		else
		{
			SendA[i*3+1]=0xFC|((TEIU.tbuf1[i*2+1]>>1)&0x01);
			SendA[i*3+2]=0xFC|(TEIU.tbuf1[i*2+1]&0x01);
		}
	}
	Set485_Dir(2,RS485_TX);
	UARTSend(2,SendA,12);
	memcpy(UART2Buffer,SendA,12);
	
	//B通道发送第二个路口四个方向的倒计时数据
	for(uint8_t i=0;i<4;i++)
	{
		SendB[i*3]=0x50+i;
		if(TEIU.tbuf1[(i+4)*2])
		{
			SendB[i*3+1]=(CountDownCode[TEIU.tbuf1[(i+4)*2]/10]&0xFE)|((TEIU.tbuf1[(i+4)*2+1]>>1)&0x01);
			SendB[i*3+2]=(CountDownCode[TEIU.tbuf1[(i+4)*2]%10]&0xFE)|(TEIU.tbuf1[(i+4)*2+1]&0x01);
		}
		else
		{
			SendB[i*3+1]=0xFC|((TEIU.tbuf1[(i+4)*2+1]>>1)&0x01);
			SendB[i*3+2]=0xFC|(TEIU.tbuf1[(i+4)*2+1]&0x01);
		}
	}
	Set485_Dir(3,RS485_TX);
	UARTSend(3,SendB,12);
	memcpy(UART3Buffer,SendB,12);
}

//TWI总线轮询
void TWI_poll(void)
{

			
	TWI_buf();																//发送缓冲数组准备
	
	SSU_COMM();
	MAU_COMM();
	Cntd_send();
			
}


void fatalfault_TWI(uint8_t enable_ctdn)
{
	PCU_buf();													//PCU模块缓冲数组赋值
	
	if(enable_ctdn)
		cntdn8();		                                        //两阶段流8块倒计时缓冲数组赋值EIU模块
	else
		cntdn_black();
	
	MCM_buf();													//MCM模块缓冲数组赋值
	
	SSU_COMM();
	MAU_COMM();
	
//	TWIStartAndWait();
}

/*********************************************************************************************************
** Function name:       I2C1_IRQHandler
** Descriptions:        硬件I2C中断服务程序。TWI通讯
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
void  I2C1_IRQHandler(void)
{   /*
     *读取I2C状态寄存器I2DAT,按照全局变量的设置进行操作及设置软件标志,清除中断逻辑,中断返回
     */
	uchar status;
   	
   	status = LPC_I2C1->STAT & 0xF8; 											//获取TWI总线状态
	

	switch (status)
    {
           /*
             根据状态码进行相应的处理
           */
      	case 0x00:   
         
		LPC_I2C1->CONSET = 0x54;
		LPC_I2C1->CONCLR = 0x08; 
        break; 
		case 0x08:    /* 已发送起始条件,主发送和主接收都有,装入SLA+W或者SLA+R */
        case 0x10:    /*已发送重复起始条件 */                           /* 主发送和主接收都有           */
                                                                        /* 装入SLA+W或者SLA+R           */
		if(ComModule==COMEIU)												//根据当前通讯模块，设置SLA+W
			TWI_SendByte((TWIEIUADDR<<1)&0xfe);//((TWIEIUADDR<<1)& 0xFE); 								// RW 为0: 写操作     	
		else if(ComModule==COMPCU)
			TWI_SendByte((TWIPCUADDR<<1)&0xfe);//((TWIPCUADDR<<1)& 0xFE); 
		else if(ComModule==COMMCM)
			TWI_SendByte((TWIMCMADDR<<1)&0xfe);//((TWIMCMADDR<<1)& 0xFE); 
		
		
		
		TWIIndex=0;															//缓冲区指针复位
		PatternIndex=0;
		
        break;
        case 0x18:
        case 0x28:                                                      /* 已发送I2DAT中的数据,已接收ACK*/
        if(ComModule==COMPCU)
			{
				if(TWIIndex<TPCUSize)
					TWI_SendByte(TPCU[TWIIndex++]);								// 发送数据 
				else
				{
					TWIIndex=0;
					PatternIndex=0;
					TWI_SendStop();												// 发送停止条件，保持TWEA以便从接收
				}
			}
			
			else if(ComModule==COMEIU)
			{
				if(PatternIndex==0)												//通道1数据帧长度
				{
					PatternIndex=1;												//进入通道1数据发送
					
					TWICheckSum=0;
					
					TWICheckSum^=TEIU.tsize1;
                 
					TWI_SendByte(TEIU.tsize1);									//发送通道1数据帧长度
					
					return;
				}
				
				if(PatternIndex==1)												//通道1数据
				{
					if(TWIIndex<TEIU.tsize1)									//发送完缓冲区数据
					{
						TWICheckSum^=TEIU.tbuf1[TWIIndex];
						
                        TWI_SendByte(TEIU.tbuf1[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=2;											//进入通道2
					}
				}
				
				if(PatternIndex==2)												//通道2数据帧长度
				{	
					PatternIndex=3;											//进入通道2数据发送
					
                    TWICheckSum^=TEIU.tsize2;
                 
					TWI_SendByte(TEIU.tsize2);								//发送通道2数据帧长度
					
					return;
				}
				
				if(PatternIndex==3)												//通道2数据
				{
					if(TWIIndex<TEIU.tsize2)									//发送完缓冲区数据
					{
						TWICheckSum^=TEIU.tbuf2[TWIIndex];
						
						TWI_SendByte(TEIU.tbuf2[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=4;											//进入通道3
					}
				}
				
				if(PatternIndex==4)												//通道3数据帧长度
				{
					PatternIndex=5;											//进入通道3数据发送
					
                    TWICheckSum^=TEIU.tsize3;
                    
					TWI_SendByte(TEIU.tsize3);								//发送通道3数据帧长度
					
					return;
				}
				
				if(PatternIndex==5)												//通道3数据
				{
					if(TWIIndex<TEIU.tsize3)									//发送完缓冲区数据
					{
						TWICheckSum^=TEIU.tbuf3[TWIIndex];
                        
                        TWI_SendByte(TEIU.tbuf3[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=6;											//进入通道4
					}
				}
				
				if(PatternIndex==6)												//通道4数据帧长度
				{
					PatternIndex=7;											//进入通道4数据发送
					
                    TWICheckSum^=TEIU.tsize4;
                    
					TWI_SendByte(TEIU.tsize4);								//发送通道4数据帧长度
					
					return;
				}
				
				if(PatternIndex==7)										//通道4数据
				{
					if(TWIIndex<TEIU.tsize4)									//发送完缓冲区数据
					{
						TWICheckSum^=TEIU.tbuf4[TWIIndex];
                        
                        TWI_SendByte(TEIU.tbuf4[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=8;											//进入校验码
					}
				}
				
				if(PatternIndex==8)             //发送校验码
				{
					TWI_SendByte(TWICheckSum);
					PatternIndex=9;											//
					return;
				}
				
				if(PatternIndex==9)             //发送STOP
				{
					
					PatternIndex=0;											//
					TWI_SendStop();	
				}
			}
			
			else if(ComModule==COMMCM)
			{
				if(TWIIndex<TMCMSize)
					TWI_SendByte(TMCM[TWIIndex++]);								// 发送数据 
				else
				{
					TWIIndex=0;
					PatternIndex=0;	
					TWI_SendStop();												// 发送停止条件，保持TWEA以便从接收
				}
			}
			
        break;
        case 0x40:                                                      /* 已发送SLA+R,已接收ACK        */
        LPC_I2C1->CONCLR = 0x28;                                        /* 清零SI,STA                   */
		LPC_I2C1->CONSET = 0x44;
        break;
		case 0x68:											// 0x68: own SLA+W has been received, ACK has been returned(应用故障)
		case 0x70:													// 0x70:     GCA+W has been received, ACK has been returned(应用故障)
		case 0x90:												// 0x90: data byte has been received, ACK has been returned(应用故障)
		case 0x78:											// 0x78:     GCA+W has been received, ACK has been returned(应用故障)
		case 0x88:													// 0x88: data byte has been received, NACK has been returned(应用故障)
		case 0x98:												// 0x98: data byte has been received, NACK has been returned(应用故障)
			
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x44; 	
			TWIfinishflg	=1;
		break;
        case 0x20:                                                      /* 已发送SLA+W,已接收非应答     */
        case 0x30:                                                      /* 已发送I2DAT中的数据，已接收  */
            switch(ComModule)
			{
				case COMPCU:	FailTWI[PCUT]	=1;	
								LPC_I2C1->CONCLR = 0x08; 	
								LPC_I2C1->CONSET = 0x54;			//严重故障，发送STOP
								TWIfinishflg	=1;
								break;
				
				case COMEIU:	if(!FailTWI[EIUT])											//如果第一次故障，则作故障记录
									FailTWI[EIUT]	=1;
								
								select_commodule(COMMCM);									//尝试与MCM通讯
								LPC_I2C1->CONCLR = 0x08;
								LPC_I2C1->CONSET = 0x64;//重发START
								break;
				
				case COMMCM:	if(!FailTWI[MCMT])											//如果第一次故障，则作故障记录
									FailTWI[MCMT]	=1;
								
								LPC_I2C1->CONCLR = 0x08; 	
								LPC_I2C1->CONSET = 0x54;			//结束本次通讯，发送STOP
								TWIfinishflg	=1;
								break;
								
				default:break;
			} 	
            
		break; 
			                                                            /* 非应答                       */
        case 0x38:                                                      /* 在SLA+R/W或数据字节中丢失仲  */
                                                                        /* 裁                           */
       
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x64;
		break;
		case 0x48:                                                      /* 已发送SLA+R,已接收非应答     */
        LPC_I2C1->CONCLR = 0x08;
		LPC_I2C1->CONSET = 0x54;
        break;
		case 0x60:														// 0x60: own SLA+W has been received, ACK has been returned
			
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x44;
			
		break;
		case 0x80:													// 0x80: data byte has been received, ACK has been returned
			
			if(ComModule==COMPCU)
			{
				if(PatternIndex==0)												//回复码
				{
					
					TWICheckSum	=0;												//初始化
					TWIIndex	=0;
					
					if(LPC_I2C1->DAT!=TPCU[0])											//回复码与发送码不一致
					{
						FailTWI[PCUT]	=1;	
					}
					else
					{
						PatternIndex++;											//进入数据帧长度
						TWICheckSum ^=LPC_I2C1->DAT;
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				else if(PatternIndex==1)										//数据帧长度
				{
					RPCUSize =LPC_I2C1->DAT;
					
					PatternIndex++;												//进入数据帧
					TWICheckSum ^=RPCUSize;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
				}
				
				else if(PatternIndex==2)										//数据帧
				{
					if(TWIIndex<RPCUSize)
					{
						RPCU[TWIIndex++] =LPC_I2C1->DAT;
						TWICheckSum ^=LPC_I2C1->DAT;
					}
					
					if(TWIIndex==RPCUSize)
						PatternIndex++;											//进入校验和
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				else if(PatternIndex==3)										//校验和
				{
					if(TWICheckSum!=LPC_I2C1->DAT)
					{
						FailTWI[PCUT]	=1;	
					}
					else
					{
						PatternIndex=0;											//等待接收STOP
					}
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
				}
			}
			
			else if(ComModule==COMEIU)
			{
				if(PatternIndex==0)												//通道1数据帧长度
				{
					PatternIndex=1;												//进入通道1数据接收
					
					TWICheckSum	=0;												//初始化
					TWIIndex	=0;
					
					TEIU.rsize1	=LPC_I2C1->DAT;											//获取数据帧长度
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
					
					return;
				}
				
				if(PatternIndex==1)												//通道1数据
				{
					if(TWIIndex<TEIU.rsize1)									//接收缓冲区数据
					{
						TEIU.rbuf1[TWIIndex++] =LPC_I2C1->DAT;							//
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=2;											//进入通道2
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==2)												//通道2
				{	
					PatternIndex=3;
					
					TEIU.rsize2	=LPC_I2C1->DAT;											//获取接收数据长度
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
					
					return;
				}
				
				if(PatternIndex==3)												//通道2数据
				{
					if(TWIIndex<TEIU.rsize2)									//接收缓冲区数据
					{
						TEIU.rbuf2[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=4;											//进入通道3
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==4)												//通道3
				{
					PatternIndex=5;
					
					TEIU.rsize3	=LPC_I2C1->DAT;											//获取接收数据长度
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
					
					return;
				}
				
				if(PatternIndex==5)												//通道3数据
				{
					if(TWIIndex<TEIU.rsize3)									//接收缓冲区数据
					{
						TEIU.rbuf3[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						LPC_I2C1->CONSET = 0x04; 
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=6;											//进入通道4
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==6)												//通道4
				{
					PatternIndex=7;
					
					TEIU.rsize4	=LPC_I2C1->DAT;											//获取接收数据长度
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
					
					return;
				}
				
				if(PatternIndex==7)												//通道4数据
				{
					if(TWIIndex<TEIU.rsize4)									//接收缓冲区数据
					{
						TEIU.rbuf4[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						LPC_I2C1->CONSET = 0x04; 
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=8;											//进入校验和
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==8)												//校验和
				{
					if(TWICheckSum!=LPC_I2C1->DAT)
					{
						FailTWI[EIUT]	=1;	
					}
					else
					{
						PatternIndex=0;											//等待接收STOP
					}
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
			}
			
			else if(ComModule==COMMCM)
			{
				if(TWIIndex<RMCMSize)
				{
					RMCM[TWIIndex++] =LPC_I2C1->DAT;										//等待接收STOP
				}
				else
				{
					FailTWI[MCMT]	=1;
				}
				
				LPC_I2C1->CONCLR = 0x0C;
				LPC_I2C1->CONSET = 0x44; 
			}
			
		break;
		case 0xA0:														// 0xA0: STOP or REPEATED START has been received while addressed as slave
			
			if(ComModule==COMPCU)
			{
				if(LampTest.enable)												//灯检测过程，只对PCU通讯
				{
					//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT);						//未寻址从机模式，脱离总线，等待成为MT
					//TWIfinishflg	=1;
					
					select_commodule(COMMCM);
					TWI_SendStart();
				}
				else
				{
					select_commodule(COMEIU);
					TWI_SendStart();											//SR收到STOP后即开始发送START成为MT
				}
				
            }
			else if(ComModule==COMEIU)
			{
				select_commodule(COMMCM);
				TWI_SendStart();												//SR收到STOP后即开始发送START成为MT
            }
			else if(ComModule==COMMCM)
			{
				LPC_I2C1->CONCLR = 0x08;
				LPC_I2C1->CONSET = 0x44; 							//未寻址从机模式，脱离总线，等待成为MT
				TWIfinishflg	=1;
            }
			
			break;
		case 0xB0:															// 0xF8: No relevant state information
				LPC_I2C1->CONCLR = 0x08;
				LPC_I2C1->CONSET = 0x64;	
			
		break;	
			
		// Misc
		case 0xF8:															// 0xF8: No relevant state information
			
			break;
        //case 0x50:                                                      /* 已接收数据字节，已返回ACK    */
       // case 0x58:                                                      /* 已接收数据字节，已返回非应答 */ 
        default:
        break;
    }
}



/*********************************************************************************************************
** End Of File
*********************************************************************************************************/

