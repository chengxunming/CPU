/*****************************************************************************
 *   wdt.c:  Watchdog C file for NXP LPC177x_8x Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "../User_code/global.h"


/**********************************************************************
* @brief
* @param[in]
*
* @return
 **********************************************************************/
uint8_t WWDT_SetTimeOutRaw(uint32_t timeoutVal)
{
    int8_t retval = 0;

    if(timeoutVal < WWDT_TIMEOUT_MIN) {
        timeoutVal = WWDT_TIMEOUT_MIN;
        retval = -1;
    }
    else if (timeoutVal > WWDT_TIMEOUT_MAX) {
        timeoutVal = WWDT_TIMEOUT_MAX;
        retval = -1;
    }

    LPC_WDT->TC = timeoutVal;

    return retval;
}

/**********************************************************************
* @brief
* @param[in]
*
* @return
 **********************************************************************/
uint8_t WWDT_SetTimeOut(uint32_t timeout)
{
    return WWDT_SetTimeOutRaw(WDT_GET_FROM_USEC(timeout));
}


/*********************************************************************//**
* @brief          Initial for Watchdog function
* @param[in]      TimeOut time out value, should be in range:
*                 2048 .. 134217728
* @return         None
 **********************************************************************/
uint8_t WWDT_Init(uint32_t TimeOut)
{
    return WWDT_SetTimeOut(TimeOut);
}

/**********************************************************************
* @brief         Enable/Disable WWDT activity
* @param[in]     None
* @return        None
 **********************************************************************/
void WWDT_Configure(st_Wdt_Config wdtCfg)
{
    WWDT_SetTimeOut(wdtCfg.wdtTmrConst);

    if(wdtCfg.wdtEnable)  {
        LPC_WDT->MOD |= WWDT_WDMOD_WDEN;
    } else {
        LPC_WDT->MOD &= ~WWDT_WDMOD_WDEN;
    }

    if(wdtCfg.wdtReset) {
        LPC_WDT->MOD |= WWDT_WDMOD_WDRESET;
    } else {
        LPC_WDT->MOD &= ~WWDT_WDMOD_WDRESET;
    }

    if(wdtCfg.wdtProtect) {
        LPC_WDT->MOD |= WWDT_WDMOD_WDPROTECT;
    } else {
        LPC_WDT->MOD &= ~WWDT_WDMOD_WDPROTECT;
    }

    WWDT_SetWarning(wdtCfg.wdtWarningVal);

    WWDT_SetWindow(wdtCfg.wdtWindowVal);
}

/**********************************************************************
* @brief         Enable/Disable WWDT activity
* @param[in]     None
* @return        None
 **********************************************************************/
uint8_t WWDT_Start(uint32_t TimeOut)
{
    uint8_t retval = 0;

    retval = WWDT_SetTimeOut(TimeOut);

    WWDT_Cmd(ENABLE);

    return retval;
}


/*********************************************************************
 * @brief          Update WDT timeout value and feed
 * @param[in]      WarnTime    time to generate watchdog warning interrupt(us)
 *                 should be in range: 2048 .. 134217728
 * @return         None
 *********************************************************************/
void WWDT_SetTimerConstant(uint32_t constVal)
{
    LPC_WDT->TC = constVal;
}


/**********************************************************************
* @brief            Enable/Disable WDT function
* @param[in]        Mode WWDT mode that will be enabled/disabled, should be:
*                 - WWDT_PROTECT_MODE    : protect mode
*                 - WWDT_RESET_MODE    : reset mode
* @param[in]        NewState new state of protection function, should be:
*                 - ENABLE: The watchdog reload value can be changed at any time
*                 - DISABLE: The watchdog reload value can be changed only after
*                            the counter is below the value of WDWARNINT and WDWINDOW
* @return           None
 **********************************************************************/
void WWDT_SetMode(uint8_t mode, FunctionalState NewState)
{
    if (mode == WWDT_PROTECT_MODE )
    {
        if(NewState == ENABLE) {
            LPC_WDT->MOD |= WWDT_WDMOD_WDPROTECT;
        } else {
        
            LPC_WDT->MOD &= ~WWDT_WDMOD_WDPROTECT;
        }
    }
    else if(mode == WWDT_RESET_MODE) {
        if(NewState == ENABLE) {
            LPC_WDT->MOD |= WWDT_WDMOD_WDRESET;
        } else {
            LPC_WDT->MOD &= ~WWDT_WDMOD_WDRESET;
        }
    }
}

/**********************************************************************
* @brief         Enable/Disable WWDT activity
* @param[in]    None
* @return         None
 **********************************************************************/
void WWDT_Enable(FunctionalState NewState)
{
    if(NewState == ENABLE)  {
        LPC_WDT->MOD |= WWDT_WDMOD_WDEN;
    }
    else {
        LPC_WDT->MOD &= ~WWDT_WDMOD_WDEN;
    }
}

/**********************************************************************
* @brief          Enable/Disable WWDT activity
* @param[in]      None
* @return         None
 **********************************************************************/
void WWDT_Cmd(FunctionalState NewState)
{
    if(NewState == ENABLE) {
        LPC_WDT->MOD |= WWDT_WDMOD_WDEN;
        WWDT_Feed();                                /* Load the Feed register to start using WDT */
    } else {
        LPC_WDT->MOD &= ~WWDT_WDMOD_WDEN;
    }
}

/*********************************************************************
 * @brief         Update WDT timeout value and feed
 * @param[in]     warnVal    value will wrie directly to Window register w/o re-calc
 * @return        None
 *********************************************************************/
uint8_t WWDT_SetWarningRaw(uint32_t warnVal)
{
    int8_t retval = 0;

    if(warnVal < WWDT_WARNINT_MIN) {
        warnVal = WWDT_WARNINT_MIN;
        retval = -1;
    }
    else if (warnVal > WWDT_WARNINT_MAX) {
        warnVal = WWDT_WARNINT_MAX;
        retval = -1;
    }

    LPC_WDT->WARNINT = warnVal;

    return retval;
}

/*********************************************************************
 * @brief          Update WDT timeout value and feed
 * @param[in]      WarnTime    time to generate watchdog warning interrupt(us)
 *                 should be in range: 2048 .. 8192
 * @return         None
 *********************************************************************/
uint8_t WWDT_SetWarning(uint32_t WarnTime)
{
    return WWDT_SetWarningRaw(WDT_GET_FROM_USEC(WarnTime));
}


/*********************************************************************
 * @brief         Update WDT timeout value and feed
 * @param[in]     wndVal    value will wrie directly to Window register w/o re-calc
 * @return        None
 *********************************************************************/
uint8_t WWDT_SetWindowRaw(uint32_t wndVal)
{
    int8_t retval = 0;

    if(wndVal < WWDT_WINDOW_MIN)  {
        wndVal = WWDT_WINDOW_MIN;
        retval = -1;
    }
    else if (wndVal > WWDT_WINDOW_MAX) {
        wndVal = WWDT_WINDOW_MAX;
        retval = -1;
    }

    LPC_WDT->WINDOW = wndVal;

    return retval;
}


/********************************************************************//**
 * @brief         Update WDT timeout value and feed
 * @param[in]     WindowedTime    expected time to set watchdog window event(us)
 * @return        None
 *********************************************************************/
uint8_t WWDT_SetWindow(uint32_t WindowedTime)
{
    return WWDT_SetWindowRaw(WDT_GET_FROM_USEC(WindowedTime));
}
/*********************************************************************
 * @brief         Update WDT timeout value and feed
 * @param[in]     TimeOut    TimeOut value to be updated, should be in range:
 *                2048 .. 134217728
 * @return        None
 *********************************************************************/
void WDT_UpdateTimeOut(uint32_t TimeOut)
{
    /* check WDPROTECT,
     * if it is enable, wait until the counter is below the value of
     * WDWARNINT and WDWINDOW
     */
    if(LPC_WDT->MOD & (1<<4)) {
        while((LPC_WDT->TV <(LPC_WDT->WARNINT & WWDT_WDWARNINT_MASK))\
                        &&(LPC_WDT->TV <(LPC_WDT->WINDOW & WWDT_WDTC_MASK)));
    }

    WWDT_SetTimeOut(TimeOut);
}
/*********************************************************************
 * @brief            Read WWDT status flag
 * @param[in]        Status kind of status flag that you want to get, should be:
 *                 - WWDT_WARNINT_FLAG: watchdog interrupt flag
 *                 - WWDT_TIMEOUT_FLAG: watchdog time-out flag
 * @return           Time out flag status of WDT
 *********************************************************************/
FlagStatus WWDT_GetStatus (uint8_t Status)
{
    if(Status == WWDT_WARNINT_FLAG)  {
        return ((FlagStatus)(LPC_WDT->MOD & (1<<3)));
    }
    else if (Status == WWDT_TIMEOUT_FLAG) {
        return ((FlagStatus)(LPC_WDT->MOD & (1<<2)));
    }
    return RESET;
}

/*********************************************************************
 * @brief            Read WWDT status flag
 * @param[in]        Status kind of status flag that you want to get, should be:
 *                 - WWDT_WARNINT_FLAG: watchdog interrupt flag
 *                 - WWDT_TIMEOUT_FLAG: watchdog time-out flag
 * @return           Time out flag status of WDT
 *********************************************************************/
void WWDT_ClearStatusFlag (uint8_t flag)
{
    if(flag == WWDT_WARNINT_FLAG) {
        LPC_WDT->MOD |= WWDT_WDMOD_WDINT;               /* 清报警标志      */
    }
    else if(flag == WWDT_TIMEOUT_FLAG) {
        LPC_WDT->MOD &= ~ WWDT_WDMOD_WDTOF;             /* 清超时标志      */
    }
}

/*********************************************************************
 * @brief         Clear WDT Time out flag
 * @param[in]    None
 * @return        None
 *********************************************************************/
void WWDT_ClrTimeOutFlag (void)
{
    LPC_WDT->MOD &= ~ WWDT_WDMOD_WDTOF;
}


/*********************************************************************
 * @brief         Following the standard sequence to Feed the WatchDog Timer
 *
 * @param[in]    None
 *
 * @return        None
 *********************************************************************/
void WWDT_FeedStdSeq (void)
{
    LPC_WDT->FEED = 0xAA;

    LPC_WDT->FEED = 0x55;
}

/*********************************************************************
 * @brief          After set WDTEN, call this function to start Watchdog
 *                 or reload the Watchdog timer
 * @param[in]      None
 *
 * @return         None
 *********************************************************************/
void WWDT_Feed (void)
{
    if(WDTUSED)
	{
		__disable_irq();

		WWDT_FeedStdSeq();

		__enable_irq();
	}
	
}

/*********************************************************************
 * @brief         Get the current value of WDT
 * @param[in]     None
 * @return        current value of WDT
 *********************************************************************/
uint32_t WWDT_GetCurrentCount(void)
{
    return LPC_WDT->TV;
}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
