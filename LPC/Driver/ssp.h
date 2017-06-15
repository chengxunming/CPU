/****************************************Copyright (c)****************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           ssp.h 
** Last modified Date:  2011-7-6
** Last Version:        V1.0
** Descriptions:        SSP初始化
**
**--------------------------------------------------------------------------------------------------------
** Created by:          ZhenJinhong
** Created date:        2011-7-6
** Version:             V1.0
** Descriptions:        SSP初始化
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
** Rechecked by:

*********************************************************************************************************/
#ifndef __SSP_H__
#define __SSP_H__
/*
** 1 is loopback, 0 is normal operation. 
*/
#define LOOPBACK_MODE    0
/*
** 1 is SLAVE mode, 0 is master mode
*/
#define SSP_SLAVE        0
/* 1 is TX or RX only depending on SSP_SLAVE flag, 0 is either loopback mode or communicate
** with a serial EEPROM
*/
#define TX_RX_ONLY       0

/* 
** if USE_CS is zero, set SSEL as GPIO that you have total control of the sequence 
*/
#define USE_CS            0

/*
** SPI read and write buffer size 
*/
#define SSP_BUFSIZE        16
#define FIFOSIZE        8

#define DELAY_COUNT        10
#define MAX_TIMEOUT        0xFF

/*
** Port0.2 is the SSP select pin 
*/
#define SSP0_SEL        (1 << 2)
    
/*
** SSP Status register 
*/
#define SSPSR_TFE        (1 << 0)
#define SSPSR_TNF        (1 << 1) 
#define SSPSR_RNE        (1 << 2)
#define SSPSR_RFF        (1 << 3) 
#define SSPSR_BSY        (1 << 4)

/*
** SSP CR0 register 
*/
#define SSPCR0_DSS        (1 << 0)
#define SSPCR0_FRF        (1 << 4)
#define SSPCR0_SPO        (1 << 6)
#define SSPCR0_SPH        (1 << 7)
#define SSPCR0_SCR        (1 << 8)

/*
** SSP CR1 register 
*/
#define SSPCR1_LBM        (1 << 0)
#define SSPCR1_SSE        (1 << 1)
#define SSPCR1_MS        (1 << 2)
#define SSPCR1_SOD        (1 << 3)

/*
** SSP Interrupt Mask Set/Clear register 
*/
#define SSPIMSC_RORIM   (1 << 0)
#define SSPIMSC_RTIM    (1 << 1)
#define SSPIMSC_RXIM    (1 << 2)
#define SSPIMSC_TXIM    (1 << 3)

/*
** SSP0 Interrupt Status register 
*/
#define SSPRIS_RORRIS    (1 << 0)
#define SSPRIS_RTRIS    (1 << 1)
#define SSPRIS_RXRIS    (1 << 2)
#define SSPRIS_TXRIS    (1 << 3)

/*
** SSP0 Masked Interrupt register 
*/
#define SSPMIS_RORMIS    (1 << 0)
#define SSPMIS_RTMIS    (1 << 1)
#define SSPMIS_RXMIS    (1 << 2)
#define SSPMIS_TXMIS    (1 << 3)

/*
** SSP0 Interrupt clear register 
*/
#define SSPICR_RORIC    (1 << 0)
#define SSPICR_RTIC        (1 << 1)

/*
** ATMEL SEEPROM command set 
*/
#define WREN        0x06
#define WRDI        0x04
#define RDSR        0x05
#define WRSR        0x01
#define READ        0x03
#define WRITE       0x02

/*
** RDSR status bit definition 
*/
#define RDSR_RDY    0x01
#define RDSR_WEN    0x02

/* If RX_INTERRUPT is enabled, the SSP RX will be handled in the ISR
** SSPReceive() will not be needed. 
*/

/*********************************************************************************************************
** Function name:       SSP0_IRQHandler
** Descriptions:        SSP port is used for SPI communication
**                      SSP interrupt handler
**                      The algorithm is, if RXFIFO is at least half full, 
**                      start receive until it's empty; if TXFIFO is at least
**                      half empty, start transmit until it's full.
**                      This will maximize the use of both FIFOs and performance.
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP0_IRQHandler (void);

/*********************************************************************************************************
** Function name:       SS1P_IRQHandler
** Descriptions:        SSP port is used for SPI communication
**                      SSP interrupt handler
**                      The algorithm is, if RXFIFO is at least half full, 
**                      start receive until it's empty; if TXFIFO is at least
**                      half empty, start transmit until it's full.
**                      This will maximize the use of both FIFOs and performance.
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP1_IRQHandler (void);

/*********************************************************************************************************
** Function name:       SSP2_IRQHandler
** Descriptions:        SSP port is used for SPI communication
**                      SSP interrupt handler
**                      The algorithm is, if RXFIFO is at least half full, 
**                      start receive until it's empty; if TXFIFO is at least
**                      half empty, start transmit until it's full.
**                      This will maximize the use of both FIFOs and performance.
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP2_IRQHandler (void);

/*********************************************************************************************************
** Function name:       SSP0_SSELSetDirection
** Descriptions:        SSP0 CS manual set, direction is output, default is high
** input parameters:    portnum    SSP端口号
**                      location   SSEL管脚位置
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP_SSELSetDirection( uint32_t portnum, uint32_t location );

/*********************************************************************************************************
** Function name:       SSP0_SSELToggle
** Descriptions:        SSP0 CS manual set
** input parameters:    portnum    SSP端口号
**                      location   SSEL管脚位置
**                      toggle     SSEL管脚电平控制，0为低电平，1为高电平
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP_SSELToggle( uint32_t portnum, uint32_t location, uint32_t toggle );

/*********************************************************************************************************
** Function name:       SSP0Init
** Descriptions:        SSP port0 initialization routine
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP0Init( uint32_t location );

/*********************************************************************************************************
** Function name:       SSP1Init
** Descriptions:        SSP port1 initialization routine
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP1Init( uint32_t location );

/*********************************************************************************************************
** Function name:       SSP2Init
** Descriptions:        SSP port2 initialization routine
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSP2Init( uint32_t location );

/*********************************************************************************************************
** Function name:       SSPSend
** Descriptions:        Send a block of data to the SSP port, the first parameter is the buffer pointer,
**                      the 2nd parameter is the block length.
** input parameters:    portnum    SSP端口号
**                      buf        发送数据指针
**                      Length     发送数据长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSPSend( uint32_t portnum, unsigned char *Buf, uint32_t Length );

/*********************************************************************************************************
** Function name:       SSPReceive
** Descriptions:        the module will receive a block of data from the SSP, the 2nd parameter is the 
**                      block length.
** input parameters:    portnum    SSP端口号
**                      buf        接收数据指针
**                      Length     接收数据长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
extern void SSPReceive( uint32_t portnum, uint8_t *buf, uint32_t Length );

#endif                                                                   /* __SSP_H__                   */
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/

