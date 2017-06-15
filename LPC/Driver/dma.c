/****************************************Copyright (c)****************************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           dma.c
** Last modified Date:  2011-7-6
** Last Version:        V1.0
** Descriptions:        DMA初始化
**
*********************************************************************************************************/

#include "../User_code/global.h"

volatile uint32_t DMATCCount  = 0;                                      /* 终端计数中断发生次数         */
volatile uint32_t DMAErrCount = 0;                                      /* DMA错误中断发生次数          */
volatile uint32_t UART2DMADone = 1,UART3DMADone=1;                                     /* 通道完成DMA操作标志       */


uint16_t UART2DMATXSIZE,UART3DMATXSIZE;//UART2DMATXSIZE,UART3DMATXSIZE,UART4DMATXSIZE;
//uint16_t UART0DMARXSIZE,UART1DMARXSIZE,UART2DMARXSIZE,UART3DMARXSIZE,UART4DMARXSIZE;


/*********************************************************************************************************
** 函数名称：DMA_IRQHandler
** 函数描述：DMA中断处理
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void DMA_IRQHandler(void) 
{
    uint32_t uiRegVal = 0;
    
    uiRegVal = LPC_GPDMA->IntTCStat;
    if (uiRegVal) {                                                     /* 发生终端计数中断             */
        DMATCCount++;
        LPC_GPDMA->IntTCClear = uiRegVal;
        if (uiRegVal & (0x01<<0)) {                                     /* 第0通道有DMA终端计数中断请求 */
            UART2DMADone = 1;
			LPC_GPDMACH0->CConfig  |= 0x00;	
			UART2DMATXSIZE = 0;
			memset(&UART2Buffer,0,sizeof(UART2Buffer));
			
	
        }
        else if (uiRegVal & (0x01<<1)) {                                /* 第1通道有DMA终端计数中断请求 */
            UART3DMADone = 1;
			LPC_GPDMACH1->CConfig  |= 0x00;	
			UART3DMATXSIZE = 0;
			memset(&UART3Buffer,0,sizeof(UART3Buffer));
        }
        else if (uiRegVal & (0x01<<2)) {                                /* 第2通道有DMA终端计数中断请求 */
           __NOP();
        }
        else if (uiRegVal & (0x01<<3)) {                                /* 第3通道有DMA终端计数中断请求 */
            __NOP();
        }
    }
    
    uiRegVal = LPC_GPDMA->IntErrStat;
    if (uiRegVal) {                                                     /* 发生DMA传送错误中断          */
        DMAErrCount++;
        LPC_GPDMA->IntErrClr = uiRegVal;
    }
    return;
}


/*********************************************************************************************************
** 函数名称：DMA_Init
** 函数描述：DMA初始化
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void DMA_Init( void )
{
    LPC_SC->PCONP |= (1 << 29);                                         /* 开启DMA外设                  */
    LPC_SC->DMAREQSEL = 0x0000;                                         /* 选择DMA请求                  */
    LPC_GPDMA->IntTCClear = 0x0F;                                       /* 清除通道0～3的终端计数中断   */
    LPC_GPDMA->IntErrClr  = 0x0F;                                       /* 清除通道0～3的DMA错误中断    */
    LPC_GPDMA->Config = 0x01;                                           /* 使能DMA，字节顺序为小端      */
        
    while (!(LPC_GPDMA->Config & 0x01));
    
    NVIC_EnableIRQ(DMA_IRQn);                                           /* 使能DMA中断请求              */
    return;
}


/*********************************************************************************************************
** 函数名称：DMAChannel_Init
** 函数描述：初始化特定的DMA通道
** 输入参数：uiChannelNum    DMA通道号
**           uiDMAMode       DMA通道模式
** 返回值  ：无
*********************************************************************************************************/
uint32_t DMAChannel_Init(uint32_t uiChannelNum, uint32_t uiDMAMode)
{
    /* 
     * 设置DMA通道0
     */
    if (uiChannelNum == 0) {
        UART2DMADone = 0;                                               /* 清除DMA完成操作标志          */
        LPC_GPDMA->IntTCClear = 0x01 << 0;                              /* 清除通道0终端计数中断标志    */
        LPC_GPDMA->IntErrClr  = 0x01 << 0;                              /* 清除通道0DMA传送错误中断标志 */
        if (uiDMAMode == M2P) {                                         /* DMA模式：存储器到外设        */
            LPC_GPDMACH0->CSrcAddr  = (uint32_t)UART0Buffer;                 /* 通道0源地址                  */
            LPC_GPDMACH0->CDestAddr = LPC_UART0_BASE;                 /* 通道0目的地址                */

            LPC_GPDMACH0->CControl = 128 |         /* 传输大小为UART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0x01 << 26) |                        /* 每次传输后源地址不变         */
                                     (0 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
        }
        else if ( uiDMAMode == P2M ) {                                  /* DMA模式：外设到存储器        */
            LPC_GPDMACH0->CSrcAddr  = LPC_UART0_BASE;                 /* 通道0源地址                  */
            LPC_GPDMACH0->CDestAddr = (uint32_t)UART0Buffer;                 /* 通道0目的地址                */

            LPC_GPDMACH0->CControl = 128 |         /* 传输大小为UART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0 << 26) |                        /* 每次传输后源地址不变         */
                                     (0x01 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
        } else {
            return (FALSE);
        }
    }
    /* 
     * 设置DMA通道1
     */
    else if (uiChannelNum == 1) {
        UART3DMADone = 0;                                               /* 清除DMA完成操作标志          */
        LPC_GPDMA->IntTCClear = 0x01<<1;                                /* 清除通道1终端计数中断标志    */
        LPC_GPDMA->IntErrClr  = 0x01<<1;                                /* 清除通道1DMA传送错误中断标志 */
        if (uiDMAMode == M2P) {                                         /* DMA模式：存储器到外设        */
            LPC_GPDMACH1->CSrcAddr  = (uint32_t)UART1Buffer;                 /* 通道1源地址                  */
            LPC_GPDMACH1->CDestAddr = (uint32_t)&LPC_UART1->THR;                 /* 通道1目的地址                */

            LPC_GPDMACH1->CControl = 128  |         /* 传输大小为SSP_DMA_SIZE       */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0x01 << 26) |                        /* 每次传输后源地址不变         */
                                     (0 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
        }
        else if (uiDMAMode == P2M) {                                    /* DMA模式：外设到存储器        */
            LPC_GPDMACH1->CSrcAddr  = LPC_UART1_BASE;                 /* 通道1源地址                  */
            LPC_GPDMACH1->CDestAddr = (uint32_t)UART0Buffer;                 /* 通道1目的地址                */

            LPC_GPDMACH1->CControl = 128|         /* 传输大小为UART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0 << 26) |                        /* 每次传输后源地址不变         */
                                     (1 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
        } else {
            return (FALSE);
        }
    }
    /* 
     * 设置DMA通道2
     */
    else if (uiChannelNum == 2) {
        UART2DMADone = 0;                                               /* 清除DMA完成操作标志          */
        LPC_GPDMA->IntTCClear = 0x01 << 2;                              /* 清除通道2终端计数中断标志    */
        LPC_GPDMA->IntErrClr  = 0x01 << 2;                              /* 清除通道2DMA传送错误中断标志 */

        if (uiDMAMode == M2P) {                                         /* DMA模式：存储器到外设        */
            LPC_GPDMACH2->CSrcAddr  = (uint32_t) UART0Buffer;                 /* 通道2源地址                  */
            LPC_GPDMACH2->CDestAddr = (uint32_t)UART0_DMA_TX_DST;                 /* 通道2目的地址                */

            LPC_GPDMACH2->CControl = (128 & 0x0FFF) |         /* 传输大小为UART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0 << 26) |                        /* 每次传输后源地址不变         */
                                     (0 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
            }
        else if (uiDMAMode == P2M) {                                    /* DMA模式：外设到存储器        */
            LPC_GPDMACH2->CSrcAddr  = (uint32_t)UART0_DMA_RX_SRC;                 /* 通道2源地址                  */
            LPC_GPDMACH2->CDestAddr = (uint32_t) UART0Buffer;                 /* 通道2目的地址                */

            LPC_GPDMACH2->CControl = (128 & 0x0FFF) |         /* 传输大小为SSP_DMA_SIZE       */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0 << 26) |                        /* 每次传输后源地址不变         */
                                     (0 << 27) |                        /* 每次传输后目标地址不变       */
                                     0x80000000;                        /* 使能终端计数中断             */
       } else {
            return ( FALSE );
       }
    }
    /* 
     * 设置DMA通道3
     */
    else if (uiChannelNum == 3) {
        UART2DMADone = 0;                                               /* 清除DMA完成操作标志          */
        LPC_GPDMA->IntTCClear = 0x01<<3;                                /* 清除通道3终端计数中断标志    */
        LPC_GPDMA->IntErrClr  = 0x01<<3;                                /* 清除通道3DMA传送错误中断标志 */

        if (uiDMAMode == M2P) {                                         /* DMA模式：存储器到外设        */
        LPC_GPDMACH3->CSrcAddr  = (uint32_t) UART0Buffer;                     /* 通道3源地址                  */
        LPC_GPDMACH3->CDestAddr = (uint32_t) UART0_DMA_TX_DST;                     /* 通道3目的地址                */

        LPC_GPDMACH3->CControl = (128 & 0x0FFF) |             /* 传输大小为UART0_DMA_SIZE      */
                                 (0x02 << 12) |                         /* 源突发大小为8位              */
                                 (0x02 << 15) |                         /* 目标突发大小为8位            */
                                 (0x00 << 18) |                         /* 源传输宽度为1字节            */
                                 (0x00 << 21) |                         /* 目标传输宽度为1字节          */
                                 (0 << 26) |                            /* 每次传输后源地址不变         */
                                 (0 << 27) |                            /* 每次传输后目标地址不变       */
                                  0x80000000;                           /* 使能终端计数中断             */
        }
        else if ( uiDMAMode == P2M )                                    /* DMA模式：外设到存储器        */
        {
            LPC_GPDMACH3->CSrcAddr  = UART0_DMA_RX_SRC;                 /* 通道3源地址                  */
            LPC_GPDMACH3->CDestAddr = (uint32_t) UART0Buffer;                 /* 通道3目的地址                */

            LPC_GPDMACH3->CControl = (128 & 0x0FFF) |         /* 传输大小为UART0_DMA_SIZE      */
                                     (0x02 << 12) |                     /* 源突发大小为8位              */
                                     (0x02 << 15) |                     /* 目标突发大小为8位            */
                                     (0x00 << 18) |                     /* 源传输宽度为1字节            */
                                     (0x00 << 21) |                     /* 目标传输宽度为1字节          */
                                     (0 << 26) |                        /* 每次传输后源地址不变         */
                                     (0 << 27) |                        /* 每次传输后目标地址不变       */
                                      0x80000000;                       /* 使能终端计数中断             */
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
