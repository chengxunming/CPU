#include "bsp.h"

#define SOCK_UDP_FD        	 0
#define SOCK_UDP_PORT        49489



//网络测试socket创建，主函数调用
void NET_UDP_SocketConfig(void)
{
	W5500_Config();
	W5500_UDP_SocketCreat(SOCK_UDP_FD,SOCK_UDP_PORT);
}
//网络回环测试，主函数调用
void NET_UDP_LoopBack(void)
{
	W5500_UDP_LoopBack(SOCK_UDP_FD,SOCK_UDP_PORT);
}

