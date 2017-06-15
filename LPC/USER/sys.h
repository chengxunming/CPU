#ifndef __SYS_H
#define __SYS_H	

#include "LPC177x_8x.h"	 																	   
																			//��ռ���ȼ���Χ		 �����ȼ���Χ
#define NVIC_PriorityGroup_0_0_32	7//0λ��ռ���ȼ�����λ��5λ�����ȼ�����λ		  0                	 0-32  
#define NVIC_PriorityGroup_1_2_16	6//1λ��ռ���ȼ�����λ��4λ�����ȼ�����λ		  0-2                0-16 
#define NVIC_PriorityGroup_2_4_8	5//2λ��ռ���ȼ�����λ��3λ�����ȼ�����λ		  0-4                0-8 
#define NVIC_PriorityGroup_3_8_4	4//3λ��ռ���ȼ�����λ��2λ�����ȼ�����λ		  0-8                0-4 
#define NVIC_PriorityGroup_4_16_2	3//4λ��ռ���ȼ�����λ��1λ�����ȼ�����λ		  0-16               0-2 
#define NVIC_PriorityGroup_5_32_0	2//5λ��ռ���ȼ�����λ��0λ�����ȼ�����λ		  0-32               0-0 

#define	LPC_NVIC_PriorityGroup	NVIC_PriorityGroup_3_8_4	
#define NVIC_PriorityCalculate(PreemptPriority,SubPriority)		NVIC_EncodePriority(LPC_NVIC_PriorityGroup,PreemptPriority,SubPriority)

void NVIC_GroupingConfiguration(void);


#endif

