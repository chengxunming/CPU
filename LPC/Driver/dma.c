/****************************************Copyright (c)****************************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           dma.c
** Last modified Date:  2011-7-6
** Last Version:        V1.0
** Descriptions:        DMA��ʼ��
**
*********************************************************************************************************/

#include "../User_code/global.h"

volatile uint32_t DMATCCount  = 0;                                      /* �ն˼����жϷ�������         */
volatile uint32_t DMAErrCount = 0;                                      /* DMA�����жϷ�������          */
volatile uint32_t UART2DMADone = 1,UART3DMADone=1;                                     /* ͨ�����DMA������־       */


uint16_t UART2DMATXSIZE,UART3DMATXSIZE;//UART2DMATXSIZE,UART3DMATXSIZE,UART4DMATXSIZE;
//uint16_t UART0DMARXSIZE,UART1DMARXSIZE,UART2DMARXSIZE,UART3DMARXSIZE,UART4DMARXSIZE;


/*********************************************************************************************************
** �������ƣ�DMA_IRQHandler
** ����������DMA�жϴ���
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void DMA_IRQHandler(void) 
{
    uint32_t uiRegVal = 0;
    
    uiRegVal = LPC_GPDMA->IntTCStat;
    if (uiRegVal) {                                                     /* �����ն˼����ж�             */
        DMATCCount++;
        LPC_GPDMA->IntTCClear = uiRegVal;
        if (uiRegVal & (0x01<<0)) {                                     /* ��0ͨ����DMA�ն˼����ж����� */
            UART2DMADone = 1;
			LPC_GPDMACH0->CConfig  |= 0x00;	
			UART2DMATXSIZE = 0;
			memset(&UART2Buffer,0,sizeof(UART2Buffer));
			
	
        }
        else if (uiRegVal & (0x01<<1)) {                                /* ��1ͨ����DMA�ն˼����ж����� */
            UART3DMADone = 1;
			LPC_GPDMACH1->CConfig  |= 0x00;	
			UART3DMATXSIZE = 0;
			memset(&UART3Buffer,0,sizeof(UART3Buffer));
        }
        else if (uiRegVal & (0x01<<2)) {                                /* ��2ͨ����DMA�ն˼����ж����� */
           __NOP();
        }
        else if (uiRegVal & (0x01<<3)) {                                /* ��3ͨ����DMA�ն˼����ж����� */
            __NOP();
        }
    }
    
    uiRegVal = LPC_GPDMA->IntErrStat;
    if (uiRegVal) {                                                     /* ����DMA���ʹ����ж�          */
        DMAErrCount++;
        LPC_GPDMA->IntErrClr = uiRegVal;
    }
    return;
}


/*********************************************************************************************************
** �������ƣ�DMA_Init
** ����������DMA��ʼ��
** �����������
** ����ֵ  ����
*********************************************************************************************************/
void DMA_Init( void )
{
    LPC_SC->PCONP |= (1 << 29);                                         /* ����DMA����                  */
    LPC_SC->DMAREQSEL = 0x0000;                                         /* ѡ��DMA����                  */
    LPC_GPDMA->IntTCClear = 0x0F;                                       /* ���ͨ��0��3���ն˼����ж�   */
    LPC_GPDMA->IntErrClr  = 0x0F;                                       /* ���ͨ��0��3��DMA�����ж�    */
    LPC_GPDMA->Config = 0x01;                                           /* ʹ��DMA���ֽ�˳��ΪС��      */
        
    while (!(LPC_GPDMA->Config & 0x01));
    
    NVIC_EnableIRQ(DMA_IRQn);                                           /* ʹ��DMA�ж�����              */
    return;
}


/*********************************************************************************************************
** �������ƣ�DMAChannel_Init
** ������������ʼ���ض���DMAͨ��
** ���������uiChannelNum    DMAͨ����
**           uiDMAMode       DMAͨ��ģʽ
** ����ֵ  ����
*********************************************************************************************************/
uint32_t DMAChannel_Init(uint32_t uiChannelNum, uint32_t uiDMAMode)
{
    /* 
     * ����DMAͨ��0
     */
    if (uiChannelNum == 0) {
        UART2DMADone = 0;                                               /* ���DMA��ɲ�����־          */
        LPC_GPDMA->IntTCClear = 0x01 << 0;                              /* ���ͨ��0�ն˼����жϱ�־    */
        LPC_GPDMA->IntErrClr  = 0x01 << 0;                              /* ���ͨ��0DMA���ʹ����жϱ�־ */
        if (uiDMAMode == M2P) {                                         /* DMAģʽ���洢��������        */
            LPC_GPDMACH0->CSrcAddr  = (uint32_t)UART0Buffer;                 /* ͨ��0Դ��ַ                  */
            LPC_GPDMACH0->CDestAddr = LPC_UART0_BASE;                 /* ͨ��0Ŀ�ĵ�ַ                */

            LPC_GPDMACH0->CControl = 128 |         /* �����СΪUART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0x01 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
        }
        else if ( uiDMAMode == P2M ) {                                  /* DMAģʽ�����赽�洢��        */
            LPC_GPDMACH0->CSrcAddr  = LPC_UART0_BASE;                 /* ͨ��0Դ��ַ                  */
            LPC_GPDMACH0->CDestAddr = (uint32_t)UART0Buffer;                 /* ͨ��0Ŀ�ĵ�ַ                */

            LPC_GPDMACH0->CControl = 128 |         /* �����СΪUART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0x01 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
        } else {
            return (FALSE);
        }
    }
    /* 
     * ����DMAͨ��1
     */
    else if (uiChannelNum == 1) {
        UART3DMADone = 0;                                               /* ���DMA��ɲ�����־          */
        LPC_GPDMA->IntTCClear = 0x01<<1;                                /* ���ͨ��1�ն˼����жϱ�־    */
        LPC_GPDMA->IntErrClr  = 0x01<<1;                                /* ���ͨ��1DMA���ʹ����жϱ�־ */
        if (uiDMAMode == M2P) {                                         /* DMAģʽ���洢��������        */
            LPC_GPDMACH1->CSrcAddr  = (uint32_t)UART1Buffer;                 /* ͨ��1Դ��ַ                  */
            LPC_GPDMACH1->CDestAddr = (uint32_t)&LPC_UART1->THR;                 /* ͨ��1Ŀ�ĵ�ַ                */

            LPC_GPDMACH1->CControl = 128  |         /* �����СΪSSP_DMA_SIZE       */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0x01 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
        }
        else if (uiDMAMode == P2M) {                                    /* DMAģʽ�����赽�洢��        */
            LPC_GPDMACH1->CSrcAddr  = LPC_UART1_BASE;                 /* ͨ��1Դ��ַ                  */
            LPC_GPDMACH1->CDestAddr = (uint32_t)UART0Buffer;                 /* ͨ��1Ŀ�ĵ�ַ                */

            LPC_GPDMACH1->CControl = 128|         /* �����СΪUART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (1 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
        } else {
            return (FALSE);
        }
    }
    /* 
     * ����DMAͨ��2
     */
    else if (uiChannelNum == 2) {
        UART2DMADone = 0;                                               /* ���DMA��ɲ�����־          */
        LPC_GPDMA->IntTCClear = 0x01 << 2;                              /* ���ͨ��2�ն˼����жϱ�־    */
        LPC_GPDMA->IntErrClr  = 0x01 << 2;                              /* ���ͨ��2DMA���ʹ����жϱ�־ */

        if (uiDMAMode == M2P) {                                         /* DMAģʽ���洢��������        */
            LPC_GPDMACH2->CSrcAddr  = (uint32_t) UART0Buffer;                 /* ͨ��2Դ��ַ                  */
            LPC_GPDMACH2->CDestAddr = (uint32_t)UART0_DMA_TX_DST;                 /* ͨ��2Ŀ�ĵ�ַ                */

            LPC_GPDMACH2->CControl = (128 & 0x0FFF) |         /* �����СΪUART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
            }
        else if (uiDMAMode == P2M) {                                    /* DMAģʽ�����赽�洢��        */
            LPC_GPDMACH2->CSrcAddr  = (uint32_t)UART0_DMA_RX_SRC;                 /* ͨ��2Դ��ַ                  */
            LPC_GPDMACH2->CDestAddr = (uint32_t) UART0Buffer;                 /* ͨ��2Ŀ�ĵ�ַ                */

            LPC_GPDMACH2->CControl = (128 & 0x0FFF) |         /* �����СΪSSP_DMA_SIZE       */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                     0x80000000;                        /* ʹ���ն˼����ж�             */
       } else {
            return ( FALSE );
       }
    }
    /* 
     * ����DMAͨ��3
     */
    else if (uiChannelNum == 3) {
        UART2DMADone = 0;                                               /* ���DMA��ɲ�����־          */
        LPC_GPDMA->IntTCClear = 0x01<<3;                                /* ���ͨ��3�ն˼����жϱ�־    */
        LPC_GPDMA->IntErrClr  = 0x01<<3;                                /* ���ͨ��3DMA���ʹ����жϱ�־ */

        if (uiDMAMode == M2P) {                                         /* DMAģʽ���洢��������        */
        LPC_GPDMACH3->CSrcAddr  = (uint32_t) UART0Buffer;                     /* ͨ��3Դ��ַ                  */
        LPC_GPDMACH3->CDestAddr = (uint32_t) UART0_DMA_TX_DST;                     /* ͨ��3Ŀ�ĵ�ַ                */

        LPC_GPDMACH3->CControl = (128 & 0x0FFF) |             /* �����СΪUART0_DMA_SIZE      */
                                 (0x02 << 12) |                         /* Դͻ����СΪ8λ              */
                                 (0x02 << 15) |                         /* Ŀ��ͻ����СΪ8λ            */
                                 (0x00 << 18) |                         /* Դ������Ϊ1�ֽ�            */
                                 (0x00 << 21) |                         /* Ŀ�괫����Ϊ1�ֽ�          */
                                 (0 << 26) |                            /* ÿ�δ����Դ��ַ����         */
                                 (0 << 27) |                            /* ÿ�δ����Ŀ���ַ����       */
                                  0x80000000;                           /* ʹ���ն˼����ж�             */
        }
        else if ( uiDMAMode == P2M )                                    /* DMAģʽ�����赽�洢��        */
        {
            LPC_GPDMACH3->CSrcAddr  = UART0_DMA_RX_SRC;                 /* ͨ��3Դ��ַ                  */
            LPC_GPDMACH3->CDestAddr = (uint32_t) UART0Buffer;                 /* ͨ��3Ŀ�ĵ�ַ                */

            LPC_GPDMACH3->CControl = (128 & 0x0FFF) |         /* �����СΪUART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* Դͻ����СΪ8λ              */
                                     (0x02 << 15) |                     /* Ŀ��ͻ����СΪ8λ            */
                                     (0x00 << 18) |                     /* Դ������Ϊ1�ֽ�            */
                                     (0x00 << 21) |                     /* Ŀ�괫����Ϊ1�ֽ�          */
                                     (0 << 26) |                        /* ÿ�δ����Դ��ַ����         */
                                     (0 << 27) |                        /* ÿ�δ����Ŀ���ַ����       */
                                      0x80000000;                       /* ʹ���ն˼����ж�             */
            } else {
                return (FALSE);
            }
        }
    else {
        return (FALSE);
    }
    return(TRUE);
}


/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
