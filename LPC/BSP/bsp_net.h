#ifndef __BSP_NET_H
#define __BSP_NET_H 

#include "LPC177x_8x.h"
#include "ssp.h"

#define W5500_RESET_PIN		GPIO_Pin_8
#define	W5500_RESET_GPIO	GPIOA
#define W5500_RESET_GPIO_CLK_ENABLE()      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE )//PORTAʱ��ʹ�� 

void W5500_Reset(void);
void W5500_Config(void);
void W5500_UDP_SocketCreat(uint8_t sn, uint16_t port);
int32_t W5500_UDP_LoopBack(uint8_t sn, uint16_t port);

#endif

