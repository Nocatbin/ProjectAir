
/***********************************************************/
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "LookUpTable.h"

unsigned char HRES_byte1,HRES_byte2,VRES_byte1,VRES_byte2;

void EPD_W21_Init(void)
{
	EPD_W21_BS_0;		// 4 wire spi mode selected 其实还是三线

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
	//CS-->PD8   SCK-->PD9  SDO--->PD10 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	// D/C--->PE15	   RES-->PE14
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_14 |GPIO_Pin_15;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	// BUSY--->PE13
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;    //abcd OE ST LT0输出
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
	driver_delay_xms(1);                       
}

void EPD_display_init(void)
{
		GPIO_Configuration();
	  HRES_byte1=0x01;			//0x0190 = 400D
	  HRES_byte2=0x90;
	  VRES_byte1=0x01;			//0x012c = 300D
	  VRES_byte2=0x2c;
	  EPD_W21_Init();
		EPD_W21_WriteCMD(0x01);			//POWER SETTING CMD
		EPD_W21_WriteDATA (0x03);
		EPD_W21_WriteDATA (0x00);
		EPD_W21_WriteDATA (0x2b);	// VDH 当前0x2b 11.0V  默认0x26 10.0V
		EPD_W21_WriteDATA (0x2b);	// VDL 当前0x2b 11.0V  默认0x26 -10.0V
		EPD_W21_WriteDATA (0x03);	// VDHR 3.0V

		EPD_W21_WriteCMD(0x06);			//boost soft start
		EPD_W21_WriteDATA (0x17);		//BT_PHA default setting
		EPD_W21_WriteDATA (0x17);		//BT_PHB
		EPD_W21_WriteDATA (0x17);		//BT_PHC

		EPD_W21_WriteCMD(0x04);  		//Power ON CMD
		lcd_chkstatus();

		// this section is essential, do not temper!
		EPD_W21_WriteCMD(0x00);			//panel setting CMD
		EPD_W21_WriteDATA(0x3f);		//400*300, LUT from OTP, BM mode
		EPD_W21_WriteDATA(0x0d);		//VCOM to 0V fast		这一行不知道是干嘛的
		
		EPD_W21_WriteCMD(0x30);			//PLL setting
    EPD_W21_WriteDATA (0x3a);   // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

		EPD_W21_WriteCMD(0x61);			//resolution setting CMD
		EPD_W21_WriteDATA (HRES_byte1);		
		EPD_W21_WriteDATA (HRES_byte2);       	 
		EPD_W21_WriteDATA (VRES_byte1);		
		EPD_W21_WriteDATA (VRES_byte2);
		
		EPD_W21_WriteCMD(0x82);			//vcom_DC setting CMD
    EPD_W21_WriteDATA (0x08);	

		EPD_W21_WriteCMD(0X50);			//VCOM AND DATA INTERVAL SETTING CMD
		EPD_W21_WriteDATA(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
	}

//LUT download
void lut(void)
{
	unsigned int count;
	EPD_W21_WriteCMD(0x20);
	for(count=0;count<44;count++)	     
		{EPD_W21_WriteDATA(lut_vcomDC[count]);}

	EPD_W21_WriteCMD(0x21);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_ww[count]);}   
	
	EPD_W21_WriteCMD(0x22);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bw[count]);} 

	EPD_W21_WriteCMD(0x23);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_wb[count]);} 

	EPD_W21_WriteCMD(0x24);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bb[count]);} 
}
	
void full_display(void pic_display(void))
{
		pic_display(); //picture
		lut(); //Power settings
		EPD_W21_WriteCMD(0x12);			//DISPLAY REFRESH 	
		driver_delay_xms(100);	    //!!!The delay here is necessary, 200uS at least!!!     
		lcd_chkstatus();
}

void pic_display_white(void)
{
	unsigned int i;
	EPD_W21_WriteCMD(0x10);
	for(i=0;i<15000;i++)	     
	{
		EPD_W21_WriteDATA(0x00);  
	}  
	driver_delay_xms(2);

	EPD_W21_WriteCMD(0x13);
	for(i=0;i<15000;i++)	     
	{
		EPD_W21_WriteDATA(0xff);  
	}  
	driver_delay_xms(2);		 
}

void lut1(void)
{
	unsigned int count;
	EPD_W21_WriteCMD(0x20);
	for(count=0;count<44;count++)	     
		{EPD_W21_WriteDATA(lut_vcom1[count]);}

	EPD_W21_WriteCMD(0x21);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_ww1[count]);}   
	
	EPD_W21_WriteCMD(0x22);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bw1[count]);} 

	EPD_W21_WriteCMD(0x23);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_wb1[count]);} 

	EPD_W21_WriteCMD(0x24);
	for(count=0;count<42;count++)	     
		{EPD_W21_WriteDATA(lut_bb1[count]);}   
}

void PartialRefresh(u16 x_start,u16 x_end,u16 y_start,u16 y_end ,unsigned char oldNumber[],unsigned char newNumber[]) //partial display
{
	unsigned int i;
	unsigned int j;
	j = (x_end - x_start) * (y_end - y_start) / 8;
	EPD_W21_WriteCMD(0x82);			//vcom_DC setting  	
  EPD_W21_WriteDATA (0x08);	
	EPD_W21_WriteCMD(0X50);
	EPD_W21_WriteDATA(0x47);		
	lut1();
	EPD_W21_WriteCMD(0x91);		//This command makes the display enter partial mode
	EPD_W21_WriteCMD(0x90);		//resolution setting AKA partial window
	EPD_W21_WriteDATA ((400-x_end)/256);
	EPD_W21_WriteDATA ((400-x_end)%256);   //x-start    
	EPD_W21_WriteDATA ((400-x_start)/256);		
	EPD_W21_WriteDATA ((400-x_start)%256-1);  //x-end

	EPD_W21_WriteDATA (y_start/256);
	EPD_W21_WriteDATA (y_start%256);   //y-start    
		
	EPD_W21_WriteDATA (y_end/256);		
	EPD_W21_WriteDATA (y_end%256-1);  //y-end
	EPD_W21_WriteDATA (0x28);	

	EPD_W21_WriteCMD(0x10);	       //writes Old data to SRAM for programming
	
	for(i=0;i<j;i++)	     
	{
		EPD_W21_WriteDATA(oldNumber[i]);
	}
	
	EPD_W21_WriteCMD(0x13);				 //writes New data to SRAM.
	for(i=0;i<j;i++)	     
	{
		EPD_W21_WriteDATA(newNumber[i]);  
	}
    	
	EPD_W21_WriteCMD(0x12);		 //DISPLAY REFRESH 		             
	driver_delay_xms(1);     //!!!The delay here is necessary, 200uS at least!!!     
	lcd_chkstatus();
}

void partial00(void)
{
	unsigned int i;
	for(i=0;i<1936;i++)	     
	{
			EPD_W21_WriteDATA(0x00);  
	}  
}

void partial01(void)
{
	unsigned int i;

		for(i=0;i<1936;i++)	     
			{
			EPD_W21_WriteDATA (~LargeNumber_0[i]);
			driver_delay_xms(2);  
			}	
}
/*
void partial02(void)
{
  unsigned int i;
		for(i=0;i<256;i++)	     
			{
			EPD_W21_WriteDATA (~gImage_num2[i]);
			driver_delay_xms(2);  
			}	
	}
*/
void partial_full00(void)
{
	unsigned int i;
	for(i=0;i<15000;i++)	     
	{
			EPD_W21_WriteDATA(0x00);  
	}  
}

/***********************************************************
						end file
***********************************************************/

