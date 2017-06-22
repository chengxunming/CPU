#include "bsp_led.h"

void LED_Init(void)
{
	//LEDRUN
	LPC_GPIO4->DIR |= 1ul<<8;                                            /*����P4.8Ϊ���    */
    LPC_GPIO4->SET |= 1ul<<8;                                            /*����P4.8Ϊ�ߵ�ƽ  */
	//LED F1A
	LPC_GPIO4->DIR |= 1ul<<4;                                            /*����P4.4Ϊ���    */
    LPC_GPIO4->SET |= 1ul<<4;                                            /*����P4.4Ϊ�ߵ�ƽ  */
	//LED F1B
	LPC_GPIO4->DIR |= 1ul<<5;                                            /*����P4.5Ϊ���    */
    LPC_GPIO4->SET |= 1ul<<5;                                            /*����P4.5Ϊ�ߵ�ƽ  */
	//LED F2A
	LPC_GPIO4->DIR |= 1ul<<6;                                            /*����P4.6Ϊ���    */
    LPC_GPIO4->SET |= 1ul<<6;                                            /*����P4.6Ϊ�ߵ�ƽ  */
	//LED F2B
	LPC_GPIO4->DIR |= 1ul<<7;                                            /*����P4.7Ϊ���    */
    LPC_GPIO4->SET |= 1ul<<7;                                            /*����P4.7Ϊ�ߵ�ƽ  */
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

