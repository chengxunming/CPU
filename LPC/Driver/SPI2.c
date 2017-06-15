#include "../User_code/global.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void WIZ_SPI_Init(void)
{
	SSP2Init(1);
}

// Connected to Data Flash
void WIZ_CS(uint8_t val)
{
	if (val == LOW) {
   		GPIO_ResetBits(GPIOB, WIZ_SCS); 
	}else if (val == HIGH){
   		GPIO_SetBits(GPIOB, WIZ_SCS); 
	}
}


uint8_t SPI2_SendByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
         
	  SPI_I2S_SendData(SPI2, byte);
          
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
          
	  return SPI_I2S_ReceiveData(SPI2);
}
/*
void SPI1_TXByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);       

	  SPI_I2S_SendData(SPI1, byte);	
}
*/