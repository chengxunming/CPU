#include "bsp_can.h"

//����CAN����֡--SSU����--��֡
uint8_t Send_CAN_DataFrame(uint8_t ch,uint8_t addr,uint8_t *ptr,uint8_t len) //ptr: ����ָ��. len: ���ݳ��� 
{
	uint16_t i=0;
	uint8_t mailbox_num, can_tx_fail=0;
	uint32_t SENDID;
	CanTxMsg TxMessage;

	
	SENDID = (0x01<<7)|addr;
	
	TxMessage.StdId = SENDID;
	TxMessage.IDE = CAN_ID_STD;//CAN_ID_EXT;
	TxMessage.RTR = CAN_RTR_DATA;
    
	if((len<=8)&&(len>0))
	{
		TxMessage.DLC = len;
		
		for(i=0; i<len; i++)
		{
			TxMessage.Data[i] = *ptr++;
		}	
	}
	else
	{
		TxMessage.DLC = 0;
	}
	

	return can_tx_fail;
}

