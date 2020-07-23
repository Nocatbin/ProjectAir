#include "stm32f10x.h"
#include "I2C.h"
#include "SCD30.h"
#include "sys.h" 
#include "delay.h"


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
																	
float sensirion_bytes_to_float(const uint8_t* bytes)
{
    union
		{
        uint32_t u32_value;
        float float32;
    } tmp;

    tmp.u32_value = sensirion_bytes_to_uint32_t(bytes);
    return tmp.float32;
}

uint32_t sensirion_bytes_to_uint32_t(const uint8_t* bytes)
{
    return (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 | (uint32_t)bytes[3];
}
