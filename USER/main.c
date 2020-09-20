#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "I2C.h"
#include "SCD30.h"
#include "lcd.h"


int main(void)
{
	u8 data[18]={0};
	u8 ready = 0;
	u32 count = 1;
	float CO2, Temperature, Humidity;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为
	LCD_Init();			   	//初始化LCD
	I2C_congfig();
	I2C_StretchClockCmd(I2C2, ENABLE);
	SCD30_TriggerContinuousMeasurement();
	delay_ms(3);
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60,50,310,24,24, "Stability Test In Progress");
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
		LCD_ShowString(60,80,200,24,24, "CO2");
		LCD_ShowNum(60,110,CO2,5,24);
		LCD_ShowString(60,140,200,24,24, "Temp");
		LCD_ShowNum(60,170,Temperature,2,24);
		LCD_ShowString(60,200,200,24,24, "Humidity");
		LCD_ShowNum(60,230,Humidity,2,24);
		LCD_ShowString(60,260,200,24,24, "Count");
		LCD_ShowNum(60,290,count,2,24);
		count++;
		}
	}
}

