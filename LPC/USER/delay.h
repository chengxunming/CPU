#ifndef __DELAY_H
#define __DELAY_H

#include "LPC177x_8x.h"                             /* LPC17xx definitions    */

void delay_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);

#endif
