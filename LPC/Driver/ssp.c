/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           ssp.c
** Last modified Date:  2014-3-6
** Last Version:        V1.0
** Descriptions:        SSP��ʼ��
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Qinqin
** Created date:        2011-7-6
** Version:             V1.0
** Descriptions:        SSP��ʼ��
**
*********************************************************************************************************/
#include "LPC177x_8x.h"
#include "ssp.h"

/* 
 * �ж�״̬��־
 */
volatile uint32_t interrupt0RxStat = 0;
volatile uint32_t interrupt0OverRunStat = 0;
volatile uint32_t interrupt0RxTimeoutStat = 0;
volatile uint32_t interrupt1RxStat = 0;
volatile uint32_t interrupt1OverRunStat = 0;
volatile uint32_t interrupt1RxTimeoutStat = 0;
volatile uint32_t interrupt2RxStat = 0;
volatile uint32_t interrupt2OverRunStat = 0;
volatile uint32_t interrupt2RxTimeoutStat = 0;

/*********************************************************************************************************
** Function name:       SSP0_IRQHandler
** Descriptions:        SSP0�жϺ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP0_IRQHandler(void)
{
    uint32_t regValue;

    regValue = LPC_SSP0->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* ����ж�                     */
        interrupt0OverRunStat++;
        LPC_SSP0->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* ���ճ�ʱ�ж�                 */
        interrupt0RxTimeoutStat++;
        LPC_SSP0->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO������һ��Ϊ��        */
        interrupt0RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP1_IRQHandler
** Descriptions:        SSP1�жϺ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP1_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP1->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* ����ж�                     */
        interrupt1OverRunStat++;
        LPC_SSP1->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* ���ճ�ʱ�ж�                 */
        interrupt1RxTimeoutStat++;
        LPC_SSP1->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO������һ��Ϊ��        */
        interrupt1RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP2_IRQHandler
** Descriptions:        SSP2�жϺ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP2_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP2->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* ����ж�                     */
        interrupt2OverRunStat++;
        LPC_SSP2->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* ���ճ�ʱ�ж�                 */
        interrupt2RxTimeoutStat++;
        LPC_SSP2->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO������һ��Ϊ��        */
        interrupt2RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP_SSELSetDirection
** Descriptions:        ѡ��SSP��CS�ܽţ�ʵ�ֶ����ƽ�ɿ�
** input parameters:    uiPortNum      SSP�˿ں�
**                      uiLocation   CS�ܽ�λ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP_SSELSetDirection(uint32_t uiPortNum, uint32_t uiLocation)
{
    if (uiPortNum == 0) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P2_23 &= ~0x07;                                  /* P2.23ΪSSP0��CS�ܽ�          */
            LPC_GPIO2->DIR |= (0x1<<23);
            LPC_GPIO2->SET |= (0x1<<23);
            break;
        case 1:
            LPC_IOCON->P1_21 &= ~0x07;                                  /* P1.21ΪSSP0��CS�ܽ�          */
            LPC_GPIO1->DIR |= (0x1<<21);
            LPC_GPIO1->SET |= (0x1<<21);
            break;
        case 2:
            LPC_IOCON->P1_28 &= ~0x07;                                  /* P1.28ΪSSP0��CS�ܽ�          */
            LPC_GPIO1->DIR |= (0x1<<28);
            LPC_GPIO1->SET |= (0x1<<28);
            break;
        case 3:
            LPC_IOCON->P0_16 &= ~0x07;                                  /* P0.16ΪSSP0��CS�ܽ�          */
            LPC_GPIO0->DIR |= (0x1<<16);
            LPC_GPIO0->SET |= (0x1<<16);
            break;
        }
    }
    else if (uiPortNum == 1) {
        switch (uiLocation) {
        case 0:
            LPC_IOCON->P0_14 &= ~0x07;                                  /* P0.14ΪSSP1��CS�ܽ�          */
            LPC_GPIO0->DIR |= (0x1<<14);
            LPC_GPIO0->SET |= (0x1<<14);
            break;
        case 1:
            LPC_IOCON->P1_26 &= ~0x07;                                  /* P1.26ΪSSP1��CS�ܽ�          */
            LPC_GPIO1->DIR |= (0x1<<26);
            LPC_GPIO1->SET |= (0x1<<26);
            break;
        case 2:
            LPC_IOCON->P4_21 &= ~0x07;                                  /* P4.21ΪSSP1��CS�ܽ�          */
            LPC_GPIO4->DIR |= (0x1<<21);
            LPC_GPIO4->SET |= (0x1<<21);
            break;
        case 3:
            LPC_IOCON->P0_6 &= ~0x07;                                  /* P0.6ΪSSP1��CS�ܽ�           */
            LPC_GPIO0->DIR |= (0x1<<6);
            LPC_GPIO0->SET |= (0x1<<6);
            break;
       }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P5_3 &= ~0x07;                                  /* P5.3ΪSSP2��CS�ܽ�           */
            LPC_GPIO5->DIR |= (0x1<<3);
            LPC_GPIO5->SET |= (0x1<<3);
            break;
        case 1:
            LPC_IOCON->P1_8 &= ~0x07;                                  /* P1.8ΪSSP2��CS�ܽ�           */
            LPC_GPIO1->DIR |= (0x1<<8);
            LPC_GPIO1->SET |= (0x1<<8);
            break;
        default:
            break;
        }
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP_SSELToggle
** Descriptions:        ����SSP��CS�ܽŵ�ƽ
** input parameters:    uiPortNum      SSP�˿ں�
**                      uiLocation   SSEL�ܽ�λ��
**                      toggle       SSEL�ܽŵ�ƽ���ƣ�0Ϊ�͵�ƽ��1Ϊ�ߵ�ƽ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP_SSELToggle( uint32_t uiPortNum, uint32_t uiLocation, uint32_t toggle )
{
    if ( uiPortNum == 0 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO2->CLR |= (0x1<<23);                            /* P2.23ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO2->SET |= (0x1<<23);                            /* P2.23ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<21);                            /* P1.21ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO1->SET |= (0x1<<21);                            /* P1.21ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<28);                            /* P1.28ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO1->SET |= (0x1<<28);                            /* P1.28ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<16);                            /* P0.16ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO0->SET |= (0x1<<16);                            /* P0.16ΪCS����Ϊ�ߵ�ƽ        */
            break;
        }
    }
    else if ( uiPortNum == 1 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<14);                            /* P0.14ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO0->SET |= (0x1<<14);                            /* P0.17ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<26);                            /* P1.26ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO1->SET |= (0x1<<26);                            /* P1.26ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO4->CLR |= (0x1<<21);                            /* P4.21ΪCS����Ϊ�͵�ƽ        */
            else
                LPC_GPIO4->SET |= (0x1<<21);                            /* P4.21ΪCS����Ϊ�ߵ�ƽ        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<6);                            /* P0.6ΪCS����Ϊ�͵�ƽ         */
            else
                LPC_GPIO0->SET |= (0x1<<6);                            /* P0.6ΪCS����Ϊ�ߵ�ƽ         */
            break;
        }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO5->CLR |= (0x1<<3);                            /* P5.3ΪCS����Ϊ�͵�ƽ         */
            else
                LPC_GPIO5->SET |= (0x1<<3);                            /* P5.3ΪCS����Ϊ�ߵ�ƽ         */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<8);                            /* P1.8ΪCS����Ϊ�͵�ƽ         */
            else
                LPC_GPIO1->SET |= (0x1<<8);                            /* P1.8ΪCS����Ϊ�ߵ�ƽ         */
            break;
        default:
            break;
        }
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP0Init
** Descriptions:        SSP0��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP0Init( uint32_t uiLocation )
{
    uint8_t i = 0, uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0;

    LPC_SC->PCONP |= (0x1 << 21);                                       /* ����SSP0����                 */

    /* 
     * �趨�����Ƶֵ
     */
    uiRegVal = LPC_SC->PCLKSEL;
    if (uiRegVal < 4) {
        LPC_SC->PCLKSEL = 4;
    }

    /* 
     * ѡ���ض�λ�ù��ܹܽ�
     */
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P2_22 &= ~0x07;
        LPC_IOCON->P2_22 |=  0x02;                                      /* SSP CLK                      */
        LPC_IOCON->P2_23 &= ~0x07;    
        LPC_IOCON->P2_23 |=  0x02;                                      /* SSP SSEL                     */
        LPC_IOCON->P2_26 &= ~0x07;
        LPC_IOCON->P2_26 |=  0x02;                                      /* SSP MISO                     */
        LPC_IOCON->P2_27 &= ~0x07;    
        LPC_IOCON->P2_27 |=  0x02;                                      /* SSP MOSI                     */
        break;

    case 1:
        LPC_IOCON->P1_20 &= ~0x07;
        LPC_IOCON->P1_20 |=  0x05;                                      /* SSP CLK                      */
        LPC_IOCON->P1_21 &= ~0x07;    
        LPC_IOCON->P1_21 |=  0x03;                                      /* SSP SSEL                     */
        LPC_IOCON->P1_23 &= ~0x07;
        LPC_IOCON->P1_23 |=  0x05;                                      /* SSP MISO                     */
        LPC_IOCON->P1_24 &= ~0x07;    
        LPC_IOCON->P1_24 |=  0x05;                                      /* SSP MOSI                     */
        break;

    case 2:
        LPC_IOCON->P1_20 &= ~0x07;
        LPC_IOCON->P1_20 |=  0x05;                                      /* SSP CLK                      */
        LPC_IOCON->P1_28 &= ~0x07;    
        LPC_IOCON->P1_28 |=  0x05;                                      /* SSP SSEL                     */
        LPC_IOCON->P1_23 &= ~0x07;
        LPC_IOCON->P1_23 |=  0x05;                                      /* SSP MISO                     */
        LPC_IOCON->P1_24 &= ~0x07;    
        LPC_IOCON->P1_24 |=  0x05;                                      /* SSP MOSI                     */
        break;

    case 3:
        LPC_IOCON->P0_15 &= ~0x07;
        LPC_IOCON->P0_15 |=  0x02;                                      /* SSP CLK                      */
        LPC_IOCON->P0_16 &= ~0x07;    
        LPC_IOCON->P0_16 |=  0x02;                                      /* SSP SSEL                     */
        LPC_IOCON->P0_17 &= ~0x07;
        LPC_IOCON->P0_17 |=  0x02;                                      /* SSP MISO                     */
        LPC_IOCON->P0_18 &= ~0x07;    
        LPC_IOCON->P0_18 |=  0x02;                                      /* SSP MOSI                     */
        break;
    default: 
        break;
    }

    /* 
     * ��ʹ�ùܽŵ�CS����ʱ���˹�����ʵ��ʹ�ܿ���
     */
    #if !USE_CS
        SSP_SSELSetDirection( 0, uiLocation );
    #endif

    LPC_SSP0->CR0 = 0x07 << 0 |                                         /* ���ݳ���Ϊ8λ                */
                    0x00 << 4 |                                         /* ֡��ʽΪSPI                  */
                    0x00 << 6 |                                         /* CPOLΪ0                      */
                    0x00 << 7 |                                         /* CPHAΪ0                      */
                    0x07 << 8;                                          /* ����ʱ������Ϊ7              */

    LPC_SSP0->CPSR = 0x2;                                               /* ʱ��Ԥ��ƵֵΪ2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP0->DR;                                         /* ��0����FIFO                  */
    }

    NVIC_EnableIRQ(SSP0_IRQn);                                          /* ʹ��SSP0�ж�                 */

    #if LOOPBACK_MODE                                                   /* ��·ģʽ                     */
        LPC_SSP0->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* �ӻ�ģʽ                     */
        if (LPC_SSP0->CR1 & SSPCR1_SSE) {
            LPC_SSP0->CR1 &= ~SSPCR1_SSE;                               /* ����SSP0������               */
        }
        LPC_SSP0->CR1 = SSPCR1_MS;                                      /* ѡ��SSP0���ߴӻ�             */
        LPC_SSP0->CR1 |= SSPCR1_SSE;                                    /* ʹ��SSP0������               */
    #else                                                               /* ����ģʽ                     */
        LPC_SSP0->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* ʹ������жϺͽ��ճ�ʱ�ж�   */
    return;
}

/*********************************************************************************************************
** Function name:       SSP1Init
** Descriptions:        SSP1��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP1Init( uint32_t uiLocation )
{
    uint8_t i = 0;
    uint8_t uiDummy = uiDummy = 0; 
    
    LPC_SC->PCONP |= (0x1<<10);                                         /* ����SSP1����                 */
    
    /* 
     * �趨�����Ƶֵ
     
    uiRegVal = LPC_SC->PCLKSEL;
    if ( uiRegVal < 4 ) {
        LPC_SC->PCLKSEL = 4;
    }*/

    /* 
     * ѡ���ض�λ�ù��ܹܽ�
     */
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P0_7  &= ~0x07;
        LPC_IOCON->P0_7  |=  0x02;                                      /* SSP CLK                      */
        LPC_IOCON->P0_14 &= ~0x07;    
        LPC_IOCON->P0_14 |=  0x02;                                      /* SSP SSEL                     */
        LPC_IOCON->P0_12 &= ~0x07;
        LPC_IOCON->P0_12 |=  0x02;                                      /* SSP MISO                     */
        LPC_IOCON->P0_13 &= ~0x07;    
        LPC_IOCON->P0_13 |=  0x02;                                      /* SSP MOSI                     */
        break;

    case 1:
        LPC_IOCON->P1_19 &= ~0x07;
        LPC_IOCON->P1_19 |=  0x05;                                      /* SSP CLK                      */
        LPC_IOCON->P1_26 &= ~0x07;    
        LPC_IOCON->P1_26 |=  0x05;                                      /* SSP SSEL                     */
        LPC_IOCON->P1_18 &= ~0x07;
        LPC_IOCON->P1_18 |=  0x05;                                      /* SSP MISO                     */
        LPC_IOCON->P1_22 &= ~0x07;    
        LPC_IOCON->P1_22 |=  0x05;                                      /* SSP MOSI                     */
        break;

    case 2:
        LPC_IOCON->P4_20 &= ~0x07;
        LPC_IOCON->P4_20 |=  0x03;                                      /* SSP CLK                      */
        LPC_IOCON->P4_21 &= ~0x07;    
        LPC_IOCON->P4_21 |=  0x03;                                      /* SSP SSEL                     */
        LPC_IOCON->P4_22 &= ~0x07;
        LPC_IOCON->P4_22 |=  0x03;                                      /* SSP MISO                     */
        LPC_IOCON->P4_23 &= ~0x07;    
        LPC_IOCON->P4_23 |=  0x03;                                      /* SSP MOSI                     */
        break;

    case 3:
        LPC_IOCON->P0_7 &= ~0x07;
        LPC_IOCON->P0_7 |=  0x02;                                       /* SSP CLK                      */
        LPC_IOCON->P0_6 &= ~0x07;    
       // LPC_IOCON->P0_6 |=  0x02;                                       /* SSP SSEL                     */
		LPC_GPIO0->DIR |= (0x1<<6);
        LPC_GPIO0->SET |= (0x1<<6);
        LPC_IOCON->P0_8 &= ~0x07;
        LPC_IOCON->P0_8 |=  0x02;                                       /* SSP MISO                     */
        LPC_IOCON->P0_9 &= ~0x07;    
        LPC_IOCON->P0_9 |=  0x02;                                       /* SSP MOSI                     */
        break;
    default :
        break;
    }
    
    /* 
     * ��ʹ�ùܽŵ�CS����ʱ���˹�����ʵ��ʹ�ܿ���
     */
    // #if !USE_CS
        // SSP_SSELSetDirection( 1, uiLocation );
    // #endif

    LPC_SSP1->CR0 = 0x07 << 0 |                                         /* ���ݳ���Ϊ8λ                */
                    0x00 << 4 |                                         /* ֡��ʽΪSPI                  */
                    0x00 << 6 |                                         /* CPOLΪ0                      */
                    0x00 << 7 |                                         /* CPHAΪ0                      */
                    0x02 << 8;                                          /* ����ʱ������Ϊ7              */

    LPC_SSP1->CPSR = 0x04;                                               /* ʱ��Ԥ��ƵֵΪ2       PCL/(CPSR*(SCR+1))       */    
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP1->DR;                                         /* ��0����FIFO                  */
    }
    

    NVIC_EnableIRQ(SSP1_IRQn);                                          /* ʹ��SSP1�ж�                 */

    #if LOOPBACK_MODE                                                   /* ��·ģʽ                     */
        LPC_SSP1->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* �ӻ�ģʽ                     */
        if ( LPC_SSP1->CR1 & SSPCR1_SSE ) {
            LPC_SSP1->CR1 &= ~SSPCR1_SSE;                               /* ����SSP1������               */
        }
        LPC_SSP1->CR1 = SSPCR1_MS;                                      /* ѡ��SSP1���ߴӻ�             */
        LPC_SSP1->CR1 |= SSPCR1_SSE;                                    /* ʹ��SSP1������               */
    #else                                                               /* ����ģʽ                     */
        LPC_SSP1->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP1->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* ʹ������жϺͽ��ճ�ʱ�ж�   */
    return;
}

/*********************************************************************************************************
** Function name:       SSP2Init
** Descriptions:        SSP2��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSP2Init( uint32_t uiLocation )
{
    uint8_t i = 0;// uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0;
    
    LPC_SC->PCONP |= (0x1<<20);                                         /* ����SSP2����                 */
    
    /* 
     * �趨�����Ƶֵ
     */
    
    /* 
     * ѡ���ض�λ�ù��ܹܽ�
     */
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P5_2 &= ~0x07;
        LPC_IOCON->P5_2 |=  0x02;                                       /* SSP CLK                      */
        LPC_IOCON->P5_3 &= ~0x07;    
        LPC_IOCON->P5_3 |=  0x02;                                       /* SSP SSEL                     */
        LPC_IOCON->P5_1 &= ~0x07;
        LPC_IOCON->P5_1 |=  0x02;                                       /* SSP MISO                     */
        LPC_IOCON->P5_0 &= ~0x07;    
        LPC_IOCON->P5_0 |=  0x02;                                       /* SSP MOSI                     */
        break;

    case 1:
        LPC_IOCON->P1_0 &= ~0x07;
        LPC_IOCON->P1_0 |=  0x04;                                       /* SSP CLK                      */
        LPC_IOCON->P1_8 &= ~0x07;    
        //LPC_IOCON->P1_8 |=  0x04;                                       /* SSP SSEL                     */
		LPC_GPIO1->DIR |= (1ul<<8);
        LPC_GPIO1->SET |= (1ul<<8);
        LPC_IOCON->P1_4 &= ~0x07;
        LPC_IOCON->P1_4 |=  0x04;                                       /* SSP MISO                     */
        LPC_IOCON->P1_1 &= ~0x07;    
        LPC_IOCON->P1_1 |=  0x04;                                       /* SSP MOSI                     */
        break;
        
    default:
        break;
    }

    /* 
     * ��ʹ�ùܽŵ�CS����ʱ���˹�����ʵ��ʹ�ܿ���
     */
    // #if !USE_CS
        // SSP_SSELSetDirection( 2, uiLocation );
    // #endif

    LPC_SSP2->CR0 = 0x07 << 0 |                                         /* ���ݳ���Ϊ8λ                */
                    0x00 << 4 |                                         /* ֡��ʽΪSPI                  */
                    0x00 << 6 |                                         /* CPOLΪ0                      */
                    0x00 << 7 |                                         /* CPHAΪ0                      */
                    0x02 << 8;                                          /* ����ʱ������Ϊ7              */

    LPC_SSP2->CPSR = 0x02;                                               /* ʱ��Ԥ��ƵֵΪ2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP2->DR;                                         /* ��0����FIFO                  */
    }

    NVIC_EnableIRQ(SSP2_IRQn);                                          /* ʹ��SSP2�ж�                 */

    #if LOOPBACK_MODE                                                   /* ��·ģʽ                     */
        LPC_SSP2->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* �ӻ�ģʽ                     */
        if ( LPC_SSP2->CR1 & SSPCR1_SSE ) {
            LPC_SSP2->CR1 &= ~SSPCR1_SSE;                               /* ����SSP1������               */
        }
        LPC_SSP2->CR1 = SSPCR1_MS;                                      /* ѡ��SSP2���ߴӻ�             */
        LPC_SSP2->CR1 |= SSPCR1_SSE;                                    /* ʹ��SSP2������               */
    #else                                                               /* ����ģʽ                     */
        LPC_SSP2->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP2->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* ʹ������жϺͽ��ճ�ʱ�ж�   */
    return;
}

/*********************************************************************************************************
** Function name:       SSPSend
** Descriptions:        ͨ��SSP�ڷ�������
** input parameters:    uiPortNum    SSP�˿ں�
**                      pBuf         ��������ָ��
**                      uiLength     �������ݳ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSPSend( uint32_t uiPortNum, unsigned char *pBuf, uint32_t uiLength )
{
    uint32_t i = 0;
    uint8_t uiDummy = uiDummy = 0;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            while ( (LPC_SSP0->SR &                                     /* SSP0��TX FIFOδ����SSP0����  */
                    (SSPSR_TNF | SSPSR_BSY))!=
                     SSPSR_TNF );

            LPC_SSP0->DR = *pBuf;                                       /* ��������                     */
            pBuf++;                                                     /* ָ����һ����������           */

            #if !LOOPBACK_MODE                                          /* ������ӻ�ģʽ               */
                while ( (LPC_SSP0->SR &                                 /* SSP0��RX FIFOδ����SSP0����  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP0->DR;                                 /* ���TX FIFO                  */
            #else                                                       /* ��·ģʽ                     */
                while ( LPC_SSP0->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 1 ) {
            while ( (LPC_SSP1->SR &                                     /* SSP0��TX FIFOδ����SSP0����  */
                    (SSPSR_TNF | SSPSR_BSY)) != 
                	SSPSR_TNF );

            LPC_SSP1->DR = *pBuf;                                       /* ��������                     */
            pBuf++;                                                     /* ָ����һ����������           */

            #if !LOOPBACK_MODE                                          /* ������ӻ�ģʽ               */
                while ( (LPC_SSP1->SR &                                 /* SSP0��RX FIFOδ����SSP0����  */
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );

                uiDummy = LPC_SSP1->DR;                                 /* ���TX FIFO                  */
            #else                                                       /* ��·ģʽ                     */
                while ( LPC_SSP1->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 2 ) {
            while ( (LPC_SSP2->SR &                                     /* SSP0��TX FIFOδ����SSP0����  */
                    (SSPSR_TNF|SSPSR_BSY)) !=
                     SSPSR_TNF );
            LPC_SSP2->DR = *pBuf;                                       /* ��������                     */
            pBuf++;                                                     /* ָ����һ����������           */
            #if !LOOPBACK_MODE                                          /* ������ӻ�ģʽ               */
                while ( (LPC_SSP2->SR &                                 /* SSP0��RX FIFOδ����SSP0����  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP2->DR;                                 /* ���TX FIFO                  */
            #else
                while ( LPC_SSP2->SR & SSPSR_BSY );
            #endif
        }
    }
    return; 
}

/*********************************************************************************************************
** Function name:       SSPReceive
** Descriptions:        ͨ��SSP��������
** input parameters:    uiPortNum    SSP�˿ں�
**                      pBuf         ��������ָ��
**                      uiLength     �������ݳ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SSPReceive( uint32_t uiPortNum, uint8_t *pBuf, uint32_t uiLength )
{
    uint32_t i;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* �ӻ�ģʽ                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #else                                                       /* ����ģʽ                     */
                LPC_SSP0->DR = 0xFF;
                while ( (LPC_SSP0->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* ��·ģʽ                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #endif
            *pBuf++ = LPC_SSP0->DR;
        }
        else if ( uiPortNum == 1 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* �ӻ�ģʽ                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #else                                                       /* ����ģʽ                     */
                LPC_SSP1->DR = 0xFF;
                while ( (LPC_SSP1->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* ��·ģʽ                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #endif
            *pBuf++ = LPC_SSP1->DR;
        }
        else if ( uiPortNum == 2 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* �ӻ�ģʽ                     */
                while (!(LPC_SSP2->SR & SSPSR_RNE));
            #else                                                       /* ����ģʽ                     */
                LPC_SSP2->DR = 0xFF;
                while ( (LPC_SSP2->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );
            #endif
            #else                                                       /* ��·ģʽ                     */
                while (!(LPC_SSP2->SR & SSPSR_RNE));
            #endif
            *pBuf++ = LPC_SSP2->DR;
        }
    }
    return;
}
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/

