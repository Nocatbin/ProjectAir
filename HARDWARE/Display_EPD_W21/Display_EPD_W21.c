
/***********************************************************/
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"


void EPD_W21_Init(void)
{
	EPD_W21_BS_0;		// 4 wire spi mode selected

	EPD_W21_RST_0;		// Module reset
	driver_delay_xms(1000);
	EPD_W21_RST_1;
	driver_delay_xms(1000);
	
//	EPD_W21_DispInit();		// pannel configure

//	EPD_W21_WirteLUT(LUTDefault);	// update wavefrom

}

void driver_delay_us(unsigned int xus)
{
	for(;xus>1;xus--);
}

void driver_delay_xms(unsigned long xms)	
{	
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
	{
        for(i=0; i<256; i++);
    }
}

/***********************************************************
						end file
***********************************************************/

