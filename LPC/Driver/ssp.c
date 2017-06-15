/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           ssp.c
** Last modified Date:  2014-3-6
** Last Version:        V1.0
** Descriptions:        SSP初始化
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Qinqin
** Created date:        2011-7-6
** Version:             V1.0
** Descriptions:        SSP初始化
**
*********************************************************************************************************/
#include "LPC177x_8x.h"
#include "ssp.h"

/* 
 * 中断状态标志
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
** Descriptions:        SSP0中断函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP0_IRQHandler(void)
{
    uint32_t regValue;

    regValue = LPC_SSP0->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt0OverRunStat++;
        LPC_SSP0->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt0RxTimeoutStat++;
        LPC_SSP0->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt0RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP1_IRQHandler
** Descriptions:        SSP1中断函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP1_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP1->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt1OverRunStat++;
        LPC_SSP1->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt1RxTimeoutStat++;
        LPC_SSP1->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt1RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP2_IRQHandler
** Descriptions:        SSP2中断函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP2_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP2->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt2OverRunStat++;
        LPC_SSP2->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt2RxTimeoutStat++;
        LPC_SSP2->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt2RxStat++;
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP_SSELSetDirection
** Descriptions:        选择SSP的CS管脚，实现对其电平可控
** input parameters:    uiPortNum      SSP端口号
**                      uiLocation   CS管脚位置
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP_SSELSetDirection(uint32_t uiPortNum, uint32_t uiLocation)
{
    if (uiPortNum == 0) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P2_23 &= ~0x07;                                  /* P2.23为SSP0的CS管脚          */
            LPC_GPIO2->DIR |= (0x1<<23);
            LPC_GPIO2->SET |= (0x1<<23);
            break;
        case 1:
            LPC_IOCON->P1_21 &= ~0x07;                                  /* P1.21为SSP0的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<21);
            LPC_GPIO1->SET |= (0x1<<21);
            break;
        case 2:
            LPC_IOCON->P1_28 &= ~0x07;                                  /* P1.28为SSP0的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<28);
            LPC_GPIO1->SET |= (0x1<<28);
            break;
        case 3:
            LPC_IOCON->P0_16 &= ~0x07;                                  /* P0.16为SSP0的CS管脚          */
            LPC_GPIO0->DIR |= (0x1<<16);
            LPC_GPIO0->SET |= (0x1<<16);
            break;
        }
    }
    else if (uiPortNum == 1) {
        switch (uiLocation) {
        case 0:
            LPC_IOCON->P0_14 &= ~0x07;                                  /* P0.14为SSP1的CS管脚          */
            LPC_GPIO0->DIR |= (0x1<<14);
            LPC_GPIO0->SET |= (0x1<<14);
            break;
        case 1:
            LPC_IOCON->P1_26 &= ~0x07;                                  /* P1.26为SSP1的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<26);
            LPC_GPIO1->SET |= (0x1<<26);
            break;
        case 2:
            LPC_IOCON->P4_21 &= ~0x07;                                  /* P4.21为SSP1的CS管脚          */
            LPC_GPIO4->DIR |= (0x1<<21);
            LPC_GPIO4->SET |= (0x1<<21);
            break;
        case 3:
            LPC_IOCON->P0_6 &= ~0x07;                                  /* P0.6为SSP1的CS管脚           */
            LPC_GPIO0->DIR |= (0x1<<6);
            LPC_GPIO0->SET |= (0x1<<6);
            break;
       }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P5_3 &= ~0x07;                                  /* P5.3为SSP2的CS管脚           */
            LPC_GPIO5->DIR |= (0x1<<3);
            LPC_GPIO5->SET |= (0x1<<3);
            break;
        case 1:
            LPC_IOCON->P1_8 &= ~0x07;                                  /* P1.8为SSP2的CS管脚           */
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
** Descriptions:        控制SSP的CS管脚电平
** input parameters:    uiPortNum      SSP端口号
**                      uiLocation   SSEL管脚位置
**                      toggle       SSEL管脚电平控制，0为低电平，1为高电平
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP_SSELToggle( uint32_t uiPortNum, uint32_t uiLocation, uint32_t toggle )
{
    if ( uiPortNum == 0 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO2->CLR |= (0x1<<23);                            /* P2.23为CS，且为低电平        */
            else
                LPC_GPIO2->SET |= (0x1<<23);                            /* P2.23为CS，且为高电平        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<21);                            /* P1.21为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<21);                            /* P1.21为CS，且为高电平        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<28);                            /* P1.28为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<28);                            /* P1.28为CS，且为高电平        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<16);                            /* P0.16为CS，且为低电平        */
            else
                LPC_GPIO0->SET |= (0x1<<16);                            /* P0.16为CS，且为高电平        */
            break;
        }
    }
    else if ( uiPortNum == 1 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<14);                            /* P0.14为CS，且为低电平        */
            else
                LPC_GPIO0->SET |= (0x1<<14);                            /* P0.17为CS，且为高电平        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<26);                            /* P1.26为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<26);                            /* P1.26为CS，且为高电平        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO4->CLR |= (0x1<<21);                            /* P4.21为CS，且为低电平        */
            else
                LPC_GPIO4->SET |= (0x1<<21);                            /* P4.21为CS，且为高电平        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<6);                            /* P0.6为CS，且为低电平         */
            else
                LPC_GPIO0->SET |= (0x1<<6);                            /* P0.6为CS，且为高电平         */
            break;
        }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO5->CLR |= (0x1<<3);                            /* P5.3为CS，且为低电平         */
            else
                LPC_GPIO5->SET |= (0x1<<3);                            /* P5.3为CS，且为高电平         */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<8);                            /* P1.8为CS，且为低电平         */
            else
                LPC_GPIO1->SET |= (0x1<<8);                            /* P1.8为CS，且为高电平         */
            break;
        default:
            break;
        }
    }
    return;
}

/*********************************************************************************************************
** Function name:       SSP0Init
** Descriptions:        SSP0初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP0Init( uint32_t uiLocation )
{
    uint8_t i = 0, uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0;

    LPC_SC->PCONP |= (0x1 << 21);                                       /* 开启SSP0外设                 */

    /* 
     * 设定外设分频值
     */
    uiRegVal = LPC_SC->PCLKSEL;
    if (uiRegVal < 4) {
        LPC_SC->PCLKSEL = 4;
    }

    /* 
     * 选定特定位置功能管脚
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
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    #if !USE_CS
        SSP_SSELSetDirection( 0, uiLocation );
    #endif

    LPC_SSP0->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x00 << 6 |                                         /* CPOL为0                      */
                    0x00 << 7 |                                         /* CPHA为0                      */
                    0x07 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP0->CPSR = 0x2;                                               /* 时钟预分频值为2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP0->DR;                                         /* 清0接收FIFO                  */
    }

    NVIC_EnableIRQ(SSP0_IRQn);                                          /* 使能SSP0中断                 */

    #if LOOPBACK_MODE                                                   /* 回路模式                     */
        LPC_SSP0->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* 从机模式                     */
        if (LPC_SSP0->CR1 & SSPCR1_SSE) {
            LPC_SSP0->CR1 &= ~SSPCR1_SSE;                               /* 禁能SSP0控制器               */
        }
        LPC_SSP0->CR1 = SSPCR1_MS;                                      /* 选择SSP0总线从机             */
        LPC_SSP0->CR1 |= SSPCR1_SSE;                                    /* 使能SSP0控制器               */
    #else                                                               /* 主机模式                     */
        LPC_SSP0->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}

/*********************************************************************************************************
** Function name:       SSP1Init
** Descriptions:        SSP1初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP1Init( uint32_t uiLocation )
{
    uint8_t i = 0;
    uint8_t uiDummy = uiDummy = 0; 
    
    LPC_SC->PCONP |= (0x1<<10);                                         /* 开启SSP1外设                 */
    
    /* 
     * 设定外设分频值
     
    uiRegVal = LPC_SC->PCLKSEL;
    if ( uiRegVal < 4 ) {
        LPC_SC->PCLKSEL = 4;
    }*/

    /* 
     * 选定特定位置功能管脚
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
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    // #if !USE_CS
        // SSP_SSELSetDirection( 1, uiLocation );
    // #endif

    LPC_SSP1->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x00 << 6 |                                         /* CPOL为0                      */
                    0x00 << 7 |                                         /* CPHA为0                      */
                    0x02 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP1->CPSR = 0x04;                                               /* 时钟预分频值为2       PCL/(CPSR*(SCR+1))       */    
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP1->DR;                                         /* 清0接收FIFO                  */
    }
    

    NVIC_EnableIRQ(SSP1_IRQn);                                          /* 使能SSP1中断                 */

    #if LOOPBACK_MODE                                                   /* 回路模式                     */
        LPC_SSP1->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* 从机模式                     */
        if ( LPC_SSP1->CR1 & SSPCR1_SSE ) {
            LPC_SSP1->CR1 &= ~SSPCR1_SSE;                               /* 禁能SSP1控制器               */
        }
        LPC_SSP1->CR1 = SSPCR1_MS;                                      /* 选择SSP1总线从机             */
        LPC_SSP1->CR1 |= SSPCR1_SSE;                                    /* 使能SSP1控制器               */
    #else                                                               /* 主机模式                     */
        LPC_SSP1->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP1->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}

/*********************************************************************************************************
** Function name:       SSP2Init
** Descriptions:        SSP2初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSP2Init( uint32_t uiLocation )
{
    uint8_t i = 0;// uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0;
    
    LPC_SC->PCONP |= (0x1<<20);                                         /* 开启SSP2外设                 */
    
    /* 
     * 设定外设分频值
     */
    
    /* 
     * 选定特定位置功能管脚
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
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    // #if !USE_CS
        // SSP_SSELSetDirection( 2, uiLocation );
    // #endif

    LPC_SSP2->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x00 << 6 |                                         /* CPOL为0                      */
                    0x00 << 7 |                                         /* CPHA为0                      */
                    0x02 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP2->CPSR = 0x02;                                               /* 时钟预分频值为2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP2->DR;                                         /* 清0接收FIFO                  */
    }

    NVIC_EnableIRQ(SSP2_IRQn);                                          /* 使能SSP2中断                 */

    #if LOOPBACK_MODE                                                   /* 回路模式                     */
        LPC_SSP2->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE                                                     /* 从机模式                     */
        if ( LPC_SSP2->CR1 & SSPCR1_SSE ) {
            LPC_SSP2->CR1 &= ~SSPCR1_SSE;                               /* 禁能SSP1控制器               */
        }
        LPC_SSP2->CR1 = SSPCR1_MS;                                      /* 选择SSP2总线从机             */
        LPC_SSP2->CR1 |= SSPCR1_SSE;                                    /* 使能SSP2控制器               */
    #else                                                               /* 主机模式                     */
        LPC_SSP2->CR1 = SSPCR1_SSE;
    #endif

    LPC_SSP2->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}

/*********************************************************************************************************
** Function name:       SSPSend
** Descriptions:        通过SSP口发送数据
** input parameters:    uiPortNum    SSP端口号
**                      pBuf         发送数据指针
**                      uiLength     发送数据长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSPSend( uint32_t uiPortNum, unsigned char *pBuf, uint32_t uiLength )
{
    uint32_t i = 0;
    uint8_t uiDummy = uiDummy = 0;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            while ( (LPC_SSP0->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF | SSPSR_BSY))!=
                     SSPSR_TNF );

            LPC_SSP0->DR = *pBuf;                                       /* 发送数据                     */
            pBuf++;                                                     /* 指向下一个发送数据           */

            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP0->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP0->DR;                                 /* 清空TX FIFO                  */
            #else                                                       /* 回路模式                     */
                while ( LPC_SSP0->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 1 ) {
            while ( (LPC_SSP1->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF | SSPSR_BSY)) != 
                	SSPSR_TNF );

            LPC_SSP1->DR = *pBuf;                                       /* 发送数据                     */
            pBuf++;                                                     /* 指向下一个发送数据           */

            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP1->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );

                uiDummy = LPC_SSP1->DR;                                 /* 清空TX FIFO                  */
            #else                                                       /* 回路模式                     */
                while ( LPC_SSP1->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 2 ) {
            while ( (LPC_SSP2->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF|SSPSR_BSY)) !=
                     SSPSR_TNF );
            LPC_SSP2->DR = *pBuf;                                       /* 发送数据                     */
            pBuf++;                                                     /* 指向下一个发送数据           */
            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP2->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP2->DR;                                 /* 清空TX FIFO                  */
            #else
                while ( LPC_SSP2->SR & SSPSR_BSY );
            #endif
        }
    }
    return; 
}

/*********************************************************************************************************
** Function name:       SSPReceive
** Descriptions:        通过SSP接收数据
** input parameters:    uiPortNum    SSP端口号
**                      pBuf         接收数据指针
**                      uiLength     接收数据长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SSPReceive( uint32_t uiPortNum, uint8_t *pBuf, uint32_t uiLength )
{
    uint32_t i;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* 从机模式                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #else                                                       /* 主机模式                     */
                LPC_SSP0->DR = 0xFF;
                while ( (LPC_SSP0->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #endif
            *pBuf++ = LPC_SSP0->DR;
        }
        else if ( uiPortNum == 1 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* 从机模式                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #else                                                       /* 主机模式                     */
                LPC_SSP1->DR = 0xFF;
                while ( (LPC_SSP1->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #endif
            *pBuf++ = LPC_SSP1->DR;
        }
        else if ( uiPortNum == 2 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE                                               /* 从机模式                     */
                while (!(LPC_SSP2->SR & SSPSR_RNE));
            #else                                                       /* 主机模式                     */
                LPC_SSP2->DR = 0xFF;
                while ( (LPC_SSP2->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
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

