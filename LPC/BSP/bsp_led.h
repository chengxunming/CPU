#ifndef __BSP_LED_H
#define __BSP_LED_H 			   

#include "bsp_include.h"

#define RUNLEDON  LPC_GPIO4->CLR = 1ul << 8;                //ÔËÐÐLED
#define RUNLEDOFF LPC_GPIO4->SET = 1ul << 8;

#define FAULTLEDON  LPC_GPIO4->CLR = 1ul << 4;				//F1A
#define FAULTLEDOFF LPC_GPIO4->SET = 1ul << 4;

#define F1BON  LPC_GPIO4->CLR = 1ul << 5;				//F1B
#define F1BOFF LPC_GPIO4->SET = 1ul << 5;

#define PWRLEDON  LPC_GPIO4->CLR = 1ul << 6;				//F2A
#define PWRLEDOFF LPC_GPIO4->SET = 1ul << 6;

#define F2BON  LPC_GPIO4->CLR = 1ul << 7;				//F2B
#define F2BOFF LPC_GPIO4->SET = 1ul << 7;


void LED_Init(void);
void LED_AllOn(void);
void LED_AllOff(void);

#endif

