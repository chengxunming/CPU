/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC177x_8x Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "../User_code/global.h"


/*********************************************************************************************************
  宏定义
*********************************************************************************************************/

#define ST_CTRL_ENABLE        ((uint32_t)(1<<0))
#define ST_CTRL_TICKINT       ((uint32_t)(1<<1))
#define ST_CTRL_CLKSOURCE     ((uint32_t)(1<<2))
#define ST_CTRL_COUNTFLAG     ((uint32_t)(1<<16))
#define ENABLE                ((uint32_t)(1))


/*********************************************************************************************************
** Function name:       SysTick_Handler
** Descriptions:        系统节拍定时器中断服务函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SysTick_Handler (void)
{
   delaytime--;
   if(delaytime==0)
   {
	   delayflg=0;
	   SYSTICK_Cmd(DISABLE);
   }   
}

/**********************************************************************
 * @brief         Initial System Tick with using internal CPU clock source
 * @param[in]     time    time interval(ms)
 * @return        None
 * Note:          time interval parameter should be less than:
 *                1/cclk * (2^24) * 1000 (ms)
 **********************************************************************/
void SYSTICK_InternalInit(uint32_t time)
{
    float maxtime;

    /* With internal CPU clock frequency for LPC178X is 'SystemCoreClock'
     * And limit 24 bit for RELOAD value
     * So the maximum time can be set:
     * 1/SystemCoreClock * (2^24) * 1000 (ms)
     */

    maxtime = (1<<24)/(SystemCoreClock / 1000) ;

    if(time > maxtime) {
        while(1);                                                       /* Error loop                     */
    } else {
        SysTick->CTRL |= ST_CTRL_CLKSOURCE;
        /* Set RELOAD value
         * RELOAD = (SystemCoreClock/1000) * time - 1
         * with time base is millisecond
         */
        SysTick->RELOAD = (SystemCoreClock/1000)*time - 1;
    }
}

/***********************************************************************
 * @brief        Initial System Tick with using external clock source
 * @param[in]    freq    external clock frequency(Hz)
 * @param[in]    time    time interval(ms)
 * @return       None
 **********************************************************************/
void SYSTICK_ExternalInit(uint32_t freq, uint32_t time)
{
    float maxtime;

    /* With external clock frequency for LPC178X is 'freq'
     * And limit 24 bit for RELOAD value
     * So the maximum time can be set:
     * 1/freq * (2^24) * 1000 (ms)
     */
    maxtime = (1<<24)/(freq / 1000) ;
    if (time>maxtime) {
        while(1);
    } else {
        SysTick->CTRL &= ~ ST_CTRL_CLKSOURCE;
        /* Set RELOAD value
         * RELOAD = (freq/1000) * time - 1
         * with time base is millisecond
         */
        maxtime = (freq/1000)*time - 1;
        SysTick->RELOAD = (freq/1000)*time - 1;
    }
}

/***********************************************************************
 * @brief            Enable/disable System Tick counter
 * @param[in]        NewState    System Tick counter status, should be:
 *                 - ENABLE
 *                 - DISABLE
 * @return           None
 **********************************************************************/
void SYSTICK_Cmd(uint32_t NewState)
{
    if(NewState == ENABLE) {
        SysTick->CTRL |=  ST_CTRL_ENABLE;                       /* Enable System Tick counter    */
    } else {
        SysTick->CTRL &= ~ST_CTRL_ENABLE;                       /* Disable System Tick counter   */
    }        
}

/**********************************************************************
 * @brief           Enable/disable System Tick interrupt
 * @param[in]       NewState    System Tick interrupt status, should be:
 *                - ENABLE
 *                - DISABLE
 * @return          None
 **********************************************************************/
void SYSTICK_IntCmd(uint32_t NewState)
{
    if(NewState == ENABLE) {
        SysTick->CTRL |= ST_CTRL_TICKINT;                       /* Enable System Tick counter    */
    } else {
        SysTick->CTRL &= ~ST_CTRL_TICKINT;                      /* Disable System Tick counter   */
    }

}

/***********************************************************************
 * @brief          Get current value of System Tick counter
 * @param[in]      None
 * @return         current value of System Tick counter
 **********************************************************************/
uint32_t SYSTICK_GetCurrentValue(void)
{
    return (SysTick->CURR);
}

/**********************************************************************
 * @brief          Clear Counter flag
 * @param[in]      None
 * @return         None
 **********************************************************************/
void SYSTICK_ClearCounterFlag(void)
{
    SysTick->CTRL &= ~ST_CTRL_COUNTFLAG;
}
/*********************************************************************************************************
** Function name:       SysTickInit
** Descriptions:        系统节拍定时器初始化(定时最大699ms)
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SysTickInit (uint32_t time)
{
    SYSTICK_InternalInit(time);                        /* Initialize System Tick with time interval      */
    SYSTICK_IntCmd(ENABLE);                            /* Enable System Tick interrupt                   */
    SYSTICK_Cmd(ENABLE);                               /* Enable System Tick Counter                     */

}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
