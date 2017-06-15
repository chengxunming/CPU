/****************************************Copyright (c)****************************************************
** File name:                  I2CINT.c
** Latest modified Date:       2014-4-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Qinqin
** Created date:               2014-4-8
** Version:
** Descriptions:               LPC17ϵ��оƬӲ��I2C�������������Ҫ���ú�I2C���߽ӿ�(I2C���Ź��ܺ�
**                             I2C�жϣ�����ʹ��I2C��ģʽ)
**                             ����ԭMCU TWI.C�ļ��Ĺ���
*********************************************************************************************************/
#include "../User_code/global.h"
                                                                                  /* �������ں�I2C�жϴ�����Ϣ��   */                                                                       /* ȫ�ֱ���                     */
volatile uint8_t     I2C0_sla;                                             /* I2C�����ӵ�ַ                 */
volatile uint32_t    I2C0_suba;                                           /* I2C�����ڲ��ӵ�ַ             */
volatile uint8_t     I2C0_suba_num ;                                  /* I2C�ӵ�ַ�ֽ���               */
volatile uint8_t     *I2C0_buf;                                           /* ���ݻ�����ָ��                */
volatile uint32_t    I2C0_num;                                             /* Ҫ��ȡ/д������ݸ���         */
volatile uint8_t     I2C0_end;                                             /* I2C���߽�����־��������������1*/
volatile uint8_t     I2C0_suba_en;                                     /*  �ӵ�ַ���ơ�
																				  0--�ӵ�ַ�Ѿ�������߲���Ҫ�ӵ�ַ
                                                                                  1--��ȡ����
                                                                                  2--д����                        */
//ԭTWI.C�б���
void cntdn_black(void);
void cntdn8(void);


//���������ļ�ʹ��
static	uint8_t	TWIIndex;					//��������ָ��
static	uint8_t ComModule;					//��ǰͨѶģ��
static	uint8_t	TWICheckSum;				//У���
static	uint8_t	PatternIndex;				//Э������ָ��

static  volatile bool 	TWIfinishflg;				//TWIͨѶ������־


//ȫ�ֱ�������
uint8_t	FailTWI[4];					//TWI����ģ��

//PCU
uint8_t	TPCUSize;					//MCU������PCU���ݳ���
uint8_t	RPCUSize;					//MCU����PCU���ݳ���
uint8_t	TPCU[MAX_PCU_SIZE_T];		//MCU������PCU��������
signed char	RPCU[MAX_PCU_SIZE_R];	//MCU����PCU��������


//EIU
TWIEIU	TEIU;						//EIUͨѶ�ṹ������

//MCM
uint8_t	TMCMSize;					//MCM
uint8_t	RMCMSize;
uint8_t	TMCM[MAX_MCM_SIZE_T];
uint8_t	RMCM[MAX_MCM_SIZE_R];


uint8_t CountDownCode[17]={0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x09,0x11,0xC1,0x63,0x85,0x61,0x71,0xFD};


/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            �����ʱ���ṩ��I2C���ߵȴ���ʱʹ��
** input parameters:        dly        ��ʱ������ֵԽ����ʱԽ��
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
uint8_t  Wait_I2c_End(uint32_t  dly)
{  
	uint32_t  i;
	if( I2C0_end==1 ) return (1);
	for(; dly>0; dly--)
	for(i=0; i<5000; i++)
	{
		if( I2C0_end==1 ) return (1);
	}
	return (0);
	
    
}

/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        �����ӵ�ַ��������1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  ISendByte(uint8_t sla, uint8_t dat)
{                                                                       /* ��������                     */

	I2C0_sla     = sla;                                                  /* д������������ַ             */
	I2C0_buf     = &dat;                                                 /* �����͵�����                 */
	I2C0_num     = 1;                                                    /* ����1�ֽ�����                */
	I2C0_suba_en = 0;                                                    /* ���ӵ�ַ                     */
	I2C0_end     = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;                                            /* ����Ϊ����������������       */

	return( Wait_I2c_End(20));	
}

/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        �����ӵ�ַ������ȡ1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  IRcvByte(uint8_t sla, uint8_t *dat)
{
	I2C0_sla     = sla+1;                                                  /* ��������������ַ             */
	I2C0_buf     = dat;                                                 /* �����͵�����                 */
	I2C0_num     = 1;                                                    /* ����1�ֽ�����                */
	I2C0_suba_en = 0;                                                    /* ���ӵ�ַ                     */
	I2C0_end     = 0;

	LPC_I2C0->CONCLR = 0x2C;
	LPC_I2C0->CONSET = 0x60;                                            /* ����Ϊ����������������       */

	return( Wait_I2c_End(20));
}

/*********************************************************************************************************
** Function name:       I2C_ReadNByte
** Descriptions:        �����ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ���ݽ��ջ�����ָ��
**                      num         ��ȡ�ĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t I2C_ReadNByte (uint8_t sla, uint32_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
	if (num > 0)                                                        /* �ж�num�����ĺϷ���          */
	{                                                                   /* ��������                     */
		if (suba_type == 1)
		{                                                               /* �ӵ�ַΪ���ֽ�               */
			I2C0_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
			I2C0_suba        = suba;                                     /* �����ӵ�ַ                   */
			I2C0_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
		}
		if (suba_type == 2)
		{                                                               /* �ӵ�ַΪ2�ֽ�                */
			I2C0_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
			I2C0_suba        = suba;                                     /* �����ӵ�ַ                   */
			I2C0_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
		}
		if (suba_type == 3)
		{                                                               /* �ӵ�ַ�ṹΪ8+X              */
			I2C0_sla         = sla + ((suba >> 7 )& 0x0e) + 1;           /* �������Ĵӵ�ַ��R=1          */
			I2C0_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
			I2C0_suba_num    = 1;                                        /* �����ӵ�ַΪ8+x              */
		}
		I2C0_buf     = s;                                                /* ���ݽ��ջ�����ָ��           */
		I2C0_num     = num;                                              /* Ҫ��ȡ�ĸ���                 */
		I2C0_suba_en = 1;                                                /* ���ӵ�ַ��                   */
		I2C0_end     = 0;
		LPC_I2C0->CONCLR = (1 << 2)|                                    /* ���AA                       */
						   (1 << 3)|                                    /* ���SI                       */
						   (1 << 5);                                    /* ���STA                      */
		LPC_I2C0->CONSET = (1 << 5)|                                    /* ��λSTA                      */
						   (1 << 6);                                    /* I2Cʹ��                      */
		return( Wait_I2c_End(20));                                      /* �ȴ�I2C�������              */
	}
	return (FALSE);
	
	
	
}

/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        �����ӵ�ַ����д��N�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    2��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ��Ҫд������ݵ�ָ��
**                      num         ��Ҫд������ݵĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
{
    if (num > 0)                                                        /* �ж�num�����ĺϷ���          */
    {                                                                   /* ���ò���                     */
        if (suba_type == 1)
        {                                                               /* �ӵ�ַΪ���ֽ�               */
            I2C0_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C0_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C0_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
        }
        if (suba_type == 2)
        {                                                               /* �ӵ�ַΪ2�ֽ�                */
            I2C0_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C0_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C0_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
        }
        if (suba_type == 3)
        {                                                               /* �ӵ�ַ�ṹΪ8+X              */
            I2C0_sla         = sla + ((suba >> 7 )& 0x0e);               /* �������Ĵӵ�ַ               */
            I2C0_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
            I2C0_suba_num    = 1;                                        /* �����ӵ�ַΪ8+X              */
        }
        I2C0_buf     = s;                                                /* ����                         */
        I2C0_num     = num;                                              /* ���ݸ���                     */
        I2C0_suba_en = 2;                                                /* ���ӵ�ַ��д����             */
        I2C0_end     = 0;
        LPC_I2C0->CONCLR = (1 << 2)|                                    /* ���AA��־λ                 */
                           (1 << 3)|                                    /* ���SI��־λ                 */
                           (1 << 5);                                    /* ���STA��־λ                */
                                                                        /* ��λSTA,����I2C����          */
        LPC_I2C0->CONSET = (1 << 5)|                                    /* STA                          */
                           (1 << 6);                                    /* I2CEN                        */
        return( Wait_I2c_End(20));                                      /* �ȴ�I2C�������              */
    }
    return (FALSE);
}



/*********************************************************************************************************
** Function name:       I2C0_IRQHandler
** Descriptions:        Ӳ��I2C�жϷ������
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
void  I2C0_IRQHandler(void)
{   /*
     *��ȡI2C״̬�Ĵ���I2DAT,����ȫ�ֱ��������ý��в��������������־,����ж��߼�,�жϷ���
     */
    switch (LPC_I2C0->STAT & 0xF8)
    {
           /*
             ����״̬�������Ӧ�Ĵ���
           */
      	case 0x00:   
        LPC_I2C0->CONCLR = 0x28; 
        I2C0_end = 0xFF; 
        break; 
		case 0x08:    /* �ѷ�����ʼ����,�����ͺ������ն���,װ��SLA+W����SLA+R */
        if(I2C0_suba_en == 1)/* SLA+R */                                 /* ָ���ӵ�ַ��                 */
        {
           LPC_I2C0->DAT = I2C0_sla & 0xFE;                              /* ��д���ַ                   */
        }
        else                                                            /* SLA+W                        */
        {   LPC_I2C0->DAT = I2C0_sla;                                    /* ����ֱ�ӷ��ʹӻ���ַ         */
        }
                                                                        /* ����SIλ                     */
        LPC_I2C0->CONCLR = (1 << 3)|                                    /* SI                           */
                           (1 << 5);                                    /* STA                          */
        break;
        case 0x10:    /*�ѷ����ظ���ʼ���� */                           /* �����ͺ������ն���           */
                                                                        /* װ��SLA+W����SLA+R           */
        LPC_I2C0->DAT = I2C0_sla;                                        /* �������ߺ��ط��ӵ�ַ       */
        LPC_I2C0->CONCLR = 0x28;                                        /* ����SI,STA                   */
        break;
        case 0x18:
        case 0x28:                                                      /* �ѷ���I2DAT�е�����,�ѽ���ACK*/
        if (I2C0_suba_en == 0)
        {
            if (I2C0_num > 0)
            {   LPC_I2C0->DAT = *I2C0_buf++;
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C0_num--;
            }
            else                                                        /* û�����ݷ�����               */
            {                                                           /* ֹͣ����                     */
                LPC_I2C0->CONSET = (1 << 4);                            /* STO                          */
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C0_end = 1;                                            /* �����Ѿ�ֹͣ                 */
            }
        }
        if(I2C0_suba_en == 1)                                            /* ����ָ����ַ��,��������������*/
        {
            if (I2C0_suba_num == 2)
            {   LPC_I2C0->DAT = ((I2C0_suba >> 8) & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C0_suba_num--;
                break;
            }
            if(I2C0_suba_num == 1)
            {   LPC_I2C0->DAT = (I2C0_suba & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C0_suba_num--;
                break;
            }
            if (I2C0_suba_num == 0)
            {
                LPC_I2C0->CONCLR = 0x08;
                LPC_I2C0->CONSET = 0x20;
                I2C0_suba_en = 0;                                        /* �ӵ�ַ������                 */
                break;
            }
        }
        if (I2C0_suba_en == 2)                                           /* ָ���ӵ�ַд,�ӵ�ַ��δָ��, */
                                                                        /* �����ӵ�ַ                 */
        {
            if (I2C0_suba_num > 0)
            {    if (I2C0_suba_num == 2)
                {   LPC_I2C0->DAT = ((I2C0_suba >> 8) & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C0_suba_num--;
                    break;
                }
                if (I2C0_suba_num == 1)
                {   LPC_I2C0->DAT    = (I2C0_suba & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C0_suba_num--;
                    I2C0_suba_en  = 0;
                    break;
                }
            }
        }
        break;
        case 0x40:                                                      /* �ѷ���SLA+R,�ѽ���ACK        */
        if (I2C0_num <= 1)                                               /* ��������һ���ֽ�           */
        {    LPC_I2C0->CONCLR = 1 << 2;                                       /* �´η��ͷ�Ӧ���ź�           */
        }
        else
        {    LPC_I2C0->CONSET = 1 << 2;                                 /* �´η���Ӧ���ź�             */
        }
        LPC_I2C0->CONCLR = 0x28;                                        /* ����SI,STA                   */
        break;
        case 0x20:                                                      /* �ѷ���SLA+W,�ѽ��շ�Ӧ��     */
        case 0x30:                                                      /* �ѷ���I2DAT�е����ݣ��ѽ���  */
                                                                        /* ��Ӧ��                       */
        case 0x38:                                                      /* ��SLA+R/W�������ֽ��ж�ʧ��  */
                                                                        /* ��                           */
        case 0x48:                                                      /* �ѷ���SLA+R,�ѽ��շ�Ӧ��     */
        LPC_I2C0->CONCLR = 0x28;
        I2C0_end = 0xFF;
        break;
        case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
        *I2C0_buf++ = LPC_I2C0->DAT;
        I2C0_num--;
        if (I2C0_num == 1)                                               /* �������һ���ֽ�             */
        {
           LPC_I2C0->CONCLR = 0x2C;                                     /* STA,SI,AA = 0                */
        }
        else
        {   LPC_I2C0->CONSET = 0x04;                                    /* AA=1                         */
            LPC_I2C0->CONCLR = 0x28;
        }
        break;
        case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */
        *I2C0_buf++ = LPC_I2C0->DAT;                                     /* ��ȡ���һ�ֽ�����           */
        LPC_I2C0->CONSET = 0x10;                                        /* ��������                     */
        LPC_I2C0->CONCLR = 0x28;
        I2C0_end = 1;
        break;
        default:
        break;
    }
}

void overtimetwi_start(void)
{
	LPC_SC->PCONP |= (1 << 22);                                      /* �򿪶�ʱ��3���ʿ���λ       */  
	LPC_TIM2->TCR  = 0x02;
    LPC_TIM2->IR   = 0x01;                                                /*����ж�                   */
    LPC_TIM2->CTCR = 0x00;
    LPC_TIM2->TC   = 0x00;
    LPC_TIM2->PR   = 0x00;
    LPC_TIM2->MR0  = (PeripheralClock/28);                              /* Լ35ms�ж�1��                  */
    LPC_TIM2->MCR  = 0x03;                                              /* ƥ�������ж�               */
    
   NVIC_EnableIRQ(TIMER2_IRQn);                                        /* �����жϲ�ʹ��               */
    NVIC_SetPriority(TIMER2_IRQn, 3);
    LPC_TIM2->TCR  = 0x01;                                              /* ������ʱ��                   */
	
	/*TCCR3B 	= 0x00;													//�رն�ʱ��3
	uint8_t sreg	=SREG;
	_CLI();
	TCNT3	= 0xFDDE;												//35MS
	SREG	= sreg;													//ԭ�Ӳ���
	TIFR3   = 0x01;													//����жϱ�־λ
	TIMSK3 	= (1<<TOIE3);											//��ʱ��3����ж�ʹ��
	TCCR3B 	= 0x05;													//������ʱ��3����Ƶ��ֵ1024*/
}

void overtimetwi_close(void)
{
	LPC_TIM2->TCR  = 0x01;                                              /* �رն�ʱ��3                   */
	LPC_TIM2->MCR  = 0x00;                                              /* ����ƥ�������ж�            */
	//TCCR3B = 0; 														//�رն�ʱ��3
	//TIMSK3 = 0;															//��ʱ��3����ж�ʹ�ܹر�
}



/*---------------------------------TWI���Ϳ�ʼ--------------------------------*/
void TWI_SendStart(void) 
{ 
    
  	LPC_I2C1->CONCLR = (1 << 2)|                                    /* ���AA                       */
	                   (1 << 3)|                                    /* ���SI                       */
					   (1 << 5);                                    /* ���STA                      */
	LPC_I2C1->CONSET = (1 << 5)|                                    /* ��λSTA                      */
					   (1 << 6);                                    /* I2Cʹ��                      */ 
	//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT)|(1<<TWSTA);
} 


/*-----------------------����ֹͣ����������TWEA�Ա�ӽ���---------------------*/
void TWI_SendStop(void) 
{ 
	LPC_I2C1->CONCLR = (1<<2) |
	(1 << 3)|					
	(1 << 5);                                    /* ���STA                      */
	LPC_I2C1->CONSET = (1 << 2)|                                    /* ��λAA                       */
					   (1 << 4)|                                    /* ��λSTO                      */
					   (1 << 6);                                    /* I2Cʹ��                      */ 
	
	//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT)|(1<<TWEA)|(1<<TWSTO); 
} 


/*--------------------------------TWI���ֽڷ���-------------------------------*/
void TWI_SendByte(const uint8_t data) 
{ 
	
	//LPC_I2C1->DAT	=data;  																// װ�����ݵ� LPC_I2C1->DAT 
	LPC_I2C1->DAT =data;
	LPC_I2C1->CONSET =  0x04;                                 /* ��λAA                       */
	LPC_I2C1->CONCLR = 0x28; 
					   
	
					                                       /* I2Cʹ��                      */ 
	
	//TWCR	=TWCR & TWCR_CMD_MASK |(1<<TWINT);  									// ���Ϳ�ʼ
} 


//TWIͨѶ���������ȴ�����ʱ��35MS,Լ13MS���TWIͨѶ
void TWIStartAndWait(void)
{
	uint8_t retrycount=0;
	do
	{
		TWI_SendStart();														//����TWIͨѶ
		
		overtimetwi_start();
		
		TWIfinishflg	=0;
		while(TWIfinishflg==0)													//�ȴ�TWIͨѶ����
		{
			if(OverTimeFlg)
			{			
				OverTimeFlg =0;													//��ʱ�˳��ȴ�
				break;
			}
		}
		
		overtimetwi_close();
		
		if(TWIfinishflg==0)														//���δ�ɹ�
		{
			retrycount++;
			if(retrycount==4)													//����3�β��ɹ������˳�
			{
				FailTWI[BUST]	=1;												//�������������������޷�ͨѶ
				break;
			}
			else
			{
				TWIBus_init();
			}
		}
		else
			break;																//�ɹ���ֱ���˳�
	}
	while(1);
}
	
	
/****************************************************

* ���ƣ�	TWIBus_init

* ���ܣ�	TWI���߳�ʼ������

* ��ڲ�������

* ���ڣ�	��

* ��ע:		MCU��PCU,EIU,MCMͨѶǰ����TWI���߳�ʼ��

****************************************************/

void TWIBus_init(void) 
{ 
	
	NVIC_DisableIRQ(I2C1_IRQn);
	LPC_I2C1->CONCLR = 0x6C;
    LPC_I2C1->CONSET = 0x40;                                            /* ʹ������ģʽI2C*/
	NVIC_EnableIRQ(I2C1_IRQn);                                          /* ʹ��I2C�ж�                  */
}




//PCU��������׼��
void PCU_buf(void)
{
	TPCU[0]	=(PCU.ModuleRst<<0)|(PCU.TVFrd<<2)|(PCU.Illum<<4)|(PCU.LampLevel<<5)|(PCU.LampSwitch<<7);
	
	
	PCU.ModuleRst	=NO_RST;		//�Ա�����λ
}


//MCM��������׼��
void MCM_buf(void)
{
	TMCM[0]	=(MCM.AllRed<<0)|(MCM.KeypadStatus<<1)|(MCM.TSCFault<<2)|(MCM.LocalRemote<<3)|(MCM.RunMode<<5);
	TMCM[1]	=MCM.CurrentStage;
}


//PCU��ʼ���������̶�ֵ
void PCU_initial(void)
{
	PCU.ModuleRst	=NO_RST;												//��ƴ򿪣��ް��ƣ��Զ�����������ѹƵ���¶ȣ��޸�λ
	PCU.TVFrd		=RD_ALL;
	PCU.Illum		=AT_ILLUM;
	PCU.LampLevel	=SIGLEVEL1;
	PCU.LampSwitch	=LAMPON;
	
	PCU_buf();
	
	TPCUSize=1;
}


void EIU_initial(void)
{
	TEIU.tsize1=TEIU.tsize2=TEIU.tsize3=TEIU.tsize4=0;						//���ݳ���Ϊ0�������ݣ� 
}


void MCM_initial(void)
{
	MCM.AllRed			=0;													//NORMALģʽ������MCM���޴��󣬼��̹ر�
	MCM.KeypadStatus	=KP_OFF;
	MCM.TSCFault		=KP_NOFAULT;
	MCM.LocalRemote		=KP_LOCAL;
	MCM.RunMode			=KP_MODE_NORMAL;
	MCM.CurrentStage	=0;
	
	MCM_buf();

	TMCMSize=2;
	RMCMSize=1;
}


void select_commodule(const uint8_t module)
{
	if(module==COMPCU)
		ComModule=COMPCU;
	else if(module==COMEIU)									//ÿ�붼��ѯEIU��MCM����ʹEIU��MCM�й���
		ComModule=COMEIU;
	else if(module==COMMCM)
		ComModule	=COMMCM;
}

/****************************************************

* ���ƣ�	TWI_initial

* ���ܣ�	TWI�����豸��ʼ������

* ��ڲ�������

* ���ڣ�	��

* ��ע:		MCU��PCU,EIU,MCM�״�ͨѶ

****************************************************/

void TWI_initial(void)
{
	PCU_initial();															//��ģ��ͨѶ������ʼ��ֵ
	EIU_initial();
	MCM_initial();
	
	select_commodule(COMPCU);												//������˳��ѡ��ǰͨѶģ��
	
//	TWIStartAndWait();
}


void TWI_buf(void)
{
	PCU_buf();																//PCUģ�黺�����鸳ֵ
	
	cntdn8();		                                                        //���׶���8�鵹��ʱ�������鸳ֵEIUģ��
	
	MCM_buf();																//MCMģ�黺�����鸳ֵ
}

void Set485_Dir(uint8_t uartno,uint8_t dir)
{
	switch (uartno)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<31;                                            /*����P1.31Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<31;                                            /*����P1.31Ϊ�͵�ƽ  */
			break;
		case 3:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<30;                                            /*����P1.30Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<30;                                            /*����P1.30Ϊ�͵�ƽ  */
			break;
		case 4:
			if(dir)
				LPC_GPIO1->SET |= 1ul<<29;                                            /*����P1.29Ϊ�ߵ�ƽ  */
			else
				LPC_GPIO1->CLR |= 1ul<<29;                                            /*����P1.29Ϊ�͵�ƽ  */
			break;
	default:
		break;
	}
}

void Cntd_send(void)
{
	uint8_t SendA[16],SendB[16];
	
	//Aͨ�����͵�һ��·���ĸ�����ĵ���ʱ����
	for(uint8_t i=0;i<4;i++)
	{
		SendA[i*3]=0x50+i;
		if(TEIU.tbuf1[i*2])
		{
			SendA[i*3+1]=(CountDownCode[TEIU.tbuf1[i*2]/10]&0xFE)|((TEIU.tbuf1[i*2+1]>>1)&0x01);
			SendA[i*3+2]=(CountDownCode[TEIU.tbuf1[i*2]%10]&0xFE)|(TEIU.tbuf1[i*2+1]&0x01);
		}
		else
		{
			SendA[i*3+1]=0xFC|((TEIU.tbuf1[i*2+1]>>1)&0x01);
			SendA[i*3+2]=0xFC|(TEIU.tbuf1[i*2+1]&0x01);
		}
	}
	Set485_Dir(2,RS485_TX);
	UARTSend(2,SendA,12);
	memcpy(UART2Buffer,SendA,12);
	
	//Bͨ�����͵ڶ���·���ĸ�����ĵ���ʱ����
	for(uint8_t i=0;i<4;i++)
	{
		SendB[i*3]=0x50+i;
		if(TEIU.tbuf1[(i+4)*2])
		{
			SendB[i*3+1]=(CountDownCode[TEIU.tbuf1[(i+4)*2]/10]&0xFE)|((TEIU.tbuf1[(i+4)*2+1]>>1)&0x01);
			SendB[i*3+2]=(CountDownCode[TEIU.tbuf1[(i+4)*2]%10]&0xFE)|(TEIU.tbuf1[(i+4)*2+1]&0x01);
		}
		else
		{
			SendB[i*3+1]=0xFC|((TEIU.tbuf1[(i+4)*2+1]>>1)&0x01);
			SendB[i*3+2]=0xFC|(TEIU.tbuf1[(i+4)*2+1]&0x01);
		}
	}
	Set485_Dir(3,RS485_TX);
	UARTSend(3,SendB,12);
	memcpy(UART3Buffer,SendB,12);
}

//TWI������ѯ
void TWI_poll(void)
{

			
	TWI_buf();																//���ͻ�������׼��
	
	SSU_COMM();
	MAU_COMM();
	Cntd_send();
			
}


void fatalfault_TWI(uint8_t enable_ctdn)
{
	PCU_buf();													//PCUģ�黺�����鸳ֵ
	
	if(enable_ctdn)
		cntdn8();		                                        //���׶���8�鵹��ʱ�������鸳ֵEIUģ��
	else
		cntdn_black();
	
	MCM_buf();													//MCMģ�黺�����鸳ֵ
	
	SSU_COMM();
	MAU_COMM();
	
//	TWIStartAndWait();
}

/*********************************************************************************************************
** Function name:       I2C1_IRQHandler
** Descriptions:        Ӳ��I2C�жϷ������TWIͨѶ
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
void  I2C1_IRQHandler(void)
{   /*
     *��ȡI2C״̬�Ĵ���I2DAT,����ȫ�ֱ��������ý��в��������������־,����ж��߼�,�жϷ���
     */
	uchar status;
   	
   	status = LPC_I2C1->STAT & 0xF8; 											//��ȡTWI����״̬
	

	switch (status)
    {
           /*
             ����״̬�������Ӧ�Ĵ���
           */
      	case 0x00:   
         
		LPC_I2C1->CONSET = 0x54;
		LPC_I2C1->CONCLR = 0x08; 
        break; 
		case 0x08:    /* �ѷ�����ʼ����,�����ͺ������ն���,װ��SLA+W����SLA+R */
        case 0x10:    /*�ѷ����ظ���ʼ���� */                           /* �����ͺ������ն���           */
                                                                        /* װ��SLA+W����SLA+R           */
		if(ComModule==COMEIU)												//���ݵ�ǰͨѶģ�飬����SLA+W
			TWI_SendByte((TWIEIUADDR<<1)&0xfe);//((TWIEIUADDR<<1)& 0xFE); 								// RW Ϊ0: д����     	
		else if(ComModule==COMPCU)
			TWI_SendByte((TWIPCUADDR<<1)&0xfe);//((TWIPCUADDR<<1)& 0xFE); 
		else if(ComModule==COMMCM)
			TWI_SendByte((TWIMCMADDR<<1)&0xfe);//((TWIMCMADDR<<1)& 0xFE); 
		
		
		
		TWIIndex=0;															//������ָ�븴λ
		PatternIndex=0;
		
        break;
        case 0x18:
        case 0x28:                                                      /* �ѷ���I2DAT�е�����,�ѽ���ACK*/
        if(ComModule==COMPCU)
			{
				if(TWIIndex<TPCUSize)
					TWI_SendByte(TPCU[TWIIndex++]);								// �������� 
				else
				{
					TWIIndex=0;
					PatternIndex=0;
					TWI_SendStop();												// ����ֹͣ����������TWEA�Ա�ӽ���
				}
			}
			
			else if(ComModule==COMEIU)
			{
				if(PatternIndex==0)												//ͨ��1����֡����
				{
					PatternIndex=1;												//����ͨ��1���ݷ���
					
					TWICheckSum=0;
					
					TWICheckSum^=TEIU.tsize1;
                 
					TWI_SendByte(TEIU.tsize1);									//����ͨ��1����֡����
					
					return;
				}
				
				if(PatternIndex==1)												//ͨ��1����
				{
					if(TWIIndex<TEIU.tsize1)									//�����껺��������
					{
						TWICheckSum^=TEIU.tbuf1[TWIIndex];
						
                        TWI_SendByte(TEIU.tbuf1[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=2;											//����ͨ��2
					}
				}
				
				if(PatternIndex==2)												//ͨ��2����֡����
				{	
					PatternIndex=3;											//����ͨ��2���ݷ���
					
                    TWICheckSum^=TEIU.tsize2;
                 
					TWI_SendByte(TEIU.tsize2);								//����ͨ��2����֡����
					
					return;
				}
				
				if(PatternIndex==3)												//ͨ��2����
				{
					if(TWIIndex<TEIU.tsize2)									//�����껺��������
					{
						TWICheckSum^=TEIU.tbuf2[TWIIndex];
						
						TWI_SendByte(TEIU.tbuf2[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=4;											//����ͨ��3
					}
				}
				
				if(PatternIndex==4)												//ͨ��3����֡����
				{
					PatternIndex=5;											//����ͨ��3���ݷ���
					
                    TWICheckSum^=TEIU.tsize3;
                    
					TWI_SendByte(TEIU.tsize3);								//����ͨ��3����֡����
					
					return;
				}
				
				if(PatternIndex==5)												//ͨ��3����
				{
					if(TWIIndex<TEIU.tsize3)									//�����껺��������
					{
						TWICheckSum^=TEIU.tbuf3[TWIIndex];
                        
                        TWI_SendByte(TEIU.tbuf3[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=6;											//����ͨ��4
					}
				}
				
				if(PatternIndex==6)												//ͨ��4����֡����
				{
					PatternIndex=7;											//����ͨ��4���ݷ���
					
                    TWICheckSum^=TEIU.tsize4;
                    
					TWI_SendByte(TEIU.tsize4);								//����ͨ��4����֡����
					
					return;
				}
				
				if(PatternIndex==7)										//ͨ��4����
				{
					if(TWIIndex<TEIU.tsize4)									//�����껺��������
					{
						TWICheckSum^=TEIU.tbuf4[TWIIndex];
                        
                        TWI_SendByte(TEIU.tbuf4[TWIIndex++]);
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=8;											//����У����
					}
				}
				
				if(PatternIndex==8)             //����У����
				{
					TWI_SendByte(TWICheckSum);
					PatternIndex=9;											//
					return;
				}
				
				if(PatternIndex==9)             //����STOP
				{
					
					PatternIndex=0;											//
					TWI_SendStop();	
				}
			}
			
			else if(ComModule==COMMCM)
			{
				if(TWIIndex<TMCMSize)
					TWI_SendByte(TMCM[TWIIndex++]);								// �������� 
				else
				{
					TWIIndex=0;
					PatternIndex=0;	
					TWI_SendStop();												// ����ֹͣ����������TWEA�Ա�ӽ���
				}
			}
			
        break;
        case 0x40:                                                      /* �ѷ���SLA+R,�ѽ���ACK        */
        LPC_I2C1->CONCLR = 0x28;                                        /* ����SI,STA                   */
		LPC_I2C1->CONSET = 0x44;
        break;
		case 0x68:											// 0x68: own SLA+W has been received, ACK has been returned(Ӧ�ù���)
		case 0x70:													// 0x70:     GCA+W has been received, ACK has been returned(Ӧ�ù���)
		case 0x90:												// 0x90: data byte has been received, ACK has been returned(Ӧ�ù���)
		case 0x78:											// 0x78:     GCA+W has been received, ACK has been returned(Ӧ�ù���)
		case 0x88:													// 0x88: data byte has been received, NACK has been returned(Ӧ�ù���)
		case 0x98:												// 0x98: data byte has been received, NACK has been returned(Ӧ�ù���)
			
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x44; 	
			TWIfinishflg	=1;
		break;
        case 0x20:                                                      /* �ѷ���SLA+W,�ѽ��շ�Ӧ��     */
        case 0x30:                                                      /* �ѷ���I2DAT�е����ݣ��ѽ���  */
            switch(ComModule)
			{
				case COMPCU:	FailTWI[PCUT]	=1;	
								LPC_I2C1->CONCLR = 0x08; 	
								LPC_I2C1->CONSET = 0x54;			//���ع��ϣ�����STOP
								TWIfinishflg	=1;
								break;
				
				case COMEIU:	if(!FailTWI[EIUT])											//�����һ�ι��ϣ��������ϼ�¼
									FailTWI[EIUT]	=1;
								
								select_commodule(COMMCM);									//������MCMͨѶ
								LPC_I2C1->CONCLR = 0x08;
								LPC_I2C1->CONSET = 0x64;//�ط�START
								break;
				
				case COMMCM:	if(!FailTWI[MCMT])											//�����һ�ι��ϣ��������ϼ�¼
									FailTWI[MCMT]	=1;
								
								LPC_I2C1->CONCLR = 0x08; 	
								LPC_I2C1->CONSET = 0x54;			//��������ͨѶ������STOP
								TWIfinishflg	=1;
								break;
								
				default:break;
			} 	
            
		break; 
			                                                            /* ��Ӧ��                       */
        case 0x38:                                                      /* ��SLA+R/W�������ֽ��ж�ʧ��  */
                                                                        /* ��                           */
       
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x64;
		break;
		case 0x48:                                                      /* �ѷ���SLA+R,�ѽ��շ�Ӧ��     */
        LPC_I2C1->CONCLR = 0x08;
		LPC_I2C1->CONSET = 0x54;
        break;
		case 0x60:														// 0x60: own SLA+W has been received, ACK has been returned
			
			LPC_I2C1->CONCLR = 0x08;
			LPC_I2C1->CONSET = 0x44;
			
		break;
		case 0x80:													// 0x80: data byte has been received, ACK has been returned
			
			if(ComModule==COMPCU)
			{
				if(PatternIndex==0)												//�ظ���
				{
					
					TWICheckSum	=0;												//��ʼ��
					TWIIndex	=0;
					
					if(LPC_I2C1->DAT!=TPCU[0])											//�ظ����뷢���벻һ��
					{
						FailTWI[PCUT]	=1;	
					}
					else
					{
						PatternIndex++;											//��������֡����
						TWICheckSum ^=LPC_I2C1->DAT;
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				else if(PatternIndex==1)										//����֡����
				{
					RPCUSize =LPC_I2C1->DAT;
					
					PatternIndex++;												//��������֡
					TWICheckSum ^=RPCUSize;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
				}
				
				else if(PatternIndex==2)										//����֡
				{
					if(TWIIndex<RPCUSize)
					{
						RPCU[TWIIndex++] =LPC_I2C1->DAT;
						TWICheckSum ^=LPC_I2C1->DAT;
					}
					
					if(TWIIndex==RPCUSize)
						PatternIndex++;											//����У���
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				else if(PatternIndex==3)										//У���
				{
					if(TWICheckSum!=LPC_I2C1->DAT)
					{
						FailTWI[PCUT]	=1;	
					}
					else
					{
						PatternIndex=0;											//�ȴ�����STOP
					}
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
				}
			}
			
			else if(ComModule==COMEIU)
			{
				if(PatternIndex==0)												//ͨ��1����֡����
				{
					PatternIndex=1;												//����ͨ��1���ݽ���
					
					TWICheckSum	=0;												//��ʼ��
					TWIIndex	=0;
					
					TEIU.rsize1	=LPC_I2C1->DAT;											//��ȡ����֡����
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
					
					return;
				}
				
				if(PatternIndex==1)												//ͨ��1����
				{
					if(TWIIndex<TEIU.rsize1)									//���ջ���������
					{
						TEIU.rbuf1[TWIIndex++] =LPC_I2C1->DAT;							//
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=2;											//����ͨ��2
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==2)												//ͨ��2
				{	
					PatternIndex=3;
					
					TEIU.rsize2	=LPC_I2C1->DAT;											//��ȡ�������ݳ���
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
					
					return;
				}
				
				if(PatternIndex==3)												//ͨ��2����
				{
					if(TWIIndex<TEIU.rsize2)									//���ջ���������
					{
						TEIU.rbuf2[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=4;											//����ͨ��3
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==4)												//ͨ��3
				{
					PatternIndex=5;
					
					TEIU.rsize3	=LPC_I2C1->DAT;											//��ȡ�������ݳ���
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
					
					return;
				}
				
				if(PatternIndex==5)												//ͨ��3����
				{
					if(TWIIndex<TEIU.rsize3)									//���ջ���������
					{
						TEIU.rbuf3[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						LPC_I2C1->CONSET = 0x04; 
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=6;											//����ͨ��4
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==6)												//ͨ��4
				{
					PatternIndex=7;
					
					TEIU.rsize4	=LPC_I2C1->DAT;											//��ȡ�������ݳ���
					
					TWICheckSum ^=LPC_I2C1->DAT;
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44;  
					
					return;
				}
				
				if(PatternIndex==7)												//ͨ��4����
				{
					if(TWIIndex<TEIU.rsize4)									//���ջ���������
					{
						TEIU.rbuf4[TWIIndex++] =LPC_I2C1->DAT;
						
						TWICheckSum ^=LPC_I2C1->DAT;
						
						LPC_I2C1->CONSET = 0x04; 
						
						return;
					}
					else
					{
						TWIIndex=0;
						PatternIndex=8;											//����У���
					}
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
				
				if(PatternIndex==8)												//У���
				{
					if(TWICheckSum!=LPC_I2C1->DAT)
					{
						FailTWI[EIUT]	=1;	
					}
					else
					{
						PatternIndex=0;											//�ȴ�����STOP
					}
					
					LPC_I2C1->CONCLR = 0x0C;
					LPC_I2C1->CONSET = 0x44; 
				}
			}
			
			else if(ComModule==COMMCM)
			{
				if(TWIIndex<RMCMSize)
				{
					RMCM[TWIIndex++] =LPC_I2C1->DAT;										//�ȴ�����STOP
				}
				else
				{
					FailTWI[MCMT]	=1;
				}
				
				LPC_I2C1->CONCLR = 0x0C;
				LPC_I2C1->CONSET = 0x44; 
			}
			
		break;
		case 0xA0:														// 0xA0: STOP or REPEATED START has been received while addressed as slave
			
			if(ComModule==COMPCU)
			{
				if(LampTest.enable)												//�Ƽ����̣�ֻ��PCUͨѶ
				{
					//TWCR=TWCR & TWCR_CMD_MASK |(1<<TWINT);						//δѰַ�ӻ�ģʽ���������ߣ��ȴ���ΪMT
					//TWIfinishflg	=1;
					
					select_commodule(COMMCM);
					TWI_SendStart();
				}
				else
				{
					select_commodule(COMEIU);
					TWI_SendStart();											//SR�յ�STOP�󼴿�ʼ����START��ΪMT
				}
				
            }
			else if(ComModule==COMEIU)
			{
				select_commodule(COMMCM);
				TWI_SendStart();												//SR�յ�STOP�󼴿�ʼ����START��ΪMT
            }
			else if(ComModule==COMMCM)
			{
				LPC_I2C1->CONCLR = 0x08;
				LPC_I2C1->CONSET = 0x44; 							//δѰַ�ӻ�ģʽ���������ߣ��ȴ���ΪMT
				TWIfinishflg	=1;
            }
			
			break;
		case 0xB0:															// 0xF8: No relevant state information
				LPC_I2C1->CONCLR = 0x08;
				LPC_I2C1->CONSET = 0x64;	
			
		break;	
			
		// Misc
		case 0xF8:															// 0xF8: No relevant state information
			
			break;
        //case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
       // case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */ 
        default:
        break;
    }
}



/*********************************************************************************************************
** End Of File
*********************************************************************************************************/

