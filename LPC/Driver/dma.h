/****************************************Copyright (c)****************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           dma.h
** Last modified Date:  2011-7-7
** Last Version:        V1.0
** Descriptions:        DMA初始化头文件
**
**--------------------------------------------------------------------------------------------------------
** Created by:          ZhenJinhong
** Created date:        2011-7-7
** Version:             V1.0
** Descriptions:        DMA初始化头文件
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
** Rechecked by:
*********************************************************************************************************/
#ifndef __DMA_H 
#define __DMA_H

/* 
 * Second half of the second RAM is used for GPDMA operation
 */

#define DMA_UART0_TX        10
#define DMA_UART0_RX        11
/* 
 * starting addr of DATA register in UART0
 */
//#define UART0_DMA_TX_SRC    0x10008800
#define UART0_DMA_TX_DST    &LPC_UART0->THR
#define UART0_DMA_RX_SRC    LPC_UART0_BASE
//#define UART0_DMA_RX_DST    0x10008900

//#define UART1_DMA_TX_SRC    0x20080120
#define UART1_DMA_TX_DST    LPC_UART1_BASE
#define UART1_DMA_RX_SRC    LPC_UART1_BASE
//#define UART1_DMA_RX_DST    0x20080124


//#define UART4_DMA_TX_SRC    0x20080180
#define UART4_DMA_TX_DST    LPC_UART4_BASE
#define UART4_DMA_RX_SRC    LPC_UART4_BASE
//#define UART4_DMA_RX_DST    0x20080184



//#define UART0_DMA_SIZE    750                                          /* DMA0 transfer size            */
//#define UART1_DMA_SIZE    256                                          /* DMA0 transfer size            */
//#define UART4_DMA_SIZE    256                                          /* DMA0 transfer size            */

/* 
 * DMA mode
 */
#define M2M                0x00
#define M2P                0x01
#define P2M                0x02
#define P2P                0x03


extern  uint16_t UART0DMATXSIZE,UART1DMATXSIZE,UART2DMATXSIZE,UART3DMATXSIZE,UART4DMATXSIZE;
extern  uint16_t UART0DMARXSIZE,UART1DMARXSIZE,UART2DMARXSIZE,UART3DMARXSIZE,UART4DMARXSIZE;
extern volatile uint32_t UART2DMADone,UART3DMADone;
extern volatile bool DatasendDone;

/*********************************************************************************************************
** 函数名称：DMA_IRQHandler
** 函数描述：DMA中断处理
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
extern void DMA_IRQHandler( void );

/*********************************************************************************************************
** 函数名称：DMA_Init
** 函数描述：DMA初始化
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
extern void DMA_Init( void );

/*********************************************************************************************************
** 函数名称：DMAChannel_Init
** 函数描述：初始化特定的DMA通道
** 输入参数：uiChannelNum    DMA通道号
**           uiDMAMode       DMA通道模式
** 返回值  ：无
*********************************************************************************************************/
extern uint32_t DMAChannel_Init( uint32_t ChannelNum, uint32_t DMAMode );

#endif                                                                  /* end __DMA_H                  */
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
