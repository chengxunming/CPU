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
//#include "LPC1700CAN.h" 
//#include <string.h> 
#include "../User_code/global.h"

MessageDetail MessageDetailT;
MessageDetail MessageCAN0;                                              /* ����CAN0ͨ��֡����           */
MessageDetail MessageCAN1;                                              /* ����CAN1ͨ��֡����           */
CAN_MSG_Type	sCANFRAME;

ScanReceive		sStatus,sStatus_history;
ScanSend		sCommand,s_command_history;							//��ʷֵ
extern Board_Online_Status Boardstatus;


t_DTnS dtns[32];
t_IOnS ions[32];
t_dev_s all_dev[DEV_NUM];
t_dataFrame send_dataFrame,rec_dataFrame;

extern uint8_t PDUDRV[4];
uint8_t AirswitchCnt;
uint8_t GetSSUinitstatus=0;
uint8_t LAMPOFF_OP_Flag,PDU_DATA_READY,GetMaustatus=0,RMCMTEMP[1]={0};

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
            myDelay(1);                                     /* ???????1ms           */
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



int frame_organise(int funId, CAN_MSG_Type *Candata, uint8* pSendData, uint8 iDataLen)
{
	t_IDFrame *p_IDFrame;
	uint8_t ch;
	
	if(funId<=38)
	{
		ch = 1;
	}
	else if((70<funId)&&(funId<301))
	{
		ch = 0;
	}
	else if((300<funId)&&(funId<370))
	{
		ch = 1;
	}
	else 
	{
		ch = 0;
	}

	if (!Candata)
	{
		return -1;
	}
	else
	{
		//memset(Candata, 0, sizeof(CANFRAME));
		memset(Candata, 0, sizeof(CAN_MSG_Type));
	}

	//just for 5 bytes
	if (iDataLen > 5)
	{
		return -1;
	}
	//Candata->can_dlc = iDataLen + 3;
	Candata->len = iDataLen + 3;

	//p_IDFrame = (t_IDFrame *) (&(Candata->can_id));
	p_IDFrame = (t_IDFrame *) (&(Candata->id));

	p_IDFrame->dataType = APP_DATA;
	p_IDFrame->addrType = ID_REV;

	send_dataFrame.ifExt = NO_EXT;
	send_dataFrame.ifFirmware = FIRMWARE_NOT;
	
	//��С��ת��
	uint8_t temp1,temp2;
	temp1 = funId;
	temp2 = (funId&0xFF00)>>8;
	
	send_dataFrame.dataType = (temp1<<8)+temp2;

	switch (funId)
	{
	case LMPC:
	case DIMC:
	case MFUC:
		p_IDFrame->nodeId = SSU_ID;
		break;
	case PD1C:
	case PD2C:
	case PD3C:
	case PD4C:
	case PD5C:
	case PD6C:
	case PD7C:
	case PD8C:
		p_IDFrame->nodeId = (funId - PD1C) + PDU1_ID;
		break;
	case IO1C:
	case IO2C:
	case IO3C:
	case IO4C:
	case IO5C:
		p_IDFrame->nodeId = (funId - IO1C) + DET_IOU01_ID;
		break;
	case DR1C:
	case DR2C:
	case DR3C:
	case DR4C:
		p_IDFrame->nodeId = (funId - DR1C) + DET_IOU01_ID;
		break;
	case PASC:
	case WARC:
	case FATC:
	case LMSC:
	case CSOC:
	case FLSC:
	case REMC:
	case KNMC:
	case KSPC:
	case KARC:
	case KLAC:
	case KLBC:
	case KLCC:
	case KLDC:
	case KLEC:
	case KTSC:
		p_IDFrame->nodeId = MAU_ID;
		break;
	default:
		break;
	}

	memcpy(send_dataFrame.data, pSendData, iDataLen);
	
	Candata->dataA[0] = send_dataFrame.ifExt + (send_dataFrame.ifFirmware<<1) + (send_dataFrame.reserver<<3);
	Candata->dataA[1] = (send_dataFrame.dataType&0xFF00)>>8;
	Candata->dataA[2] = send_dataFrame.dataType&0x00FF;
	Candata->dataA[3] = send_dataFrame.data[0];
	Candata->dataB[0] = send_dataFrame.data[1];
	Candata->dataB[1] = send_dataFrame.data[2];
	Candata->dataB[2] = send_dataFrame.data[3];
	Candata->dataB[3] = send_dataFrame.data[4];

	
	memcpy(&Candata->id, p_IDFrame, 4);
	Candata->format = STD_ID_FORMAT;
	Candata->type = DATA_FRAME;
	
	CAN_SendMsg(ch,Candata);
	
	return 0;
}


void Send_Heartbeat(void)
{
	memset(&sCANFRAME,0x00,sizeof(sCANFRAME));
	sCANFRAME.format = STD_ID_FORMAT;
	sCANFRAME.len = 0;
	sCANFRAME.id = 1ul<<28;
	sCANFRAME.type = DATA_FRAME;
	
	CAN_SendMsg(CAN_1,&sCANFRAME);
	CAN_SendMsg(CAN_2,&sCANFRAME);

}


void SSU_data_convert(void)
{
	if(RPCU[0]<0)
	{
		RPCU[0] -= 0x80;
		RPCU[0] = ~RPCU[0];
		RPCU[0] += 0x01;
	}
	
	if(RPCU[1]<0)
	{
		RPCU[1] -= 0x80;
		RPCU[1] = ~RPCU[1];
		RPCU[1] += 0x01;
	}
	
	if(RPCU[2]<0)
	{
		RPCU[2] -= 0x80;
		RPCU[2] = ~RPCU[2];
		RPCU[2] += 0x01;
		RPCU[2] |= 0x80;
	}
}

/*********************************************************************************************************
** Functoin name:       CAN_Protocol_Process
** Descriptions:        ����CAN���ݽ���״̬������
** input paraments:  
**                      *MessageCAN:  ���CAN֡��Ϣ�Ľṹ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void CAN_Protocol_Process(MessageDetail *MessageCAN)
{	
	uint8_t DATA_Frame[8];
	t_IDFrame *p_IDFrame;
	
	DATA_Frame[0] 	= 	MessageCAN->DATAA&0x000000FF;
	DATA_Frame[1]	= 	(MessageCAN->DATAA&0x0000FF00)>>8;
	DATA_Frame[2]	= 	(MessageCAN->DATAA&0x00FF0000)>>16;
	DATA_Frame[3]	= 	(MessageCAN->DATAA&0xFF000000)>>24;
	DATA_Frame[4]	= 	MessageCAN->DATAB&0x000000FF;
	DATA_Frame[5]	= 	(MessageCAN->DATAB&0x0000FF00)>>8;
	DATA_Frame[6]	= 	(MessageCAN->DATAB&0x00FF0000)>>16;
	DATA_Frame[7]	= 	(MessageCAN->DATAB&0xFF000000)>>24;
	
	p_IDFrame = (t_IDFrame *) (&(MessageCAN->CANID));
	rec_dataFrame.ifExt = DATA_Frame[0] & 0x03;
	rec_dataFrame.ifFirmware = (DATA_Frame[0] & 0x06)>>1;
	rec_dataFrame.reserver = (DATA_Frame[0] & 0xF8)>>3;
	rec_dataFrame.dataType = (DATA_Frame[2]<<8)+DATA_Frame[1];
	for(uint8_t i=0;i<5;i++)
	{
		rec_dataFrame.data[i] = DATA_Frame[i+3];
	}
	
	if (p_IDFrame->dataType == HEART_BEAT)
	{
		all_dev[p_IDFrame->nodeId].dev_active = DEV_STATUS_ACTIVE;
		all_dev[p_IDFrame->nodeId].time_count = 0;
		
		p_IDFrame->addrType = ID_REV;
		
		switch(p_IDFrame->nodeId)
		{
			case SSU_ID:
					Boardstatus.SSU_OFFLINE = 0;
				
			break;
			case MAU_ID:
					Boardstatus.MAU_OFFLINE = 0;
				
			break;
			case PDU1_ID:
			case PDU2_ID:
			case PDU3_ID:
			case PDU4_ID:
			case PDU5_ID:
			case PDU6_ID:
			case PDU7_ID:
			case PDU8_ID:
				if(p_IDFrame->nodeId<PDU_NUM+8)
				{	
					Boardstatus.PDU_OFFLINE |= 0x00<<(p_IDFrame->nodeId-8);	
				}	
			break;
			case DET_IOU01_ID:
			case DET_IOU02_ID:
			case DET_IOU03_ID:
			case DET_IOU04_ID:
			case DET_IOU05_ID:
			break;
			default:
				break;
		}
		
	}
	else if (p_IDFrame->dataType == APP_DATA)
	{
		switch (rec_dataFrame.dataType)
		{
			case MLMS:							//"MLMS" ���״̬
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bToggleOn = CTRL_ACTIVE;		//MAU	��ƴ�		
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bToggleOn = CTRL_INACTIVE;		//MAU	��ƹر�		
			}
			if(sStatus.bToggleOn)   //��ƿ���
			{

					RMCM[0] &= 0xFE;
					RMCM[0] |= !LAMPON;
					RMCMTEMP[0] |= !LAMPON;
				
			}
			else
			{

					RMCM[0] &= 0xFE;
					RMCM[0] |= !LAMPOFF;
					RMCMTEMP[0] |= !LAMPOFF;

			}
			
			break;
			case MMAS:		//�ֿؿ���		"MMAS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bManualOn = CTRL_ACTIVE;		//MAU手控开关为"打开"位置，应用程序中应捕捉变化状�?			
        
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bManualOn = CTRL_INACTIVE;		//MAU手控开关为"关闭"位置，应用程序中应捕捉变化状�?			
			}
			if(sStatus.bManualOn)
			{

				if(MCM.KeypadStatus)
				{
					RMCM[0] &= 0xF1;
					RMCM[0] |= KP_MANUAL;
				}
				else
				{
					RMCMTEMP[0] &= 0xF1;
					RMCMTEMP[0] |= KP_MANUAL;
					GetMaustatus = 1;
				}
				
			}
			else
			{
				if(MCM.KeypadStatus)
				{
					RMCM[0] &= 0xF1;
					RMCM[0] |= KP_NORMAL;
				}
				else
				{
					RMCMTEMP[0] &= 0xF1;
					RMCMTEMP[0] |= KP_NORMAL;
					GetMaustatus = 1;
				}
			}
      
			break;
			case MFSS:		//��˸����		"MFSS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bFlashOn = CTRL_ACTIVE;	//MAU手控闪烁开关为"打开"位置，应用程序中应捕捉变化状�?			
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bFlashOn = CTRL_INACTIVE;	//MAU手控闪烁开关为"关闭"位置，应用程序中应捕捉变化状�?			
			}
			if(sStatus.bFlashOn)
			{
				if(MCM.KeypadStatus)
				{
					RMCM[0] &= 0xF1;
					RMCM[0] |= KP_PT;
				}
				else
				{
					RMCMTEMP[0] &= 0xF1;
					RMCMTEMP[0] |= KP_PT;
					GetMaustatus = 1;
				}

			}
			else
			{
				
				if(MCM.KeypadStatus)
				{
					RMCM[0] &= 0xF1;
					if(sStatus.bManualOn)
					{
						RMCM[0] |= KP_MANUAL;
					}
					else
					{
						RMCM[0] |= KP_NORMAL;
					}
				}
				else
				{
					RMCMTEMP[0] &= 0xF1;
					if(sStatus.bManualOn)
					{
						RMCMTEMP[0] |= KP_MANUAL;
					}
					else
					{
						RMCMTEMP[0] |= KP_NORMAL;
					}
					GetMaustatus = 1;
				}	
			}
			
			break;
			
			
			case MRTS:	//ң�ؿ���		"MRTS"
				if (rec_dataFrame.data[0] == COMMAND_EN)
				{
					sStatus.bRemoteOn = CTRL_ACTIVE;	//MAU手控遥控开关为"打开"位置，应用程序中应捕捉变化状�?			
				}
				else if (rec_dataFrame.data[0] == COMMAND_DIS)
				{
					sStatus.bRemoteOn = CTRL_INACTIVE;	//MAU手控遥控开关为"关闭"位置，应用程序中应捕捉变化状�?			
				}
				break;
			case MKYS:	//���󰴼�
			{
				sStatus.cKey = rec_dataFrame.data[0];	//ʵ�ʰ����?	
				if(MCM.KeypadStatus)
				{
					RMCM[0] &= 0x0F;
					if(sStatus.cKey == 0)
					{
						RMCM[0] |= 9<<4;
					}
					else if(sStatus.cKey == 9)
					{
						RMCM[0] |= 0<<4;
					}
					else
					{
						RMCM[0] |= 0xFE;
					}
				}
				else
				{
					RMCMTEMP[0] &= 0x0F;
					if(sStatus.cKey == 0)
					{
						RMCMTEMP[0] |= 9<<4;
					}
					else if(sStatus.cKey == 9)
					{
						RMCMTEMP[0] |= 0<<4;
					}
					else
					{
						RMCMTEMP[0] |= 0xFE;
					}
					GetMaustatus = 1;
				}
				
				
			}
				break;
			case MAVS:
			
			break;
		case MFUS:	//MAU����״̬	"MAUS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bMfuWork = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bMfuWork = STATUS_INACTIVE;
			}
			break;
		case ACSA:	//SSU检测的交流掉电状�?			if (rec_dataFrame.data[0] == COMMAND_EN)
			if (rec_dataFrame.data[0] == COMMAND_EN)	
			{
				sStatus.bACSA = STATUS_ACTIVE;
				
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bACSA = STATUS_INACTIVE;
				
				
			}
			break;
		case DCFS:	//SSU检测的直流掉电状�?			if (rec_dataFrame.data[0] == COMMAND_EN)
			if (rec_dataFrame.data[0] == COMMAND_EN)	
			{
				sStatus.bDcFail = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bDcFail = STATUS_INACTIVE;
			}
			break;
		case SAMS:	//SSU检测到的四个环境变量状态，包括交流电压、直流电压、交流频率和温度
			sStatus.cAcVolt = rec_dataFrame.data[0];
			sStatus.cDcVolt = rec_dataFrame.data[1];
			sStatus.cAcFreq = rec_dataFrame.data[2];
			sStatus.cTemp = rec_dataFrame.data[3];
			
			RPCU[0] = sStatus.cTemp;
			RPCU[1] = sStatus.cAcVolt-130;
			RPCU[2] = sStatus.cAcFreq-50;
			//SSU_data_convert();
			
			
			
			break;
		case ACSD:
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				if((!sStatus_history.bACSD)&&initfinishflg)
				{
					if(!LampTest.enable)
						RstFlgReboot = 1;	
				}
					
				sStatus.bACSD = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				if((sStatus_history.bACSD)&&initfinishflg)
				{
					LAMPOFF_OP_Flag = 1;
				}
				sStatus.bACSD = STATUS_INACTIVE;
				
			}
			sStatus_history.bACSD = sStatus.bACSD;
			break;
			
		case ACSC:
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bACSC = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bACSC = STATUS_INACTIVE;
			}
			break;
		case ACSB:
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bACSB = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bACSB = STATUS_INACTIVE;
			}
			break;
		case SSVS:
			break;
		case ACSE:
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bACSE = STATUS_ACTIVE;
				ACFAILtimeCnt = 0;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bACSE = STATUS_INACTIVE;
				ACFAILtimeCnt = MAXACFAILTIME;
			}
			break;
		case MAUS:	//MAU����״̬	"MAUS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bMauEncryption = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bMauEncryption = STATUS_INACTIVE;
			}
			break;
		case PD1S:
		case PD2S:
		case PD3S:
		case PD4S:
		case PD5S:
		case PD6S:
		case PD7S:
		case PD8S:	//PDU检测到的四个通道的各信号灯的实时监测状�?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
			{
				switch (rec_dataFrame.ifExt)
				{
					case 0:
						break;
					case 1:
						memcpy((uint8_t*) &sStatus.sLoadCheck[rec_dataFrame.dataType - PD1S], rec_dataFrame.data, 5);
						break;
					case 2:
						memcpy((uint8_t*) &sStatus.sLoadCheck[rec_dataFrame.dataType - PD1S] + 5, rec_dataFrame.data,
								5);
						break;
					case 3:
						if (rec_dataFrame.data[0] == COMMAND_EN)
						{
							sStatus.sLoadCheck[rec_dataFrame.dataType - PD1S].power = STATUS_ACTIVE;
						}
						else if (rec_dataFrame.data[0] == COMMAND_DIS)
						{
							sStatus.sLoadCheck[rec_dataFrame.dataType - PD1S].power = STATUS_INACTIVE;
						}
						break;
					default:
						break;
				}
				PDU_DATA_READY = 1;
				
				for(uint8_t i=0;i<8;i++)
				{
					if(PDUR[i].amp1<sStatus.sLoadCheck[i].current[0]) PDUR[i].amp1 = sStatus.sLoadCheck[i].current[0];
					if(PDUR[i].amp2<sStatus.sLoadCheck[i].current[1]) PDUR[i].amp2 = sStatus.sLoadCheck[i].current[1];
					if(PDUR[i].amp3<sStatus.sLoadCheck[i].current[2]) PDUR[i].amp3 = sStatus.sLoadCheck[i].current[2];
					if(PDUR[i].amp4<sStatus.sLoadCheck[i].current[3]) PDUR[i].amp4 = sStatus.sLoadCheck[i].current[3];
					
				}
			}
			break;
		case PDVS:
			break;
		case DT1S:
		case DT2S:
		case DT3S:
		case DT4S:	//DET�?个检测通道100毫秒内采集到的检测状态及通道故障状�?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
			{
				switch (rec_dataFrame.ifExt)
				{
					case NO_EXT:
						break;
					case DATA_BEGIN:
						memcpy((uint8*) (sStatus.sDetCheck[(rec_dataFrame.dataType - DT1S)]), rec_dataFrame.data, 5);
						break;
					case DATA_MIDD:
						break;
					case DATA_END:
						memcpy((uint8*) (sStatus.sDetCheck[(rec_dataFrame.dataType - DT1S)]) + 5, rec_dataFrame.data,
								3);
						break;
					default:
						break;
				}
			}
			Det_IOU_Dataconvert();
			break;
		case DTVS:
			
			break;
		case IO1S:
		case IO2S:
		case IO3S:
		case IO4S:	//IOU�?个检测通道100毫秒内采集到的检测状�?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
			{
				switch (rec_dataFrame.ifExt)
				{
					case NO_EXT:
						break;
					case DATA_BEGIN:
						memcpy((uint8*) (sStatus.sDetCheck[(rec_dataFrame.dataType - IO1S)]), rec_dataFrame.data, 5);	//ions replaced by dtns
						break;
					case DATA_MIDD:
						break;
					case DATA_END:
						memcpy((uint8*) (sStatus.sDetCheck[(rec_dataFrame.dataType - IO1S)]) + 5, rec_dataFrame.data,
								3);	//ions replaced by dtns
						break;
					default:
						break;
				}
			}
			Det_IOU_Dataconvert();
			break;
		case IOVS:
			break;
		default:
			break;
       
		}
	}
	
}	


//CPU����CAN0/1�����ϵĸ�������������ģ��
void SendCanCommand(uint16_t nCmdNo)
{
	uint8 cSendData[4];
	uint8 cSendLen = 1;
	//CANFRAME data;
	CAN_MSG_Type data;
	bool bSendOn=0;


	memset(cSendData, 0, 4);
	switch (nCmdNo)
	{
		case LMPC:								//LMPC�����ԭֵ�ı仯״̬���仯ʱ����
			if (s_command_history.bLoadRelayOn != sCommand.bLoadRelayOn)
			{
				cSendData[0] = (sCommand.bLoadRelayOn == CTRL_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bLoadRelayOn = sCommand.bLoadRelayOn;

				bSendOn = 1;
			}
			break;
		case DIMC:								//DIMC�����ԭֵ�ı仯״̬���仯ʱ����
			if (s_command_history.bDimRelayOn != sCommand.bDimRelayOn)
			{
				cSendData[0] = (sCommand.bDimRelayOn == CTRL_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bDimRelayOn = sCommand.bDimRelayOn;

				bSendOn = 1;
			}
			break;
		case MFUC:								//MFUC�����ԭֵ�ı仯״̬���仯ʱ����
			if (s_command_history.bMfuRelayOn != sCommand.bMfuRelayOn)
			{
				cSendData[0] = (sCommand.bMfuRelayOn == CTRL_INACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bMfuRelayOn = sCommand.bMfuRelayOn;

				bSendOn = 1;
			}
			break;
		case PASC:								//PASC�����ԭֵ�ı仯״̬���仯ʱ����
			if (s_command_history.cMauPass != sCommand.cMauPass)
			{
				cSendData[0] = sCommand.cMauPass;
				s_command_history.cMauPass = sCommand.cMauPass;

				bSendOn = 1;
			 }
			break;
		case PD1C:
		case PD2C:
		case PD3C:
		case PD4C:
		case PD5C:
		case PD6C:
		case PD7C:
		case PD8C:								

				memcpy(cSendData, PDUDRV, 4);
				cSendLen = 4;		//��д���ͳ���

				bSendOn = 1;
			break;
		case IO1C:
		case IO2C:
		case IO3C:
		case IO4C:
		case IO5C:								//IONC�����ԭֵ�ı仯״̬���仯ʱ����
			if (*(uint8*) (&s_command_history.sLogicDrv[nCmdNo - IO1C])
					!= *(uint8*) (&sCommand.sLogicDrv[nCmdNo - IO1C]))
			{
				memcpy(cSendData, &sCommand.sLogicDrv[nCmdNo - IO1C], sizeof(SbitDrv));

				memcpy(&s_command_history.sLogicDrv[nCmdNo - IO1C],
						&sCommand.sLogicDrv[nCmdNo - IO1C], sizeof(SbitDrv));

				bSendOn = 1;
			}
			break;
		case DR1C:
		case DR2C:
		case DR3C:
		case DR4C:							//DRNC�������ֵ�Ƿ���Ч�������ͣ������Իָ�
			if (sCommand.cIouDetReset[nCmdNo - DR1C] == CTRL_ACTIVE)
			{
				cSendData[0] = COMMAND_ENABLE;
				sCommand.cIouDetReset[nCmdNo - DR1C] = CTRL_INACTIVE;

				bSendOn = 1;
			}
			break;
		case WARC:								//�źŻ��������״̬
			if (s_command_history.bWarn != sCommand.bWarn)
			{
				cSendData[0] = (sCommand.bWarn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bWarn = sCommand.bWarn;

				bSendOn = 1;
			}
			break;
		case FATC:								//�źŻ����ع���״̬
			if (s_command_history.bFatal != sCommand.bFatal)
			{
				cSendData[0] = (sCommand.bFatal == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bFatal = sCommand.bFatal;

				bSendOn = 1;
			}
			break;
		case LMSC:								//�źŻ����ͨ���������״̬
			 if (s_command_history.bLoadStatus != sCommand.bLoadStatus)
			 {
				cSendData[0] = (sCommand.bLoadStatus == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bLoadStatus = sCommand.bLoadStatus;

				bSendOn = 1;
			 }
			break;
		case CSOC:								//�źŻ�����Դ״̬
			if (s_command_history.cCtrlSource != sCommand.cCtrlSource)
			{
				cSendData[0] = sCommand.cCtrlSource;
				s_command_history.cCtrlSource = sCommand.cCtrlSource;

				bSendOn = 1;
			}
			break;
		case FLSC:								//�źŻ�����״̬
			if (s_command_history.cFlashSource != sCommand.cFlashSource)
			{
				cSendData[0] = sCommand.cFlashSource;
				s_command_history.cFlashSource = sCommand.cFlashSource;

				bSendOn = 1;
			}
			break;
		case REMC:								//�źŻ�ң�ع���״̬
			if (s_command_history.bRemoteOpen != sCommand.bRemoteOpen)
			{
				cSendData[0] = (sCommand.bRemoteOpen == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bRemoteOpen = sCommand.bRemoteOpen;

				bSendOn = 1;
			}
			break;
		case KNMC:								//���ּ�״̬	"KNMC"
			if (*(uint8*) (&s_command_history.sKeyNum) != *(uint8*) (&sCommand.sKeyNum))
			{
				memcpy(cSendData, &sCommand.sKeyNum, sizeof(SbitDrv));
				memcpy(&s_command_history.sKeyNum, &sCommand.sKeyNum, sizeof(SbitDrv));

				bSendOn = 1;
			}
			break;
		case KSPC:								//����״̬
			if (s_command_history.bKeyStepOn != sCommand.bKeyStepOn)
			{
				cSendData[0] = (sCommand.bKeyStepOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyStepOn = sCommand.bKeyStepOn;

				bSendOn = 1;
			}
			break;
		case KARC:								//ȫ��״̬
			if (s_command_history.bKeyAllRedOn != sCommand.bKeyAllRedOn)
			{
				cSendData[0] = (sCommand.bKeyAllRedOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyAllRedOn = sCommand.bKeyAllRedOn;

				bSendOn = 1;
			}
			break;
		case KLAC:								//��A 			"KLAC"
			if (s_command_history.bKeyAOn != sCommand.bKeyAOn)
			{
				cSendData[0] = (sCommand.bKeyAOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyAOn = sCommand.bKeyAOn;

				bSendOn = 1;
			}
			break;
		case KLBC:								//��B 			"KLBC"
			if (s_command_history.bKeyBOn != sCommand.bKeyBOn)
			{
				cSendData[0] = (sCommand.bKeyBOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyBOn = sCommand.bKeyBOn;

				bSendOn = 1;
			}
			break;
		case KLCC:								//��C 			"KLCC"
			if (s_command_history.bKeyCOn != sCommand.bKeyCOn)
			{
				cSendData[0] = (sCommand.bKeyCOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyCOn = sCommand.bKeyCOn;

				bSendOn = 1;
			}
			break;
		case KLDC:								//��D			"KLDC"
			if (s_command_history.bKeyDOn != sCommand.bKeyDOn)
			{
				cSendData[0] = (sCommand.bKeyDOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyDOn = sCommand.bKeyDOn;

				bSendOn = 1;
			}
			break;
		case KLEC:                                //��E
			if (s_command_history.bKeyEOn != sCommand.bKeyEOn)
			{
				cSendData[0] = (sCommand.bKeyEOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyEOn = sCommand.bKeyEOn;

				bSendOn = 1;
			}
			break;
		case KTSC:								//���Ʋ���
			if (s_command_history.bKeyTestOn != sCommand.bKeyTestOn)
			{
				cSendData[0] = (sCommand.bKeyTestOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyTestOn = sCommand.bKeyTestOn;

				bSendOn = 1;
			}
			break;

		default:
			break;
	}

	if (bSendOn)
	{
		frame_organise(nCmdNo, &data, cSendData, cSendLen);

		bSendOn = 0;
	}
	
}


void SSU_COMM(void)
{
	if(PCU.LampSwitch)
	{
		sCommand.bLoadRelayOn = CTRL_ACTIVE;
		SendCanCommand(LMPC);
	}
	else
	{
		sCommand.bLoadRelayOn = CTRL_INACTIVE;
		SendCanCommand(LMPC);
	}
	if(PCU.LampLevel)
	{
		sCommand.bDimRelayOn = CTRL_ACTIVE;
		SendCanCommand(DIMC);
	}
	else
	{
		sCommand.bDimRelayOn = CTRL_INACTIVE;
		SendCanCommand(DIMC);
	}
}

void MAU_COMM(void)
{
	if(LampSupply)
	{
		sCommand.bLoadStatus = CTRL_ACTIVE;
		SendCanCommand(LMSC);
	}
	else
	{
		sCommand.bLoadStatus = CTRL_INACTIVE;
		SendCanCommand(LMSC);
	}
	
	
	if(MCM.AllRed)//ȫ��
	{
		sCommand.bKeyAllRedOn = CTRL_ACTIVE;
		SendCanCommand(KARC);
	}
	else
	{
		sCommand.bKeyAllRedOn = CTRL_INACTIVE;
		SendCanCommand(KARC);
	}
	
	if(MCM.KeypadStatus)//����״̬
	{
		sCommand.cMauPass = STATUS_INACTIVE;
		SendCanCommand(PASC);
	}
	else
	{
		sCommand.cMauPass = 0x10;
		SendCanCommand(PASC);
	}
	
	if(MCM.TSCFault)//����
	{
		sCommand.bWarn = STATUS_ACTIVE;
		SendCanCommand(WARC);
	}
	else
	{
		sCommand.bWarn = STATUS_INACTIVE;
		SendCanCommand(WARC);
	}
	
	if(MCM.LocalRemote == 0x01)
	{
		sCommand.cCtrlSource = 0x02;
		SendCanCommand(CSOC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	else if(MCM.LocalRemote == 0x03)
	{
		sCommand.bRemoteOpen = STATUS_ACTIVE;
		SendCanCommand(REMC);
		
		sCommand.cCtrlSource = 0x00;
		SendCanCommand(CSOC);
	}
	else if(MCM.LocalRemote == 0x00)
	{
		sCommand.cCtrlSource = 0x00;
		SendCanCommand(CSOC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	
	if(sStatus.bManualOn)
	{
		sCommand.bKeyStepOn = MCM.KeypadStatus;
		SendCanCommand(KSPC);
	}
	else
	{
		sCommand.bKeyStepOn = STATUS_INACTIVE;
		SendCanCommand(KSPC);
	}
	
	if((Strm[0].cmode == FTMODE) ||(Strm[0].cmode == CLFMODE))  //�Զ�
	{
		sCommand.cCtrlSource = 0x06;
		SendCanCommand(CSOC);
		
		sCommand.cFlashSource = 0x01;
		SendCanCommand(FLSC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	else if (Strm[0].cmode == MCMODE)   //�ֶ�
	{
		sCommand.cCtrlSource = 0x05;
		SendCanCommand(CSOC);
		
		sCommand.cFlashSource = 0x01;
		SendCanCommand(FLSC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	if (Strm[0].cmode == PTMODE)   //����
	{
		if(sStatus.bFlashOn)
			sCommand.cFlashSource = 0x04;
		else
			sCommand.cFlashSource = 0x03;
		
		
		SendCanCommand(FLSC);
		
		sCommand.cCtrlSource = 0x08;
		SendCanCommand(CSOC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
  
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
			   
			   CAN_Protocol_Process(&MessageCAN0);
               
               regaddr = (unsigned long)(&LPC_CAN1->CMR)+j*CANOFFSET;                
               mes=RGE(regaddr);
               mes |= (1<<2);                                           /* �ͷŽ��ջ�����               */
               RGE(regaddr)=mes;
            break;
            }
            
            if(j==1)
            {
               CANRCV(j, &MessageCAN1);                                 /* �յ�CAN1�ж�,����֡          */
			   
			   CAN_Protocol_Process(&MessageCAN1);

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
