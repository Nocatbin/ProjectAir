#include "stm32f10x.h"
#include "I2C.h"
#include "sys.h" 
#include "delay.h"
							    
//I2C��ʼ����ʹ��
void I2C_congfig(void)
{
	//����I2C �� GPIO ��ʼ���ṹ��
 	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ��PORTBʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//ʹ��I2Cʱ��
	RCC_APB1PeriphClockCmd(I2C_CLK,ENABLE);
	
	//IO�ڸ�������
	GPIO_InitStructure.GPIO_Pin  = I2C_SCL_Pin|I2C_SDA_Pin;
	//���óɿ�©���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//I2C����ģʽ��ʼ��
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;	//�Զ�Ӧ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//7λ��ַģʽ
	I2C_InitStructure.I2C_ClockSpeed = 50000;	//50KHZ
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x5F;	//STM32�ĵ�ַ����Ϊ5F
	I2C_Init(I2C_Num,&I2C_InitStructure);
	
	//ʹ��I2C
	I2C_Cmd(I2C_Num, ENABLE);
}


//д��һ���ֽ�
void I2C_WriteByte(u8 address, u8 data)
{
	u8 i = 0;
	//����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//�����豸��ַ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//��������,���ڴ��ַ
	I2C_SendData(I2C_Num, address);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//����������Ҫ���͵�����
	I2C_SendData(I2C_Num, address);
	//���EV8_2�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		i++;	
		if(i>=100)
			break;
	}
	
	//���ݴ������ ����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}


//��ȡByteNum���ֽڵ�����
void I2C_ReadData(u8 addressMSB, u8 addressLSB, u8 *data, u8 ByteNum)
{
	u8 i = 0, j = 0;
	//����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//�����豸��ַ дģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//��������,���ڴ��ַMSB
	I2C_SendData(I2C_Num, addressMSB);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//��������,���ڴ��ַLSB
	I2C_SendData(I2C_Num, addressLSB);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
	
	//�ӳ�3ms����
	delay_ms(4);
	
	//�ٴ�����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//�����豸��ַ ��ģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Receiver);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100)
			break;
	}
	
	//��ʼ������
	for(j = 0; j < ByteNum; j++)
	{
		if(j == ByteNum-1)
		{
			//׼�������һ���ֽ� ����NACK
			I2C_AcknowledgeConfig(I2C_Num, DISABLE);
		}
		//���EV7�¼�
		while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
		{
			i++;
			if(i>=100)
				break;
		}
		//��һ���ֽ�����
		*data = I2C_ReceiveData(I2C_Num);
		data++;
	}
	
	//���´��Զ�ACK
	I2C_AcknowledgeConfig(I2C_Num, DISABLE);
	
	//���ݴ������ ����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}
