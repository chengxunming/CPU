#include "bsp_net.h"
#include "socket.h"	// Just include one header for WIZCHIP

#include "bsp_include.h"

/* Private define ------------------------------------------------------------*/
#define DATA_BUF_SIZE    10

uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	

uint8_t gDATABUF[DATA_BUF_SIZE];	
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };
////////////////////////////////////////////////LINK  W5500/////////////////////////////////////////////////////////////////////

//W5500复位引脚和片选引脚初始化 
static void W5500_GPIO_Init(void)
{
	//W5500 RESET
	LPC_GPIO4->DIR |= 1ul<<15;                                            /*设置P4.15为输出    */
    LPC_GPIO4->SET |= 1ul<<15;                                            /*设置P4.15为高电平  */ //默认高电平，低电平复位有效
	
	//W5500 CS
	LPC_GPIO1->DIR |= 1ul<<8;                                            /*设置P1.8为输出    */
    LPC_GPIO1->SET |= 1ul<<8;                                            /*设置P1.8为高电平  */	
	
}
//片选使能
static void SPI2_CS_Select(void)
{
	LPC_GPIO1->CLR |= (1UL<<8);
}
//取消片选使能
static void SPI2_CS_Deselect(void)
{
	LPC_GPIO1->SET |= (1UL<<8);
}
//关中断
static void SPI_CrisEnter(void)
{
	__disable_irq();
}
//开中断
static void SPI_CrisExit(void)
{
	__enable_irq();
}
//写一个字节
static void SPI2_WriteByte(uint8_t TxData)
{
	SSPSend(2,&TxData, 1);
}
//读一个字节
static uint8_t SPI2_ReadByte(void)
{
	uint8_t RxData;
	SSPReceive(2, &RxData, 1);
	return RxData;
}
       
//SPI回调函数注册
static void W5500_RegisterFunction(void)
{
	// 注册W5500的SPI接口函数 
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//注册临界区函数
	
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(SPI2_CS_Select, SPI2_CS_Deselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(SPI2_CS_Select, SPI2_CS_Deselect); 
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif
	
	reg_wizchip_spi_cbfunc(SPI2_ReadByte, SPI2_WriteByte);	//注册读写函数	
}
//初始化芯片参数
static void W5500_ParametersConfiguration(void)
{
	uint8_t link_state;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

	//WIZCHIP SOCKET缓存区初始化
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		while(1);
	}
	//PHY物理层连接状态检查
	do{
		 ctlwizchip(CW_GET_PHYLINK, (void*)&link_state);
	}while(link_state == PHY_LINK_OFF);
}
//初始化网络参数
static void W5500_NetworkConfiguration(void)
{
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
}

//W5500 硬重启
void W5500_Reset(void)
{
	LPC_GPIO4->CLR |= 1ul<<15;                                            /*设置P4.15为低电平  */
	delay_ms(50);	
	LPC_GPIO4->SET |= 1ul<<15;                                            /*设置P4.15为高电平  */
}
	
//W5500配置
void W5500_Config(void)
{
	SSP2Init(1);		//SSP2初始化，W5500使用
	W5500_GPIO_Init();	//初始化复位引脚
	
	W5500_Reset();
	
	W5500_RegisterFunction();
	W5500_ParametersConfiguration();
	W5500_NetworkConfiguration();
}
//创建UDP连接
void W5500_UDP_SocketCreat(uint8_t sn, uint16_t port)
{
	while(getSn_SR(sn)==SOCK_CLOSED)
	{
		socket(sn, Sn_MR_UDP, port, 0x00);
	}
}
//UDP连接数据回环测试
int32_t W5500_UDP_LoopBack(uint8_t sn, uint16_t port)
{
	int32_t  ret;
	uint16_t size, sentsize;
	uint8_t  destip[4];
	uint16_t destport;
	uint8_t* buf;

	switch(getSn_SR(sn))//socket状态
	{
		case SOCK_UDP :
			if((size = getSn_RX_RSR(sn)) > 0)
			{
				if(size > DATA_BUF_SIZE)size = DATA_BUF_SIZE;
				buf=(uint8_t*)malloc(size);//分配内存，存放接收数据
				if(buf==NULL) return -1;
				ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
				if(ret <= 0)
				{
					return ret;
				}
				size = (uint16_t) ret;
				sentsize = 0;
				while(sentsize != size)
				{
					ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
					if(ret < 0)
					{
						return ret;
					}
					sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				}
				free(buf);//处理完成，释放内存
			}
			break;
		case SOCK_CLOSED:
			if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)//创建UDP连接
			{
				return ret;
			}
			break;
		default :
			break;
	}
	return 1;	
}

