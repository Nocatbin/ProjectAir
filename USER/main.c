#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "I2C.h"
#include "SCD30.h"


int main(void)
{
	u8 data[9]={0};
	u8 i;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	//printf("USART INIT SUCCESSED\n");
	I2C_congfig();
	printf("I2C2 INIT SUCCESSED\n");
	I2C_StretchClockCmd(I2C2, ENABLE);
	SCD30_TriggerContinuousMeasurement();
	printf("SCD30 Measuring\n");
	delay_ms(100);
	SCD30_ReadMeasurement(data);
	printf("SCD30 Measure Success\n");
	for(i = 0; i<9; i++)
	{
		USART_SendData(USART1,data[i]);
		delay_ms(5);
	}
 	while(1);
}
