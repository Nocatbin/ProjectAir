#include "stm32f10x.h"
#include "I2C.h"
#include "sys.h" 
#include "delay.h"
							    
//I2C初始化及使能
void I2C_congfig(void)
{
	//建立I2C 和 GPIO 初始化结构体
 	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能PORTB时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//使能I2C时钟
	RCC_APB1PeriphClockCmd(I2C_CLK,ENABLE);
	
	//IO口复用配置
	GPIO_InitStructure.GPIO_Pin  = I2C_SCL_Pin|I2C_SDA_Pin;
	//设置成开漏输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//I2C工作模式初始化
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;	//自动应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//7位地址模式
	I2C_InitStructure.I2C_ClockSpeed = 50000;	//50KHZ
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x5F;	//STM32的地址设置为5F
	I2C_Init(I2C_Num,&I2C_InitStructure);
	
	//使能I2C
	I2C_Cmd(I2C_Num, ENABLE);
}


//写入一个字节
void I2C_WriteByte(u8 address, u8 data)
{
	u8 i = 0;
	//生成Start信号
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//检测EV5事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送设备地址
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//检测EV6事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送数据,即内存地址
	I2C_SendData(I2C_Num, address);
	//检测EV8事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送真正需要发送的数据
	I2C_SendData(I2C_Num, address);
	//检测EV8_2事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		i++;	
		if(i>=100)
			break;
	}
	
	//数据传输完成 生成Stop信号
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}


//读取ByteNum个字节的数据
void I2C_ReadData(u8 addressMSB, u8 addressLSB, u8 *data, u8 ByteNum)
{
	u8 i = 0, j = 0;
	//生成Start信号
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//检测EV5事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送设备地址 写模式
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//检测EV6事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送数据,即内存地址MSB
	I2C_SendData(I2C_Num, addressMSB);
	//检测EV8事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送数据,即内存地址LSB
	I2C_SendData(I2C_Num, addressLSB);
	//检测EV8事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//生成Stop信号
	I2C_GenerateSTOP(I2C_Num, ENABLE);
	
	//延迟3ms以上
	delay_ms(4);
	
	//再次生成Start信号
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//检测EV5事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//发送设备地址 读模式
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Receiver);
	//检测EV6事件
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//开始读数据
	for(j = 0; j < ByteNum; j++)
	{
		if(j == ByteNum-1)
		{
			//准备读最后一个字节 发送NACK
			I2C_AcknowledgeConfig(I2C_Num, DISABLE);
		}
		//检测EV7事件
		while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
		{
			i++;
			if(i>=100)
				break;
		}
		//读一个字节数据
		*data = I2C_ReceiveData(I2C_Num);
		data++;
	}
	
	//重新打开自动ACK
	I2C_AcknowledgeConfig(I2C_Num, DISABLE);
	
	//数据传输完成 生成Stop信号
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}
