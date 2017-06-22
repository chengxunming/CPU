#include "bsp_led.h"

void LED_Init(void)
{
	//LEDRUN
	LPC_GPIO4->DIR |= 1ul<<8;                                            /*设置P4.8为输出    */
    LPC_GPIO4->SET |= 1ul<<8;                                            /*设置P4.8为高电平  */
	//LED F1A
	LPC_GPIO4->DIR |= 1ul<<4;                                            /*设置P4.4为输出    */
    LPC_GPIO4->SET |= 1ul<<4;                                            /*设置P4.4为高电平  */
	//LED F1B
	LPC_GPIO4->DIR |= 1ul<<5;                                            /*设置P4.5为输出    */
    LPC_GPIO4->SET |= 1ul<<5;                                            /*设置P4.5为高电平  */
	//LED F2A
	LPC_GPIO4->DIR |= 1ul<<6;                                            /*设置P4.6为输出    */
    LPC_GPIO4->SET |= 1ul<<6;                                            /*设置P4.6为高电平  */
	//LED F2B
	LPC_GPIO4->DIR |= 1ul<<7;                                            /*设置P4.7为输出    */
    LPC_GPIO4->SET |= 1ul<<7;                                            /*设置P4.7为高电平  */
}

void LED_AllOn(void)
{
	RUNLEDON;
	FAULTLEDON;
	PWRLEDON;
	F1BON;
	F2BON;
}

void LED_AllOff(void)
{
	RUNLEDOFF;
	FAULTLEDOFF;
	PWRLEDOFF;
	F1BOFF;
	F2BOFF;
}

