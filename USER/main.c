#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "I2C.h"
#include "SCD30.h"
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

int main(void)
{
	u8 data[18]={0};
	u8 ready = 0;
	float CO2, Temperature, Humidity;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化
  EPD_display_init(); //EPD init
	I2C_congfig();
	I2C_StretchClockCmd(I2C2, ENABLE);
	SCD30_TriggerContinuousMeasurement();
	delay_ms(3);
	while(1)
	{
		ready = SCD30_CheckDataReady();
		if(ready)
		{
		SCD30_ReadMeasurement(data);
		CO2 = bytes_to_float(data[0], data[1], data[3], data[4]);
		Temperature = bytes_to_float(data[6], data[7], data[9], data[10]);
		Humidity = bytes_to_float(data[12], data[13], data[15], data[16]);
		printf("Co2:%f", CO2);
		printf("T:%f", Temperature);
		printf("H:%f\n", Humidity);
		}
	}
}

