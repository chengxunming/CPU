/*****************************************************************************
 *   timer.h:  Header file for NXP LPC177x_8x Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

#include "../LPC177x_8x/type.h"

extern void SysTick_Handler (void);
extern void SYSTICK_InternalInit(uint32_t time);
extern void SYSTICK_ExternalInit(uint32_t freq, uint32_t time);
extern void SYSTICK_Cmd(uint32_t NewState);
extern void SYSTICK_IntCmd(uint32_t NewState);
extern uint32_t SYSTICK_GetCurrentValue(void);
extern void SYSTICK_ClearCounterFlag(void);
extern void SysTickInit (uint32_t time);

#endif /* end __TIMER_H */

/*****************************************************************************
**                            End Of File
******************************************************************************/
