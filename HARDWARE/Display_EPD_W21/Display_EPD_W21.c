
/***********************************************************/
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

unsigned char HRES_byte1,HRES_byte2,VRES_byte1,VRES_byte2;

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

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;    //abcd OE ST LT0Êä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_14 |GPIO_Pin_15;    //abcd OE ST LT0Êä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;    //abcd OE ST LT0Êä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
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

void lcd_chkstatus(void)
{
	unsigned char busy;
	do
	{
		EPD_W21_WriteCMD(0x71);
		busy = isEPD_W21_BUSY;
		busy =!(busy & 0x01);        
	}
	while(busy);   
	driver_delay_xms(200);                       
}

void EPD_display_init(void)
{
		GPIO_Configuration();
	  HRES_byte1=0x01;			//400
	  HRES_byte2=0x90;
	  VRES_byte1=0x01;			//300
	  VRES_byte2=0x2c;
	  EPD_W21_Init();
		EPD_W21_WriteCMD(0x01);			//POWER SETTING 
		EPD_W21_WriteDATA (0x03);	          
		EPD_W21_WriteDATA (0x00);
		EPD_W21_WriteDATA (0x2b);
		EPD_W21_WriteDATA (0x2b);
		//EPD_W21_WriteDATA (0x03);

		EPD_W21_WriteCMD(0x06);         //boost soft start
		EPD_W21_WriteDATA (0x17);		//A
		EPD_W21_WriteDATA (0x17);		//B
		EPD_W21_WriteDATA (0x17);		//C       

		EPD_W21_WriteCMD(0x04);  
		lcd_chkstatus();

		EPD_W21_WriteCMD(0x00);			//panel setting
		EPD_W21_WriteDATA(0xbf);		//LUT from OTP£¬128x296
		EPD_W21_WriteDATA(0x0d);		//VCOM to 0V fast
		
		EPD_W21_WriteCMD(0x30);			//PLL setting
    EPD_W21_WriteDATA (0x3a);   // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

		EPD_W21_WriteCMD(0x61);			//resolution setting
		EPD_W21_WriteDATA (HRES_byte1);		
		EPD_W21_WriteDATA (HRES_byte2);       	 
		EPD_W21_WriteDATA (VRES_byte1);		
		EPD_W21_WriteDATA (VRES_byte2);
		
		EPD_W21_WriteCMD(0x82);			//vcom_DC setting  	
    EPD_W21_WriteDATA (0x08);	

		EPD_W21_WriteCMD(0X50);			//VCOM AND DATA INTERVAL SETTING			
		EPD_W21_WriteDATA(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
	}

/***********************************************************
						end file
***********************************************************/

