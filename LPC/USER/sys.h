#ifndef __SYS_H
#define __SYS_H	

#include "LPC177x_8x.h"	 																	   
																			//抢占优先级范围		 子优先级范围
#define NVIC_PriorityGroup_0_0_32	7//0位抢占优先级控制位，5位子优先级控制位		  0                	 0-32  
#define NVIC_PriorityGroup_1_2_16	6//1位抢占优先级控制位，4位子优先级控制位		  0-2                0-16 
#define NVIC_PriorityGroup_2_4_8	5//2位抢占优先级控制位，3位子优先级控制位		  0-4                0-8 
#define NVIC_PriorityGroup_3_8_4	4//3位抢占优先级控制位，2位子优先级控制位		  0-8                0-4 
#define NVIC_PriorityGroup_4_16_2	3//4位抢占优先级控制位，1位子优先级控制位		  0-16               0-2 
#define NVIC_PriorityGroup_5_32_0	2//5位抢占优先级控制位，0位子优先级控制位		  0-32               0-0 

#define	LPC_NVIC_PriorityGroup	NVIC_PriorityGroup_3_8_4	
#define NVIC_PriorityCalculate(PreemptPriority,SubPriority)		NVIC_EncodePriority(LPC_NVIC_PriorityGroup,PreemptPriority,SubPriority)

void NVIC_GroupingConfiguration(void);


#endif

