#ifndef __I2C_H
#define __I2C_H	 
#include "sys.h"

#define TargetAddress 0x61
#define I2C_Num					I2C2
#define I2C_CLK					RCC_APB1Periph_I2C2
#define I2C_SDA_Pin 		GPIO_Pin_11
#define I2C_SCL_Pin 		GPIO_Pin_10

void I2C_congfig(void);
void I2C_WriteByte(u8 address, u8 data);
void I2C_ReadData(u8 addressMSB, u8 addressLSB, u8 *data, u8 ByteNum);
#endif
