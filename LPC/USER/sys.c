#include "sys.h"

//�ж����ȼ�������
void NVIC_GroupingConfiguration(void)
{

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3_8_4);	//����NVIC�жϷ���3:2  3λ��ռ���ȼ���2λ��Ӧ���ȼ�

}


