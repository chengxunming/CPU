#include "bsp.h"

#define SOCK_UDP_FD        	 0
#define SOCK_UDP_PORT        49489



//�������socket����������������
void NET_UDP_SocketConfig(void)
{
	W5500_Config();
	W5500_UDP_SocketCreat(SOCK_UDP_FD,SOCK_UDP_PORT);
}
//����ػ����ԣ�����������
void NET_UDP_LoopBack(void)
{
	W5500_UDP_LoopBack(SOCK_UDP_FD,SOCK_UDP_PORT);
}

