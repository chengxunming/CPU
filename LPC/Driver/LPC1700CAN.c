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
MessageDetail MessageCAN0;                                              /* 引用CAN0通道帧变量           */
MessageDetail MessageCAN1;                                              /* 引用CAN1通道帧变量           */
CAN_MSG_Type	sCANFRAME;

ScanReceive		sStatus,sStatus_history;
ScanSend		sCommand,s_command_history;							//历史值
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
** Descriptions:        向PC机发送显示字符。
** input parameters:    Ch:CAN控制器号，0表示第一路CAN
** Output parameters:   无
** Returned value:      0：          失败
**                      1:           成功
*********************************************************************************************************/
unsigned char Enter_SWRst(unsigned char Ch)
{
    unsigned long regaddr;
    REG_CANMOD regmod;
    regaddr = (unsigned long)(&LPC_CAN1->MOD)+Ch*CANOFFSET;            
    regmod.DWord = RGE(regaddr);                                        /* 读取LPC_CAN1->MOD寄存器      */
    regmod.Bits.RM = 1;                                                 /* RM位置"1"                    */
    RGE(regaddr) = regmod.DWord;                                        /* 回写LPC_CAN1->MOD寄存器      */
    regmod.DWord = RGE(regaddr);                                        /* 验证写入值                   */
    return (0 != regmod.Bits.RM)? 1:0; 
}  

/*********************************************************************************************************
** Functoin name:       Quit_SWRst
** Descriptions:        CAN控制器退出软件复位模式
** input paraments:     Ch:     CAN控制器号，0表示第一路CAN
** output paraments:    无    
** Returned values:     0：     失败
**                      1:      成功
*********************************************************************************************************/
unsigned char Quit_SWRst(unsigned char Ch)
{
    unsigned long regaddr;
    REG_CANMOD regmod;
    regaddr = (unsigned long)(&LPC_CAN1->MOD)+Ch*CANOFFSET;            
    regmod.DWord = RGE(regaddr);                                        /* 读取LPC_CAN1->MOD寄存器      */
    regmod.Bits.RM = 0;                                                 /* RM位置"0"                    */
    RGE(regaddr) = regmod.DWord;                                        /* 回写LPC_CAN1->MOD寄存器      */
    regmod.DWord = RGE(regaddr);                                        /* 验证写入值                   */
    return (0 != regmod.Bits.RM)? 0:1; 
}

/*********************************************************************************************************
** Functoin name:       CAN_Init
** Descriptions:        CAN控制器退出软件复位模式
** input paraments:     Ch:     CAN控制器号，0表示第一路CAN
**                      Baud:   CAN波特率值
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/
void CAN_Init(unsigned char Ch, unsigned long Baud)
{
    unsigned long regaddr;
    uint16_t i;
    i = i;

    switch(Ch){                                                         /* 配置CAN控制器引脚            */
    case 0:
        LPC_SC->PCONP   |= 0x01L<<13;                                   /* 打开CAN控制器电源            */
        LPC_IOCON->P0_0 &= ~0x07;
        LPC_IOCON->P0_0 |= 0x01;                                        /* 选择RD1                      */
        LPC_IOCON->P0_1 &= ~0x07;
        LPC_IOCON->P0_1 |= 0x01;                                        /* 选择TD1                      */
        break;
    case 1:
        LPC_SC->PCONP   |= 0x01L<<14;                                   /* 打开CAN控制器电源            */
        LPC_IOCON->P0_4 &= ~0x07;
        LPC_IOCON->P0_4 |= 0x02;                                        /* 选择RD2                      */
                                                                       
        LPC_IOCON->P0_5 &= ~0x07;
        LPC_IOCON->P0_5 |= 0x02;                                        /* 选择TD2                      */
                                                                       
        break;
    default:
        break;
    }
    
    Enter_SWRst(Ch);

    regaddr = (unsigned long)(&LPC_CAN1->BTR)+Ch*CANOFFSET;
    RGE(regaddr) = Baud;
    
    LPC_CANAF->AFMR = 0x01;                                             /* 设置为关闭模式               */
    if(Ch == 0){                                                                        
        LPC_CAN1->CMR  |= (1 << 1) | (1 << 2) | (1 << 3);               /* 释放接收缓冲区               */                              
         LPC_CAN1->IER |= (1<<0);                                       /* 接收中断使能                 */
         LPC_CAN1->GSR |= (0<<0);
         i = LPC_CAN1->ICR;                                             /* 清中断标志                   */
    } else{
         LPC_CAN2->CMR |= (1 << 1) | (1 << 2) | (1 << 3);
         LPC_CAN2->IER |= (1<<0);
         LPC_CAN2->GSR |= (0<<0);
         i = LPC_CAN2->ICR;
    }

    LPC_CANAF->AFMR = 0x02;                                             /* 设置为旁路模式               */
    Quit_SWRst(Ch);
    
}

/*********************************************************************************************************
** Functoin name:       CANRCV
** Descriptions:        接受CAN帧放在指定的结构体中
** input paraments:     Ch:           CAN控制器号，0表示第一路CAN
**                      *MessageCAN:  存放CAN帧信息的结构体
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/
unsigned char CANRCV(unsigned char Ch, MessageDetail *MessageCAN)
{
    unsigned long mes;
    unsigned long regaddr;
   
    regaddr = (unsigned long)(&LPC_CAN1->RFS)+Ch*CANOFFSET;             /* 处理DLC、RTR、FF             */
    mes = RGE(regaddr);
    MessageCAN->LEN =  (mes >>16) & 0x0F;                               /* 获取帧长度                   */
    MessageCAN->FF  =  (mes >>31);                                      /* 获取FF                       */
   
    regaddr = (unsigned long)(&LPC_CAN1->RID)+Ch*CANOFFSET;             /* 处理ID                       */
    mes = RGE(regaddr);
    if(MessageCAN->FF) {                                                /* FF为1，ID为29位              */
   
        MessageCAN->CANID = mes & 0x1fffffff;
    } else {                                                            /* FF为0 ，ID为11位             */
        MessageCAN->CANID = mes & 0x000007ff;
    }
   
    regaddr = (unsigned long)(&LPC_CAN1->RDA)+Ch*CANOFFSET;             /* 处理 数据A                   */
    mes = RGE(regaddr);
    MessageCAN->DATAA = mes; 
   
    regaddr = (unsigned long)(&LPC_CAN1->RDB)+Ch*CANOFFSET;             /* 处理 数据B                   */
    mes = RGE(regaddr);
    MessageCAN->DATAB = mes;
   
    return(1);                                        
}

/*********************************************************************************************************
** Functoin name:       Writedetail
** Descriptions:        用户填写发送 帧信息
** input paraments:     LEN:    数据长度
**                      *data:  发送数据所在的数组 
**                      FF:
**                      ID:
** output paraments:    无    
** Returned values:     0：     失败
**                      1:      成功
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
        MessageDetailT.DATAA=0;                                            /* 先清零                    */
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
** Descriptions:        CAN控制器退出软件复位模式
** input paraments:     Ch:             CAN控制器号，0表示第一路CAN
**                      BUFNum:         选择缓冲区
** output paraments:    MessageDetailT: 报文的特征信息及数据    
** Returned values:     0：             失败
**                      1:              成功
*********************************************************************************************************/
unsigned char CANSend(unsigned char Ch, unsigned char BUFNum)
{
    unsigned long CAN32reg;
    unsigned long regaddr;
    unsigned char FFflag;

    CAN32reg = 0;
    while (!(CAN32reg & (1 << 3))) {
        regaddr = (unsigned long)(&LPC_CAN1->GSR)+Ch*CANOFFSET;         /* 等待上一次发送完成           */
        CAN32reg = RGE(regaddr);
				if(!(CAN32reg & (1 << 3))) 
				{                          													 /* ????????????     */
            myDelay(1);                                     /* ???????1ms           */
            CAN32reg=(1<<3);                               /* ????? ????          */			
				}
    }
    
    BUFNum-=1;                                                          /* 计算地址方便                 */
    
    regaddr = (unsigned long)(&LPC_CAN1->TFI1)+Ch*CANOFFSET+BUFNum*0X10;/* 3缓冲区间地址差 0x10         */
    CAN32reg = RGE(regaddr);
    CAN32reg &= ~((0x0fL<<16) | (0x01L<<30) | (0x80000000));            /* 清 DLC,RTR.FF位              */
    CAN32reg |= ((unsigned long)MessageDetailT.LEN<<16) | ((unsigned long)MessageDetailT.FF<<31);
    RGE(regaddr)=CAN32reg;
    
    FFflag = MessageDetailT.FF;
    
    regaddr = (unsigned long)(&LPC_CAN1->TID1)+Ch*CANOFFSET+BUFNum*0X10;/* 写帧ID                       */
    CAN32reg = RGE(regaddr);
    if(FFflag){                                                         /* FF为1，ID为29位              */
        CAN32reg &=0x70000000;
        CAN32reg |= (MessageDetailT.CANID & 0x1fffffff);
    }  else{
        CAN32reg &= 0xfffff800;                                         /* FF为0 ，ID为11位             */
        CAN32reg |= (MessageDetailT.CANID & 0x000007ff);
    }
    RGE(regaddr)=CAN32reg;
    regaddr = (unsigned long)(&LPC_CAN1->TDA1)+Ch*CANOFFSET+BUFNum*0X10;/* 写帧数据A                    */
    RGE(regaddr) = MessageDetailT.DATAA;
    
    regaddr = (unsigned long)(&LPC_CAN1->TDB1)+Ch*CANOFFSET+BUFNum*0X10;/* 写帧数据B                    */
    RGE(regaddr) = MessageDetailT.DATAB;
    
    regaddr = (unsigned long)(&LPC_CAN1->CMR)+Ch*CANOFFSET;             /* 写控制寄存器，发送           */
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
    regaddr = (unsigned long)(&LPC_CAN1->GSR)+Ch*CANOFFSET;             /* 查询发送状态                 */
    CAN32reg = RGE(regaddr);

    if(CAN32reg&(1<<3))    {                                            /* 所有的请求已成功完成         */
        return(1);                                                      /* 发送成功返回 1               */
    } else { 
        return (0);                                                     /* 发送失败返回 0               */
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
	
	//大小端转换
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
** Descriptions:        接受CAN数据进入状态机处理。
** input paraments:  
**                      *MessageCAN:  存放CAN帧信息的结构体
** output paraments:    无    
** Returned values:     无
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
			case MLMS:							//"MLMS" 外灯状态
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bToggleOn = CTRL_ACTIVE;		//MAU	外灯打开		
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bToggleOn = CTRL_INACTIVE;		//MAU	外灯关闭		
			}
			if(sStatus.bToggleOn)   //外灯开关
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
			case MMAS:		//锟街控匡拷锟斤拷		"MMAS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bManualOn = CTRL_ACTIVE;		//MAU鎵嬫帶寮�鍏充负"鎵撳紑"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
        
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bManualOn = CTRL_INACTIVE;		//MAU鎵嬫帶寮�鍏充负"鍏抽棴"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
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
			case MFSS:		//锟斤拷烁锟斤拷锟斤拷		"MFSS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bFlashOn = CTRL_ACTIVE;	//MAU鎵嬫帶闂儊寮�鍏充负"鎵撳紑"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bFlashOn = CTRL_INACTIVE;	//MAU鎵嬫帶闂儊寮�鍏充负"鍏抽棴"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
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
			
			
			case MRTS:	//遥锟截匡拷锟斤拷		"MRTS"
				if (rec_dataFrame.data[0] == COMMAND_EN)
				{
					sStatus.bRemoteOn = CTRL_ACTIVE;	//MAU鎵嬫帶閬ユ帶寮�鍏充负"鎵撳紑"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
				}
				else if (rec_dataFrame.data[0] == COMMAND_DIS)
				{
					sStatus.bRemoteOn = CTRL_INACTIVE;	//MAU鎵嬫帶閬ユ帶寮�鍏充负"鍏抽棴"浣嶇疆锛屽簲鐢ㄧ▼搴忎腑搴旀崟鎹夊彉鍖栫姸鎬?			
				}
				break;
			case MKYS:	//锟斤拷锟襟按硷拷
			{
				sStatus.cKey = rec_dataFrame.data[0];	//实锟绞帮拷锟斤拷锟?	
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
		case MFUS:	//MAU锟斤拷锟斤拷状态	"MAUS"
			if (rec_dataFrame.data[0] == COMMAND_EN)
			{
				sStatus.bMfuWork = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bMfuWork = STATUS_INACTIVE;
			}
			break;
		case ACSA:	//SSU妫�娴嬬殑浜ゆ祦鎺夌數鐘舵�?			if (rec_dataFrame.data[0] == COMMAND_EN)
			if (rec_dataFrame.data[0] == COMMAND_EN)	
			{
				sStatus.bACSA = STATUS_ACTIVE;
				
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bACSA = STATUS_INACTIVE;
				
				
			}
			break;
		case DCFS:	//SSU妫�娴嬬殑鐩存祦鎺夌數鐘舵�?			if (rec_dataFrame.data[0] == COMMAND_EN)
			if (rec_dataFrame.data[0] == COMMAND_EN)	
			{
				sStatus.bDcFail = STATUS_ACTIVE;
			}
			else if (rec_dataFrame.data[0] == COMMAND_DIS)
			{
				sStatus.bDcFail = STATUS_INACTIVE;
			}
			break;
		case SAMS:	//SSU妫�娴嬪埌鐨勫洓涓幆澧冨彉閲忕姸鎬侊紝鍖呮嫭浜ゆ祦鐢靛帇銆佺洿娴佺數鍘嬨�佷氦娴侀鐜囧拰娓╁害
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
		case MAUS:	//MAU锟斤拷锟斤拷状态	"MAUS"
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
		case PD8S:	//PDU妫�娴嬪埌鐨勫洓涓�氶亾鐨勫悇淇″彿鐏殑瀹炴椂鐩戞祴鐘舵�?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
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
		case DT4S:	//DET鐨?涓娴嬮�氶亾100姣鍐呴噰闆嗗埌鐨勬娴嬬姸鎬佸強閫氶亾鏁呴殰鐘舵�?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
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
		case IO4S:	//IOU鐨?涓娴嬮�氶亾100姣鍐呴噰闆嗗埌鐨勬娴嬬姸鎬?			if (rec_dataFrame.ifFirmware == FIRMWARE_NOT)
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


//CPU发送CAN0/1总线上的各类命令至其他模块
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
		case LMPC:								//LMPC：监控原值的变化状态，变化时发送
			if (s_command_history.bLoadRelayOn != sCommand.bLoadRelayOn)
			{
				cSendData[0] = (sCommand.bLoadRelayOn == CTRL_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bLoadRelayOn = sCommand.bLoadRelayOn;

				bSendOn = 1;
			}
			break;
		case DIMC:								//DIMC：监控原值的变化状态，变化时发送
			if (s_command_history.bDimRelayOn != sCommand.bDimRelayOn)
			{
				cSendData[0] = (sCommand.bDimRelayOn == CTRL_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bDimRelayOn = sCommand.bDimRelayOn;

				bSendOn = 1;
			}
			break;
		case MFUC:								//MFUC：监控原值的变化状态，变化时发送
			if (s_command_history.bMfuRelayOn != sCommand.bMfuRelayOn)
			{
				cSendData[0] = (sCommand.bMfuRelayOn == CTRL_INACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bMfuRelayOn = sCommand.bMfuRelayOn;

				bSendOn = 1;
			}
			break;
		case PASC:								//PASC：监控原值的变化状态，变化时发送
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
				cSendLen = 4;		//改写发送长度

				bSendOn = 1;
			break;
		case IO1C:
		case IO2C:
		case IO3C:
		case IO4C:
		case IO5C:								//IONC：监控原值的变化状态，变化时发送
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
		case DR4C:							//DRNC：监控数值是否有效，是则发送，变量自恢复
			if (sCommand.cIouDetReset[nCmdNo - DR1C] == CTRL_ACTIVE)
			{
				cSendData[0] = COMMAND_ENABLE;
				sCommand.cIouDetReset[nCmdNo - DR1C] = CTRL_INACTIVE;

				bSendOn = 1;
			}
			break;
		case WARC:								//信号机警告故障状态
			if (s_command_history.bWarn != sCommand.bWarn)
			{
				cSendData[0] = (sCommand.bWarn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bWarn = sCommand.bWarn;

				bSendOn = 1;
			}
			break;
		case FATC:								//信号机严重故障状态
			if (s_command_history.bFatal != sCommand.bFatal)
			{
				cSendData[0] = (sCommand.bFatal == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bFatal = sCommand.bFatal;

				bSendOn = 1;
			}
			break;
		case LMSC:								//信号机外灯通道输出开关状态
			 if (s_command_history.bLoadStatus != sCommand.bLoadStatus)
			 {
				cSendData[0] = (sCommand.bLoadStatus == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bLoadStatus = sCommand.bLoadStatus;

				bSendOn = 1;
			 }
			break;
		case CSOC:								//信号机控制源状态
			if (s_command_history.cCtrlSource != sCommand.cCtrlSource)
			{
				cSendData[0] = sCommand.cCtrlSource;
				s_command_history.cCtrlSource = sCommand.cCtrlSource;

				bSendOn = 1;
			}
			break;
		case FLSC:								//信号机闪灯状态
			if (s_command_history.cFlashSource != sCommand.cFlashSource)
			{
				cSendData[0] = sCommand.cFlashSource;
				s_command_history.cFlashSource = sCommand.cFlashSource;

				bSendOn = 1;
			}
			break;
		case REMC:								//信号机遥控工作状态
			if (s_command_history.bRemoteOpen != sCommand.bRemoteOpen)
			{
				cSendData[0] = (sCommand.bRemoteOpen == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bRemoteOpen = sCommand.bRemoteOpen;

				bSendOn = 1;
			}
			break;
		case KNMC:								//数字键状态	"KNMC"
			if (*(uint8*) (&s_command_history.sKeyNum) != *(uint8*) (&sCommand.sKeyNum))
			{
				memcpy(cSendData, &sCommand.sKeyNum, sizeof(SbitDrv));
				memcpy(&s_command_history.sKeyNum, &sCommand.sKeyNum, sizeof(SbitDrv));

				bSendOn = 1;
			}
			break;
		case KSPC:								//步进状态
			if (s_command_history.bKeyStepOn != sCommand.bKeyStepOn)
			{
				cSendData[0] = (sCommand.bKeyStepOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyStepOn = sCommand.bKeyStepOn;

				bSendOn = 1;
			}
			break;
		case KARC:								//全红状态
			if (s_command_history.bKeyAllRedOn != sCommand.bKeyAllRedOn)
			{
				cSendData[0] = (sCommand.bKeyAllRedOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyAllRedOn = sCommand.bKeyAllRedOn;

				bSendOn = 1;
			}
			break;
		case KLAC:								//键A 			"KLAC"
			if (s_command_history.bKeyAOn != sCommand.bKeyAOn)
			{
				cSendData[0] = (sCommand.bKeyAOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyAOn = sCommand.bKeyAOn;

				bSendOn = 1;
			}
			break;
		case KLBC:								//键B 			"KLBC"
			if (s_command_history.bKeyBOn != sCommand.bKeyBOn)
			{
				cSendData[0] = (sCommand.bKeyBOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyBOn = sCommand.bKeyBOn;

				bSendOn = 1;
			}
			break;
		case KLCC:								//键C 			"KLCC"
			if (s_command_history.bKeyCOn != sCommand.bKeyCOn)
			{
				cSendData[0] = (sCommand.bKeyCOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyCOn = sCommand.bKeyCOn;

				bSendOn = 1;
			}
			break;
		case KLDC:								//键D			"KLDC"
			if (s_command_history.bKeyDOn != sCommand.bKeyDOn)
			{
				cSendData[0] = (sCommand.bKeyDOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyDOn = sCommand.bKeyDOn;

				bSendOn = 1;
			}
			break;
		case KLEC:                                //键E
			if (s_command_history.bKeyEOn != sCommand.bKeyEOn)
			{
				cSendData[0] = (sCommand.bKeyEOn == STATUS_ACTIVE) ?
				COMMAND_ENABLE :COMMAND_DISABLE;
				s_command_history.bKeyEOn = sCommand.bKeyEOn;

				bSendOn = 1;
			}
			break;
		case KTSC:								//键灯测试
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
	
	
	if(MCM.AllRed)//全红
	{
		sCommand.bKeyAllRedOn = CTRL_ACTIVE;
		SendCanCommand(KARC);
	}
	else
	{
		sCommand.bKeyAllRedOn = CTRL_INACTIVE;
		SendCanCommand(KARC);
	}
	
	if(MCM.KeypadStatus)//键锁状态
	{
		sCommand.cMauPass = STATUS_INACTIVE;
		SendCanCommand(PASC);
	}
	else
	{
		sCommand.cMauPass = 0x10;
		SendCanCommand(PASC);
	}
	
	if(MCM.TSCFault)//故障
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
	
	if((Strm[0].cmode == FTMODE) ||(Strm[0].cmode == CLFMODE))  //自动
	{
		sCommand.cCtrlSource = 0x06;
		SendCanCommand(CSOC);
		
		sCommand.cFlashSource = 0x01;
		SendCanCommand(FLSC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	else if (Strm[0].cmode == MCMODE)   //手动
	{
		sCommand.cCtrlSource = 0x05;
		SendCanCommand(CSOC);
		
		sCommand.cFlashSource = 0x01;
		SendCanCommand(FLSC);
		
		sCommand.bRemoteOpen = STATUS_INACTIVE;
		SendCanCommand(REMC);
	}
	if (Strm[0].cmode == PTMODE)   //待机
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
** Descriptions:        CAN控制器中断处理函数
** input paraments:     无
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/
void CAN_IRQHandler (void)
{
    unsigned char j;
    unsigned int CAN32reg,mes;
    unsigned int regaddr;
    
    /*
     * 最大CAN通道数为2，分别检测两个通道的中断   
     */
    for(j=0;j<2;j++){                
    
        regaddr = (unsigned long)(&LPC_CAN1->ICR)+j*CANOFFSET;    
        CAN32reg=RGE(regaddr);    
        
        if((CAN32reg&(1<<0))!= 0)                                       /* RI 接收中断                  */
        {

            if(j==0)
            {
               CANRCV(j, &MessageCAN0);                                 /* 收到CAN0中断,接收帧          */
			   
			   CAN_Protocol_Process(&MessageCAN0);
               
               regaddr = (unsigned long)(&LPC_CAN1->CMR)+j*CANOFFSET;                
               mes=RGE(regaddr);
               mes |= (1<<2);                                           /* 释放接收缓冲区               */
               RGE(regaddr)=mes;
            break;
            }
            
            if(j==1)
            {
               CANRCV(j, &MessageCAN1);                                 /* 收到CAN1中断,接收帧          */
			   
			   CAN_Protocol_Process(&MessageCAN1);

               regaddr = (unsigned long)(&LPC_CAN1->CMR)+j*CANOFFSET;                
               mes=RGE(regaddr);
               mes |= (1<<2);                                           /* 释放接收缓冲区               */
               RGE(regaddr)=mes;
            break;
            }
    
        }
        
         
        /* 根据需要添加
        if((CAN32reg&(1<<1))!= 0)                                       // TI1 第一发送缓冲区发送完成中断

        {
            ;
        }
        if((CAN32reg&(1<<9))!= 0)                                       // TI2 第二发送缓冲区发送完成中断

        {
            ;
        }
        if((CAN32reg&(1<<10))!= 0)                                      // TI3 第三发送缓冲区发送完成中断
        {
            ;
        }
        */
        if((CAN32reg&(1<<7))!= 0)                                       /* BEI 总线错误中断             */
        {
            Enter_SWRst(j);
            regaddr = (unsigned long)(&LPC_CAN1->GSR)+j*CANOFFSET;                
            mes=RGE(regaddr);
            mes &=0x00ff;                                                
            RGE(regaddr)=mes;                                           /*总线错误清除处理              */
            Quit_SWRst(j);
        }
        if((CAN32reg&(1<<6))!= 0)                                       /* ALI 仲裁丢失中断             */
        {
            //添加用户代码
        }
        if((CAN32reg&(1<<3))!= 0)
        {
            //添加用户代码
            //ClrCanDataOver(j);                                        /*释放接收缓冲区                */
        }
        
    }
        
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
