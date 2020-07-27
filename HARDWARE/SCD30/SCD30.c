#include "stm32f10x.h"
#include "I2C.h"
#include "SCD30.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"


uint8_t sensirion_common_generate_crc(const uint8_t* data, uint16_t count)
{
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

int8_t sensirion_common_check_crc(const uint8_t* data, uint16_t count,uint8_t checksum)
{
    if (sensirion_common_generate_crc(data, count) != checksum)
        return STATUS_FAIL;
    return NO_ERROR;
}

float bytes_to_float(u8 MMSB, u8 MLSB, u8 LMSB, u8 LLSB)
{
    union
		{
        uint32_t u32_value;
        float float32;
    } tmp;

    tmp.u32_value = bytes_to_uint32_t(MMSB, MLSB, LMSB, LLSB);
    return tmp.float32;
}

u32 bytes_to_uint32_t(u8 MMSB, u8 MLSB, u8 LMSB, u8 LLSB)
{
	
	return (u32)MMSB << 24 | (u32)MLSB << 16 |
           (u32)LMSB << 8 | (u32)LLSB;
}



//��ȡSCD30���������� ����CRCλ ��9���ֽ�
//Param
//*data ��ȡ���������׵�ַ ��Ϊ����
void SCD30_ReadMeasurement(u8 *data)
{
	u32 i = 0, j = 0;
	//����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error8\n");
			break;
		}
	}
	
	i = 0;
	//�����豸��ַ дģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=1000)
		{
			printf("error9\n");
			break;
		}
	}
	
	i = 0;
	//��������,�������ݵ��ڴ��ַMSB 0x03
	I2C_SendData(I2C_Num, 0x03);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
		{
			printf("error10\n");
			break;
		}
	}
	
	i = 0;
	delay_ms(1);
	//��������,�������ݵ��ڴ��ַLSB 0x00
	I2C_SendData(I2C_Num, 0x00);
	//���EV8_2�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error11\n");
			break;
		}
	}
	
	i = 0;
	//����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
	//д���̽���  ׼����ʼ������
	
	//�ӳ�3ms����
	delay_ms(5);
	
	//�����ݹ��̿�ʼ
	//�ٴ�����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error12\n");
			break;
		}
	}
	
	i = 0;
	//�����豸��ַ ��ģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Receiver);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error13\n");
			break;
		}
	}
	
	i = 0;
	//��ʼ������
	for(j = 0; j < 18; j++)
	{
		if(j == 17)
		{
			//׼�������һ���ֽ� ����NACK
			I2C_AcknowledgeConfig(I2C_Num, DISABLE);
		}
		//���EV7�¼�
		while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
		{
			i++;
			if(i>=10000)
			{
				printf("error14\n");
				break;
			}
		}
		i = 0;
		//��һ���ֽ����� 
		*data = I2C_ReceiveData(I2C_Num);
		data++;
	}
	
	//���´��Զ�ACK
	I2C_AcknowledgeConfig(I2C_Num, DISABLE);
	
	//���ݴ������ ����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}



//��SCD30�������ɼ�����ģʽ
void SCD30_TriggerContinuousMeasurement(void)
{
	u32 i = 0;
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100000)
		{
			printf("error1\n");
			break;
		}
	}
	
	i = 0;
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=100000)
		{
			printf("error2\n");
			break;
		}
	}
	
	i = 0;
	//���·���16bit CMD
	I2C_SendData(I2C_Num, 0x00);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
		{
			printf("error3\n");
			break;
		}
	}
	i = 0;
	delay_ms(1);
	I2C_SendData(I2C_Num, 0x10);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
		{
			printf("error4\n");
			break;
		}
	}
	
	i = 0;
	delay_ms(1);
	//���·���16bit Pressure
	I2C_SendData(I2C_Num, 0x00);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
		{
			printf("error5\n");
			break;
		}
	}
	
	i = 0;
	delay_ms(1);
	I2C_SendData(I2C_Num, 0x00);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=100)
		{
			printf("error6\n");
			break;
		}
	}
	
	i = 0;
	delay_ms(1);
	//����CRC
	I2C_SendData(I2C_Num, 0x81);
	//���EV8_2�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		i++;	
		if(i>=100000)
		{
			printf("error7\n");
			break;
		}
	}
	
	//���ݴ������ ����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
}


u8 SCD30_CheckDataReady(void)
{
	u8 DataReadyMSB, DataReadyLSB;
	u32 i=0;
	//����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=100000)
		{
			printf("error8\n");
			break;
		}
	}
	
	i=0;
	delay_ms(1);
	//�����豸��ַ дģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Transmitter);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error9\n");
			break;
		}
	}
	
	i = 0;
	//��������,�������ݵ��ڴ��ַMSB 0x02
	I2C_SendData(I2C_Num, 0x02);
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR)
	{
		i++;
		if(i>=1000)
		{
			printf("error10\n");
			break;
		}
	}
	
	i = 0;
	delay_ms(1);
	//��������,�������ݵ��ڴ��ַLSB 0x02
	I2C_SendData(I2C_Num, 0x02);
	//���EV8_2�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error11\n");
			break;
		}
	}
	
	i = 0;
	//����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);
	//д���̽���  ׼����ʼ������
	
	//�ӳ�3ms����
	delay_ms(5);
	
	//�����ݹ��̿�ʼ
	//�ٴ�����Start�ź�
	I2C_GenerateSTART(I2C_Num, ENABLE);
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_MODE_SELECT) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error12\n");
			break;
		}
	}
	
	i = 0;
	//�����豸��ַ ��ģʽ
	I2C_Send7bitAddress(I2C_Num, TargetAddress, I2C_Direction_Receiver);
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error13\n");
			break;
		}
	}
	
	i = 0;
	//��ʼ������
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error14\n");
			break;
		}
	}
	
	i = 0;
	//��һ���ֽ����� 
	DataReadyMSB = I2C_ReceiveData(I2C_Num);
	
	//׼�������һ���ֽ� ����NACK
	I2C_AcknowledgeConfig(I2C_Num, DISABLE);
	
	while(I2C_CheckEvent(I2C_Num, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR)
	{
		i++;
		if(i>=10000)
		{
			printf("error14\n");
			break;
		}
	}
	DataReadyLSB = I2C_ReceiveData(I2C_Num);
	
	//���ݴ������ ����Stop�ź�
	I2C_GenerateSTOP(I2C_Num, ENABLE);

	if(DataReadyMSB == 0x00 && DataReadyLSB == 0x01)
		return 0x01;
	else
		return 0;
}
