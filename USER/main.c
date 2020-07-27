#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "I2C.h"
#include "SCD30.h"


int main(void)
{
	u8 data[18]={0};
	float CO2, Temperature, Humidity;
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	I2C_congfig();
	I2C_StretchClockCmd(I2C2, ENABLE);
	SCD30_TriggerContinuousMeasurement();
	while(1)
	{
		delay_ms(3);
		while(SCD30_CheckDataReady())
		{
			SCD30_ReadMeasurement(data);
			CO2 = bytes_to_float(data[0], data[1], data[3], data[4]);
			Temperature = bytes_to_float(data[6], data[7], data[9], data[10]);
			Humidity = bytes_to_float(data[12], data[13], data[15], data[16]);
			printf("Co2:%f", CO2);
			printf("T:%f", Temperature);
			printf("H:%f", Humidity);
		}
	}
}
