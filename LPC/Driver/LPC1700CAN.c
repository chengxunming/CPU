/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           LPC1700CAN.c
** Last modified Date:  2009-07-8
** Last Version:        V1.0
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created By:          chenyicheng
** Created date:        2011-07-14
** Version:             V1.0
** Descriptions:        First version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         chenyicheng   
** Modified date:       2011-07-14       
** Version:             V1.0           
** Descriptions:                    
**
*********************************************************************************************************/
#include "LPC1700CAN.h" 
#include "bsp.h" 
//#include <string.h> 
#include "bsp_include.h"

MessageDetail MessageDetailT;
MessageDetail MessageCAN0;                                              /* ����CAN0ͨ��֡����           */
MessageDetail MessageCAN1;                                              /* ����CAN1ͨ��֡����           */

CAN_MSG_Type	sCANFRAME;
static Frame_DefType receive_Frame,receive_Frame2;

static Status CAN_SendMsg (uint8_t canId, CAN_MSG_Type *CAN_Msg);



/*********************************************************************************************************
** Function name:       Enter_SWRst
** Descriptions:        ��PC��������ʾ�ַ���
** input parameters:    Ch:CAN�������ţ�0��ʾ��һ·CAN
** Output parameters:   ��
** Returned value:      0��          ʧ��
**                      1:           �ɹ�
*********************************************************************************************************/
unsigned char Enter_SWRst(unsigned char Ch)
{
    unsigned long regaddr;
    REG_CANMOD regmod;
    regaddr = (unsigned long)(&LPC_CAN1->MOD)+Ch*CANOFFSET;            
    regmod.DWord = RGE(regaddr);                                        /* ��ȡLPC_CAN1->MOD�Ĵ���      */
    regmod.Bits.RM = 1;                                                 /* RMλ��"1"                    */
    RGE(regaddr) = regmod.DWord;                                        /* ��дLPC_CAN1->MOD�Ĵ���      */
    regmod.DWord = RGE(regaddr);                                        /* ��֤д��ֵ                   */
    return (0 != regmod.Bits.RM)? 1:0; 
}  

/*********************************************************************************************************
** Functoin name:       Quit_SWRst
** Descriptions:        CAN�������˳������λģʽ
** input paraments:     Ch:     CAN�������ţ�0��ʾ��һ·CAN
** output paraments:    ��    
** Returned values:     0��     ʧ��
**                      1:      �ɹ�
*********************************************************************************************************/
unsigned char Quit_SWRst(unsigned char Ch)
{
    unsigned long regaddr;
    REG_CANMOD regmod;
    regaddr = (unsigned long)(&LPC_CAN1->MOD)+Ch*CANOFFSET;            
    regmod.DWord = RGE(regaddr);                                        /* ��ȡLPC_CAN1->MOD�Ĵ���      */
    regmod.Bits.RM = 0;                                                 /* RMλ��"0"                    */
    RGE(regaddr) = regmod.DWord;                                        /* ��дLPC_CAN1->MOD�Ĵ���      */
    regmod.DWord = RGE(regaddr);                                        /* ��֤д��ֵ                   */
    return (0 != regmod.Bits.RM)? 0:1; 
}

/*********************************************************************************************************
** Functoin name:       CAN_Init
** Descriptions:        CAN�������˳������λģʽ
** input paraments:     Ch:     CAN�������ţ�0��ʾ��һ·CAN
**                      Baud:   CAN������ֵ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_Init(unsigned char Ch, unsigned long Baud)
{
    unsigned long regaddr;
    uint16_t i;
    i = i;

    switch(Ch){                                                         /* ����CAN����������            */
    case 0:
        LPC_SC->PCONP   |= 0x01L<<13;                                   /* ��CAN��������Դ            */
        LPC_IOCON->P0_0 &= ~0x07;
        LPC_IOCON->P0_0 |= 0x01;                                        /* ѡ��RD1                      */
        LPC_IOCON->P0_1 &= ~0x07;
        LPC_IOCON->P0_1 |= 0x01;                                        /* ѡ��TD1                      */
        break;
    case 1:
        LPC_SC->PCONP   |= 0x01L<<14;                                   /* ��CAN��������Դ            */
        LPC_IOCON->P0_4 &= ~0x07;
        LPC_IOCON->P0_4 |= 0x02;                                        /* ѡ��RD2                      */
                                                                       
        LPC_IOCON->P0_5 &= ~0x07;
        LPC_IOCON->P0_5 |= 0x02;                                        /* ѡ��TD2                      */
                                                                       
        break;
    default:
        break;
    }
    
    Enter_SWRst(Ch);

    regaddr = (unsigned long)(&LPC_CAN1->BTR)+Ch*CANOFFSET;
    RGE(regaddr) = Baud;
    
    LPC_CANAF->AFMR = 0x01;                                             /* ����Ϊ�ر�ģʽ               */
    if(Ch == 0){                                                                        
        LPC_CAN1->CMR  |= (1 << 1) | (1 << 2) | (1 << 3);               /* �ͷŽ��ջ�����               */                              
         LPC_CAN1->IER |= (1<<0);                                       /* �����ж�ʹ��                 */
         LPC_CAN1->GSR |= (0<<0);
         i = LPC_CAN1->ICR;                                             /* ���жϱ�־                   */
    } else{
         LPC_CAN2->CMR |= (1 << 1) | (1 << 2) | (1 << 3);
         LPC_CAN2->IER |= (1<<0);
         LPC_CAN2->GSR |= (0<<0);
         i = LPC_CAN2->ICR;
    }

    LPC_CANAF->AFMR = 0x02;                                             /* ����Ϊ��·ģʽ               */
    Quit_SWRst(Ch);
    
}

/*********************************************************************************************************
** Functoin name:       CANRCV
** Descriptions:        ����CAN֡����ָ���Ľṹ����
** input paraments:     Ch:           CAN�������ţ�0��ʾ��һ·CAN
**                      *MessageCAN:  ���CAN֡��Ϣ�Ľṹ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
unsigned char CANRCV(unsigned char Ch, MessageDetail *MessageCAN)
{
    unsigned long mes;
    unsigned long regaddr;
   
    regaddr = (unsigned long)(&LPC_CAN1->RFS)+Ch*CANOFFSET;             /* ����DLC��RTR��FF             */
    mes = RGE(regaddr);
    MessageCAN->LEN =  (mes >>16) & 0x0F;                               /* ��ȡ֡����                   */
    MessageCAN->FF  =  (mes >>31);                                      /* ��ȡFF                       */
   
    regaddr = (unsigned long)(&LPC_CAN1->RID)+Ch*CANOFFSET;             /* ����ID                       */
    mes = RGE(regaddr);
    if(MessageCAN->FF) {                                                /* FFΪ1��IDΪ29λ              */
   
        MessageCAN->CANID = mes & 0x1fffffff;
    } else {                                                            /* FFΪ0 ��IDΪ11λ             */
        MessageCAN->CANID = mes & 0x000007ff;
    }
   
    regaddr = (unsigned long)(&LPC_CAN1->RDA)+Ch*CANOFFSET;             /* ���� ����A                   */
    mes = RGE(regaddr);
    MessageCAN->DATAA = mes; 
   
    regaddr = (unsigned long)(&LPC_CAN1->RDB)+Ch*CANOFFSET;             /* ���� ����B                   */
    mes = RGE(regaddr);
    MessageCAN->DATAB = mes;
   
    return(1);                                        
}

/*********************************************************************************************************
** Functoin name:       Writedetail
** Descriptions:        �û���д���� ֡��Ϣ
** input paraments:     LEN:    ���ݳ���
**                      *data:  �����������ڵ����� 
**                      FF:
**                      ID:
** output paraments:    ��    
** Returned values:     0��     ʧ��
**                      1:      �ɹ�
*********************************************************************************************************/
unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data)
{
    unsigned char i;
    if(LEN>8){
        return(0);
    } 
	else 
	{
        MessageDetailT.LEN=LEN;
        MessageDetailT.FF=FF;
        MessageDetailT.CANID=ID;
        MessageDetailT.DATAA=0;                                            /* ������                    */
        MessageDetailT.DATAB=0;
    
        for(i=0;i<LEN;i++) {
            if(i<4){
                MessageDetailT.DATAA |= (*(data+i)<<(i*8)); 
            }  
			else 
			{
                MessageDetailT.DATAB |= (*(data+i)<<((i-4)*8));
            }
        }
		return(1);
    }
}

/*********************************************************************************************************
** Functoin name:       CANSend
** Descriptions:        CAN�������˳������λģʽ
** input paraments:     Ch:             CAN�������ţ�0��ʾ��һ·CAN
**                      BUFNum:         ѡ�񻺳���
** output paraments:    MessageDetailT: ���ĵ�������Ϣ������    
** Returned values:     0��             ʧ��
**                      1:              �ɹ�
*********************************************************************************************************/
unsigned char CANSend(unsigned char Ch, unsigned char BUFNum)
{
    unsigned long CAN32reg;
    unsigned long regaddr;
    unsigned char FFflag;

    CAN32reg = 0;
    while (!(CAN32reg & (1 << 3))) {
        regaddr = (unsigned long)(&LPC_CAN1->GSR)+Ch*CANOFFSET;         /* �ȴ���һ�η������           */
        CAN32reg = RGE(regaddr);
				if(!(CAN32reg & (1 << 3))) 
				{                          													 /* ????????????     */
            delay_ms(1);                                     /* ???????1ms           */
            CAN32reg=(1<<3);                               /* ????? ????          */			
				}
    }
    
    BUFNum-=1;                                                          /* �����ַ����                 */
    
    regaddr = (unsigned long)(&LPC_CAN1->TFI1)+Ch*CANOFFSET+BUFNum*0X10;/* 3���������ַ�� 0x10         */
    CAN32reg = RGE(regaddr);
    CAN32reg &= ~((0x0fL<<16) | (0x01L<<30) | (0x80000000));            /* �� DLC,RTR.FFλ              */
    CAN32reg |= ((unsigned long)MessageDetailT.LEN<<16) | ((unsigned long)MessageDetailT.FF<<31);
    RGE(regaddr)=CAN32reg;
    
    FFflag = MessageDetailT.FF;
    
    regaddr = (unsigned long)(&LPC_CAN1->TID1)+Ch*CANOFFSET+BUFNum*0X10;/* д֡ID                       */
    CAN32reg = RGE(regaddr);
    if(FFflag){                                                         /* FFΪ1��IDΪ29λ              */
        CAN32reg &=0x70000000;
        CAN32reg |= (MessageDetailT.CANID & 0x1fffffff);
    }  else{
        CAN32reg &= 0xfffff800;                                         /* FFΪ0 ��IDΪ11λ             */
        CAN32reg |= (MessageDetailT.CANID & 0x000007ff);
    }
    RGE(regaddr)=CAN32reg;
    regaddr = (unsigned long)(&LPC_CAN1->TDA1)+Ch*CANOFFSET+BUFNum*0X10;/* д֡����A                    */
    RGE(regaddr) = MessageDetailT.DATAA;
    
    regaddr = (unsigned long)(&LPC_CAN1->TDB1)+Ch*CANOFFSET+BUFNum*0X10;/* д֡����B                    */
    RGE(regaddr) = MessageDetailT.DATAB;
    
    regaddr = (unsigned long)(&LPC_CAN1->CMR)+Ch*CANOFFSET;             /* д���ƼĴ���������           */
    CAN32reg = RGE(regaddr);
    
    CAN32reg &= ~0x03;
    CAN32reg |= 0x03;
    CAN32reg &= ~((1<<5)|(1<<6)|(1<<7));
    switch(BUFNum){
    case 0: CAN32reg |= 01<<5;break;
    case 1: CAN32reg |= 01<<6;break;
    case 2: CAN32reg |= 01<<7;break;
    default: break;
    }
    RGE(regaddr)=CAN32reg;
    regaddr = (unsigned long)(&LPC_CAN1->GSR)+Ch*CANOFFSET;             /* ��ѯ����״̬                 */
    CAN32reg = RGE(regaddr);

    if(CAN32reg&(1<<3))    {                                            /* ���е������ѳɹ����         */
        return(1);                                                      /* ���ͳɹ����� 1               */
    } else { 
        return (0);                                                     /* ����ʧ�ܷ��� 0               */
    }    
}



//����CAN����֡--SSU����--��֡
Status Send_CAN_DataFrame(uint8_t ch,uint8_t addr,uint8_t *ptr,uint8_t len) //ptr: ����ָ��. len: ���ݳ��� 
{
	uint16_t i=0;
	uint32_t SENDID;
	CAN_MSG_Type TxMessage;

	
	SENDID = (0x01<<7)|addr;
	
	TxMessage.format = STD_ID_FORMAT;
	TxMessage.id = SENDID;
	TxMessage.type = DATA_FRAME;
	
    
	if((len<=8)&&(len>0))
	{
		TxMessage.len = len;
		
		if(len<=4)
		{
			for(i=0; i<len; i++)
			{
				TxMessage.dataA[i] = *ptr++;
			}
		}
		else
		{
			for(i=0; i<4; i++)
			{
				TxMessage.dataA[i] = *ptr++;
			}
			for(i=0; i<len-4; i++)
			{
				TxMessage.dataB[i] = *ptr++;
			}
		}
			
	}
	else
	{
		TxMessage.len = 0;
	}	

	return CAN_SendMsg(ch,&TxMessage);;
}
//����CAN����֡��������ʶ��
void Receive_CAN_DataFrame(MessageDetail* canRx,uint8_t num)
{
	uint8_t *prt_rx;
	if(num!=5)
	{
		ev_CanRespone=CAN_EV_ResFail;
		return;
	}
	prt_rx=(uint8_t *)(&receive_Frame);

	*((uint32_t*)prt_rx)= canRx->DATAA;
	*(prt_rx+4)=canRx->DATAB;
	
	if(DataCheckSum((uint8_t *)(&receive_Frame),4)!=receive_Frame.check_sum)
	{
		ev_CanRespone=CAN_EV_ResFail;
		return;
	}
	
	CAN1_RxMessage_Deal(&receive_Frame);

}
//����CAN����֡��������ʶ��
void Receive_CAN2_DataFrame(MessageDetail* canRx,uint8_t num)
{
	uint8_t *prt_rx;
	if(num!=5)
	{
		ev_CanRespone=CAN_EV_ResFail;
		return;
	}
	prt_rx=(uint8_t *)(&receive_Frame2);

	*((uint32_t*)prt_rx)= canRx->DATAA;
	*(prt_rx+4)=canRx->DATAB;
	
	if(DataCheckSum((uint8_t *)(&receive_Frame2),4)!=receive_Frame2.check_sum)
	{
		ev_CanRespone=CAN_EV_ResFail;
		return;
	}
	
	CAN2_RxMessage_Deal(&receive_Frame2);

}




/*********************************************************************//**
 * @brief 		Setting CAN baud rate (bps)
 * @param[in] 	canId point to LPC_CAN_TypeDef object, should be:
 * 				- LPC_CAN1: CAN1 peripheral
 * 				- LPC_CAN2: CAN2 peripheral
 * @return 		The pointer to CAN peripheral that's expected to use
 ***********************************************************************/
static LPC_CAN_TypeDef* CAN_GetPointer (uint8_t canId)
{
	LPC_CAN_TypeDef* pCan;

	switch (canId)
	{
		case CAN_ID_1:
			pCan = LPC_CAN1;
			break;

		case CAN_ID_2:
			pCan = LPC_CAN2;
			break;

		default:
			pCan = NULL;
			break;
	}

	return pCan;
}

/********************************************************************//**
 * @brief		Send message data
 * @param[in]	 canId			 The Id of the expected CAN component
 *
 * @param[in]	CAN_Msg point to the CAN_MSG_Type Structure, it contains message
 * 				information such as: ID, DLC, RTR, ID Format
 * @return 		Status:
 * 				- SUCCESS: send message successfully
 * 				- ERROR: send message unsuccessfully
 *********************************************************************/
Status CAN_SendMsg (uint8_t canId, CAN_MSG_Type *CAN_Msg)
{
	LPC_CAN_TypeDef* pCan = CAN_GetPointer(canId);

	uint32_t data;

	//Check status of Transmit Buffer 1
	if (pCan->SR & (1 << 2))
	{
		/* Transmit Channel 1 is available */
		/* Write frame informations and frame data into its CANxTFI1,
		 * CANxTID1, CANxTDA1, CANxTDB1 register */
		pCan->TFI1 &= ~ 0x000F0000;

		pCan->TFI1 |= (CAN_Msg->len) << 16;

		if(CAN_Msg->type == REMOTE_FRAME)
		{
			pCan->TFI1 |= (1ul << 30); //set bit RTR
		}
		else
		{
			pCan->TFI1 &= ~(1 << 30);
		}

		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			pCan->TFI1 |= (1ul << 31); //set bit FF
		}
		else
		{
			pCan->TFI1 &= ~(1ul << 31);
		}

		/* Write CAN ID*/
		pCan->TID1 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0]) | (((CAN_Msg->dataA[1]))<< 8) | ((CAN_Msg->dataA[2]) << 16) | ((CAN_Msg->dataA[3]) << 24);

		pCan->TDA1 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0]) | (((CAN_Msg->dataB[1])) << 8)|((CAN_Msg->dataB[2]) << 16)|((CAN_Msg->dataB[3]) << 24);

		pCan->TDB1 = data;

		 /*Write transmission request*/
		 pCan->CMR = 0x21;

		 return SUCCESS;
	}

	//check status of Transmit Buffer 2
	else if((pCan->SR) & (1 << 10))
	{
		/* Transmit Channel 2 is available */
		/* Write frame informations and frame data into its CANxTFI2,
		 * CANxTID2, CANxTDA2, CANxTDB2 register */
		pCan->TFI2 &= ~0x000F0000;

		pCan->TFI2 |= (CAN_Msg->len) << 16;

		if(CAN_Msg->type == REMOTE_FRAME)
		{
			pCan->TFI2 |= (1ul << 30); //set bit RTR
		}
		else
		{
			pCan->TFI2 &= ~(1ul << 30);
		}

		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			pCan->TFI2 |= (1ul << 31); //set bit FF
		}
		else
		{
			pCan->TFI2 &= ~(1ul << 31);
		}

		/* Write CAN ID*/
		pCan->TID2 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0]) | (((CAN_Msg->dataA[1])) << 8) | ((CAN_Msg->dataA[2]) << 16)|((CAN_Msg->dataA[3]) << 24);

		pCan->TDA2 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0]) | (((CAN_Msg->dataB[1])) << 8) | ((CAN_Msg->dataB[2]) << 16) | ((CAN_Msg->dataB[3]) << 24);

		pCan->TDB2 = data;

		/*Write transmission request*/
		pCan->CMR = 0x41;

		return SUCCESS;
	}

	//check status of Transmit Buffer 3
	else if (pCan->SR & (1<<18))
	{
		/* Transmit Channel 3 is available */
		/* Write frame informations and frame data into its CANxTFI3,
		 * CANxTID3, CANxTDA3, CANxTDB3 register */
		pCan->TFI3 &= ~0x000F0000;

		pCan->TFI3 |= (CAN_Msg->len) << 16;

		if(CAN_Msg->type == REMOTE_FRAME)
		{
			pCan->TFI3 |= (1ul << 30); //set bit RTR
		}
		else
		{
			pCan->TFI3 &= ~(1ul << 30);
		}

		if(CAN_Msg->format == EXT_ID_FORMAT)
		{
			pCan->TFI3 |= (1ul << 31); //set bit FF
		}
		else
		{
			pCan->TFI3 &= ~(1ul << 31);
		}

		/* Write CAN ID*/
		pCan->TID3 = CAN_Msg->id;

		/*Write first 4 data bytes*/
		data = (CAN_Msg->dataA[0]) | (((CAN_Msg->dataA[1])) << 8) | ((CAN_Msg->dataA[2]) << 16) | ((CAN_Msg->dataA[3]) << 24);

		pCan->TDA3 = data;

		/*Write second 4 data bytes*/
		data = (CAN_Msg->dataB[0]) | (((CAN_Msg->dataB[1])) << 8) | ((CAN_Msg->dataB[2]) << 16) | ((CAN_Msg->dataB[3]) << 24);

		pCan->TDB3 = data;

		/*Write transmission request*/
		pCan->CMR = 0x81;

		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}




/*********************************************************************************************************
** Functoin name:       CAN_IRQHandler
** Descriptions:        CAN�������жϴ�����
** input paraments:     ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_IRQHandler (void)
{
    unsigned char j;
    unsigned int CAN32reg,mes;
    unsigned int regaddr;
    
    /*
     * ���CANͨ����Ϊ2���ֱ�������ͨ�����ж�   
     */
    for(j=0;j<2;j++){                
    
        regaddr = (unsigned long)(&LPC_CAN1->ICR)+j*CANOFFSET;    
        CAN32reg=RGE(regaddr);    
        
        if((CAN32reg&(1<<0))!= 0)                                       /* RI �����ж�                  */
        {

            if(j==0)
            {
               CANRCV(j, &MessageCAN0);                                 /* �յ�CAN0�ж�,����֡          */
			   
			   //����û�����
//			   CAN_Protocol_Process(&MessageCAN0);
               Receive_CAN_DataFrame(&MessageCAN0,MessageCAN0.LEN);
				
               regaddr = (unsigned long)(&LPC_CAN1->CMR)+j*CANOFFSET;                
               mes=RGE(regaddr);
               mes |= (1<<2);                                           /* �ͷŽ��ջ�����               */
               RGE(regaddr)=mes;
            break;
            }
            
            if(j==1)
            {
               CANRCV(j, &MessageCAN1);                                 /* �յ�CAN1�ж�,����֡          */
			   
				//����û�����
//			   CAN_Protocol_Process(&MessageCAN1);
			   Receive_CAN2_DataFrame(&MessageCAN1,MessageCAN1.LEN);

               regaddr = (unsigned long)(&LPC_CAN1->CMR)+j*CANOFFSET;                
               mes=RGE(regaddr);
               mes |= (1<<2);                                           /* �ͷŽ��ջ�����               */
               RGE(regaddr)=mes;
            break;
            }
    
        }
        
         
        /* ������Ҫ���
        if((CAN32reg&(1<<1))!= 0)                                       // TI1 ��һ���ͻ�������������ж�

        {
            ;
        }
        if((CAN32reg&(1<<9))!= 0)                                       // TI2 �ڶ����ͻ�������������ж�

        {
            ;
        }
        if((CAN32reg&(1<<10))!= 0)                                      // TI3 �������ͻ�������������ж�
        {
            ;
        }
        */
        if((CAN32reg&(1<<7))!= 0)                                       /* BEI ���ߴ����ж�             */
        {
            Enter_SWRst(j);
            regaddr = (unsigned long)(&LPC_CAN1->GSR)+j*CANOFFSET;                
            mes=RGE(regaddr);
            mes &=0x00ff;                                                
            RGE(regaddr)=mes;                                           /*���ߴ����������              */
            Quit_SWRst(j);
        }
        if((CAN32reg&(1<<6))!= 0)                                       /* ALI �ٲö�ʧ�ж�             */
        {
            //����û�����
        }
        if((CAN32reg&(1<<3))!= 0)
        {
            //����û�����
            //ClrCanDataOver(j);                                        /*�ͷŽ��ջ�����                */
        }
        
    }
        
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
