/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  I2CINT.c
** Latest modified Date:       2009-6-8
** Latest Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chenmingji
** Created date:               2009-6-8
** Version:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Modified by:                chenxuemin
** Modified date:              2011-7-21
** Version:
** Descriptions:               LPC17ϵ��оƬӲ��I2C�������������Ҫ���ú�I2C���߽ӿ�(I2C���Ź��ܺ�
**                             I2C�жϣ�����ʹ��I2C��ģʽ)
*********************************************************************************************************/
#include "I2CINT.h"
                                                                        /* �������ں�I2C�жϴ�����Ϣ��  */                                                                       /* ȫ�ֱ���                     */
volatile INT8U     I2C_sla;                                             /* I2C�����ӵ�ַ                */
volatile INT32U    I2C_suba;                                            /* I2C�����ڲ��ӵ�ַ            */
volatile INT8U     I2C_suba_num;                                        /* I2C�ӵ�ַ�ֽ���              */
volatile INT8U     *I2C_buf;                                            /* ���ݻ�����ָ��               */
volatile INT32U    I2C_num;                                             /* Ҫ��ȡ/д������ݸ���        */
volatile INT8U     I2C_end;                                             /* I2C���߽�����־������������  */
                                                                        /* ��1                          */
volatile INT8U     I2C_suba_en;                                         /*  �ӵ�ַ���ơ�
                                                                         0--�ӵ�ַ�Ѿ�������߲���Ҫ�ӵ�ַ
                                                                         1--��ȡ����
                                                                         2--д����                      */

/*********************************************************************************************************
** �������ƣ�I2C0Init
** ����������I2C0��ʼ��
** ���������FI2C0    I2C0����Ƶ��(���400K)
** ����ֵ  ����
*********************************************************************************************************/
void I2C0Init(uint32_t ulFI2C0)
{
    LPC_SC->PCONP |= (0x01 << 7);                                      /* ��I2C0ģ�鹦�ʿ���λ       */
    LPC_SC->PCONP |= (0x01 << 15);                                      /* ��GPIOģ�鹦�ʿ���λ       */
    LPC_IOCON->P0_27 = 0x01 | (0x00 << 3) | (0x01 << 10);               /* P0.27����ΪSDA0����          */
    LPC_IOCON->P0_28 = 0x01 | (0x00 << 3) | (0x01 << 10);               /* P0.28����ΪSCL0����          */
    if (ulFI2C0 > 400000) {
        ulFI2C0 = 400000;
    }
    LPC_I2C0->SCLH = (PeripheralClock / ulFI2C0) / 2;                   /* ����I2Cʱ��Ϊfi2c            */
    LPC_I2C0->SCLL = (PeripheralClock / ulFI2C0) / 2;
    NVIC_EnableIRQ(I2C0_IRQn);                                          /* ʹ��I2C�ж�                  */
    NVIC_SetPriority(I2C0_IRQn, 4);                                     /* ����I2C0�ж����ȼ���ʹ���ж� */
    LPC_I2C0->CONCLR = 0x2C;
    LPC_I2C0->CONSET = 0x40;                                            /* ʹ������ģʽI2C                    */
}

/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            �����ʱ���ṩ��I2C���ߵȴ���ʱʹ��
** input parameters:        dly        ��ʱ������ֵԽ����ʱԽ��
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
INT8U  Wait_I2c_End(INT32U  dly)
{  INT32U  i;

    if( I2C_end==1 ) return (1);
    for(; dly>0; dly--)
    for(i=0; i<5000; i++)
    {
        if( I2C_end==1 ) return (1);
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
INT8U  ISendByte(INT8U sla, INT8U dat)
{                                                                       /* ��������                     */
    I2C_sla     = sla;                                                  /* д������������ַ             */
    I2C_buf     = &dat;                                                 /* �����͵�����                 */
    I2C_num     = 1;                                                    /* ����1�ֽ�����                */
    I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
    I2C_end     = 0;

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
INT8U  IRcvByte(INT8U sla, INT8U *dat)
{
    I2C_sla     = sla+1;                                                /* ��������������ַ             */
    I2C_buf     = dat;
    I2C_num     = 1;
    I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
    I2C_end     = 0;

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
INT8U I2C_ReadNByte (INT8U sla, INT32U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* �ж�num�����ĺϷ���          */
    {                                                                   /* ��������                     */
        if (suba_type == 1)
        {                                                               /* �ӵ�ַΪ���ֽ�               */
            I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
        }
        if (suba_type == 2)
        {                                                               /* �ӵ�ַΪ2�ֽ�                */
            I2C_sla         = sla + 1;                                  /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
        }
        if (suba_type == 3)
        {                                                               /* �ӵ�ַ�ṹΪ8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e) + 1;           /* �������Ĵӵ�ַ��R=1          */
            I2C_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ8+x              */
        }
        I2C_buf     = s;                                                /* ���ݽ��ջ�����ָ��           */
        I2C_num     = num;                                              /* Ҫ��ȡ�ĸ���                 */
        I2C_suba_en = 1;                                                /* ���ӵ�ַ��                   */
        I2C_end     = 0;
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
INT8U I2C_WriteNByte(INT8U sla, INT8U suba_type, INT32U suba, INT8U *s, INT32U num)
{
    if (num > 0)                                                        /* �ж�num�����ĺϷ���          */
    {                                                                   /* ���ò���                     */
        if (suba_type == 1)
        {                                                               /* �ӵ�ַΪ���ֽ�               */
            I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ1�ֽ�            */
        }
        if (suba_type == 2)
        {                                                               /* �ӵ�ַΪ2�ֽ�                */
            I2C_sla         = sla;                                      /* �������Ĵӵ�ַ               */
            I2C_suba        = suba;                                     /* �����ӵ�ַ                   */
            I2C_suba_num    = 2;                                        /* �����ӵ�ַΪ2�ֽ�            */
        }
        if (suba_type == 3)
        {                                                               /* �ӵ�ַ�ṹΪ8+X              */
            I2C_sla         = sla + ((suba >> 7 )& 0x0e);               /* �������Ĵӵ�ַ               */
            I2C_suba        = suba & 0x0ff;                             /* �����ӵ�ַ                   */
            I2C_suba_num    = 1;                                        /* �����ӵ�ַΪ8+X              */
        }
        I2C_buf     = s;                                                /* ����                         */
        I2C_num     = num;                                              /* ���ݸ���                     */
        I2C_suba_en = 2;                                                /* ���ӵ�ַ��д����             */
        I2C_end     = 0;
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
        I2C_end = 0xFF; 
        break; 
		case 0x08:    /* �ѷ�����ʼ����,�����ͺ������ն���,װ��SLA+W����SLA+R */
        if(I2C_suba_en == 1)/* SLA+R */                                 /* ָ���ӵ�ַ��                 */
        {
           LPC_I2C0->DAT = I2C_sla & 0xFE;                              /* ��д���ַ                   */
        }
        else                                                            /* SLA+W                        */
        {   LPC_I2C0->DAT = I2C_sla;                                    /* ����ֱ�ӷ��ʹӻ���ַ         */
        }
                                                                        /* ����SIλ                     */
        LPC_I2C0->CONCLR = (1 << 3)|                                    /* SI                           */
                           (1 << 5);                                    /* STA                          */
        break;
        case 0x10:    /*�ѷ����ظ���ʼ���� */                           /* �����ͺ������ն���           */
                                                                        /* װ��SLA+W����SLA+R           */
        LPC_I2C0->DAT = I2C_sla;                                        /* �������ߺ��ط��ӵ�ַ       */
        LPC_I2C0->CONCLR = 0x28;                                        /* ����SI,STA                   */
        break;
        case 0x18:
        case 0x28:                                                      /* �ѷ���I2DAT�е�����,�ѽ���ACK*/
        if (I2C_suba_en == 0)
        {
            if (I2C_num > 0)
            {   LPC_I2C0->DAT = *I2C_buf++;
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C_num--;
            }
            else                                                        /* û�����ݷ�����               */
            {                                                           /* ֹͣ����                     */
                LPC_I2C0->CONSET = (1 << 4);                            /* STO                          */
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C_end = 1;                                            /* �����Ѿ�ֹͣ                 */
            }
        }
        if(I2C_suba_en == 1)                                            /* ����ָ����ַ��,��������������*/
        {
            if (I2C_suba_num == 2)
            {   LPC_I2C0->DAT = ((I2C_suba >> 8) & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C_suba_num--;
                break;
            }
            if(I2C_suba_num == 1)
            {   LPC_I2C0->DAT = (I2C_suba & 0xff);
                LPC_I2C0->CONCLR = 0x28;                                /* ����SI,STA                   */
                I2C_suba_num--;
                break;
            }
            if (I2C_suba_num == 0)
            {
                LPC_I2C0->CONCLR = 0x08;
                LPC_I2C0->CONSET = 0x20;
                I2C_suba_en = 0;                                        /* �ӵ�ַ������                 */
                break;
            }
        }
        if (I2C_suba_en == 2)                                           /* ָ���ӵ�ַд,�ӵ�ַ��δָ��, */
                                                                        /* �����ӵ�ַ                 */
        {
            if (I2C_suba_num > 0)
            {    if (I2C_suba_num == 2)
                {   LPC_I2C0->DAT = ((I2C_suba >> 8) & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C_suba_num--;
                    break;
                }
                if (I2C_suba_num == 1)
                {   LPC_I2C0->DAT    = (I2C_suba & 0xff);
                    LPC_I2C0->CONCLR = 0x28;
                    I2C_suba_num--;
                    I2C_suba_en  = 0;
                    break;
                }
            }
        }
        break;
        case 0x40:                                                      /* �ѷ���SLA+R,�ѽ���ACK        */
        if (I2C_num <= 1)                                               /* ��������һ���ֽ�           */
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
        I2C_end = 0xFF;
        break;
        case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
        *I2C_buf++ = LPC_I2C0->DAT;
        I2C_num--;
        if (I2C_num == 1)                                               /* �������һ���ֽ�             */
        {
           LPC_I2C0->CONCLR = 0x2C;                                     /* STA,SI,AA = 0                */
        }
        else
        {   LPC_I2C0->CONSET = 0x04;                                    /* AA=1                         */
            LPC_I2C0->CONCLR = 0x28;
        }
        break;
        case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */
        *I2C_buf++ = LPC_I2C0->DAT;                                     /* ��ȡ���һ�ֽ�����           */
        LPC_I2C0->CONSET = 0x10;                                        /* ��������                     */
        LPC_I2C0->CONCLR = 0x28;
        I2C_end = 1;
        break;
        default:
        break;
    }
}
/*********************************************************************************************************
** Function name:       I2C1_IRQHandler
** Descriptions:        Ӳ��I2C�жϷ������
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
//void  I2C1_IRQHandler(void)
//{   /*
//     *��ȡI2C״̬�Ĵ���I2DAT,����ȫ�ֱ��������ý��в��������������־,����ж��߼�,�жϷ���
//     */
//    switch (LPC_I2C1->STAT & 0xF8)
//    {
//           /*
//             ����״̬�������Ӧ�Ĵ���
//           */
//        case 0x08:    /* �ѷ�����ʼ����,�����ͺ������ն���,װ��SLA+W����SLA+R */
//        if(I2C_suba_en == 1)/* SLA+R */                                 /* ָ���ӵ�ַ��                 */
//        {
//           LPC_I2C1->DAT = 0x50&0xfe;//I2C_sla & 0xFE;                              /* ��д���ַ                   */
//        }
//        else                                                            /* SLA+W                        */
//        {   LPC_I2C1->DAT = I2C_sla;                                    /* ����ֱ�ӷ��ʹӻ���ַ         */
//        }
//                                                                        /* ����SIλ                     */
//        LPC_I2C1->CONCLR = (1 << 3)|                                    /* SI                           */
//                           (1 << 5);                                    /* STA                          */
//        break;
//        case 0x10:    /*�ѷ����ظ���ʼ���� */                           /* �����ͺ������ն���           */
//                                                                        /* װ��SLA+W����SLA+R           */
//        LPC_I2C1->DAT = I2C_sla;                                        /* �������ߺ��ط��ӵ�ַ       */
//        LPC_I2C1->CONCLR = 0x28;                                        /* ����SI,STA                   */
//        break;
//        case 0x18:
//        case 0x28:                                                      /* �ѷ���I2DAT�е�����,�ѽ���ACK*/
//        if (I2C_suba_en == 0)
//        {
//            if (I2C_num > 0)
//            {   LPC_I2C1->DAT = *I2C_buf++;
//                LPC_I2C1->CONCLR = 0x28;                                /* ����SI,STA                   */
//                I2C_num--;
//            }
//            else                                                        /* û�����ݷ�����               */
//            {                                                           /* ֹͣ����                     */
//                LPC_I2C1->CONSET = (1 << 4);                            /* STO                          */
//                LPC_I2C1->CONCLR = 0x28;                                /* ����SI,STA                   */
//                I2C_end = 1;                                            /* �����Ѿ�ֹͣ                 */
//            }
//        }
//        if(I2C_suba_en == 1)                                            /* ����ָ����ַ��,��������������*/
//        {
//            if (I2C_suba_num == 2)
//            {   LPC_I2C1->DAT = ((I2C_suba >> 8) & 0xff);
//                LPC_I2C1->CONCLR = 0x28;                                /* ����SI,STA                   */
//                I2C_suba_num--;
//                break;
//            }
//            if(I2C_suba_num == 1)
//            {   LPC_I2C1->DAT = (I2C_suba & 0xff);
//                LPC_I2C1->CONCLR = 0x28;                                /* ����SI,STA                   */
//                I2C_suba_num--;
//                break;
//            }
//            if (I2C_suba_num == 0)
//            {
//                LPC_I2C1->CONCLR = 0x08;
//                LPC_I2C1->CONSET = 0x20;
//                I2C_suba_en = 0;                                        /* �ӵ�ַ������                 */
//                break;
//            }
//        }
//        if (I2C_suba_en == 2)                                           /* ָ���ӵ�ַд,�ӵ�ַ��δָ��, */
//                                                                        /* �����ӵ�ַ                 */
//        {
//            if (I2C_suba_num > 0)
//            {    if (I2C_suba_num == 2)
//                {   LPC_I2C1->DAT = ((I2C_suba >> 8) & 0xff);
//                    LPC_I2C1->CONCLR = 0x28;
//                    I2C_suba_num--;
//                    break;
//                }
//                if (I2C_suba_num == 1)
//                {   LPC_I2C1->DAT    = (I2C_suba & 0xff);
//                    LPC_I2C1->CONCLR = 0x28;
//                    I2C_suba_num--;
//                    I2C_suba_en  = 0;
//                    break;
//                }
//            }
//        }
//        break;
//        case 0x40:                                                      /* �ѷ���SLA+R,�ѽ���ACK        */
//        if (I2C_num <= 1)                                               /* ��������һ���ֽ�           */
//        {    LPC_I2C1->CONCLR = 1 << 2;                                       /* �´η��ͷ�Ӧ���ź�           */
//        }
//        else
//        {    LPC_I2C1->CONSET = 1 << 2;                                 /* �´η���Ӧ���ź�             */
//        }
//        LPC_I2C1->CONCLR = 0x28;                                        /* ����SI,STA                   */
//        break;
//        case 0x20:                                                      /* �ѷ���SLA+W,�ѽ��շ�Ӧ��     */
//        case 0x30:                                                      /* �ѷ���I2DAT�е����ݣ��ѽ���  */
//                                                                        /* ��Ӧ��                       */
//        case 0x38:                                                      /* ��SLA+R/W�������ֽ��ж�ʧ��  */
//                                                                        /* ��                           */
//        case 0x48:                                                      /* �ѷ���SLA+R,�ѽ��շ�Ӧ��     */
//        LPC_I2C1->CONCLR = 0x28;
//        I2C_end = 0xFF;
//        break;
//        case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
//        *I2C_buf++ = LPC_I2C1->DAT;
//        I2C_num--;
//        if (I2C_num == 1)                                               /* �������һ���ֽ�             */
//        {
//           LPC_I2C1->CONCLR = 0x2C;                                     /* STA,SI,AA = 0                */
//        }
//        else
//        {   LPC_I2C1->CONSET = 0x04;                                    /* AA=1                         */
//            LPC_I2C1->CONCLR = 0x28;
//        }
//        break;
//        case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */
//        *I2C_buf++ = LPC_I2C1->DAT;                                     /* ��ȡ���һ�ֽ�����           */
//        LPC_I2C1->CONSET = 0x10;                                        /* ��������                     */
//        LPC_I2C1->CONCLR = 0x28;
//        I2C_end = 1;
//        break;
//        default:
//        break;
//    }
//}

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/

