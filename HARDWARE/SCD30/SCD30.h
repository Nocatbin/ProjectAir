#ifndef __SCD30_H
#define __SCD30_H	 
#include "sys.h"

#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF
#define CRC8_LEN 1
#define STATUS_OK 0
#define STATUS_FAIL (-1)
#define NO_ERROR 0

float bytes_to_float(u8 MMSB, u8 MLSB, u8 LMSB, u8 LLSB);
u32 bytes_to_uint32_t(u8 MMSB, u8 MLSB, u8 LMSB, u8 LLSB);
void SCD30_ReadMeasurement(u8 *data);
void SCD30_TriggerContinuousMeasurement(void);
u8 SCD30_CheckDataReady(void);

uint8_t sensirion_common_generate_crc(const uint8_t* data, uint16_t count);
int8_t sensirion_common_check_crc(const uint8_t* data, uint16_t count,uint8_t checksum);
	
#endif
