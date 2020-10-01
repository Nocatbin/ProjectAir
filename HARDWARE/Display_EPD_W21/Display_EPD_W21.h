
#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#define EPD_W21_WRITE_DATA 1
#define EPD_W21_WRITE_CMD  0
#include <stm32f10x.h>

void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void GPIO_Configuration(void);
void lcd_chkstatus(void);
void EPD_display_init(void);
void full_display(void pic_display(void));
void lut(void);
void lut1(void);
void full_display(void pic_display(void));
void pic_display_white(void);
void partial_display(u16 x_start,u16 x_end,u16 y_start,u16 y_end ,void partial_old(void),void partial_new(void)); //partial display
void partial00(void);
void partial01(void);
void partial02(void);
void partial_full00(void);
void partial_full02(void);

extern void EPD_W21_WriteCMD(unsigned char command);
static void EPD_W21_Update(void);
static void EPD_W21_EnableChargepump(void);
static void EPD_W21_DisableChargepump(void);
static void EPD_W21_WirteLUT(unsigned char *LUTvalue);
static void EPD_W21_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1);
static void EPD_W21_SetRamArea(unsigned char Xstart,unsigned char Xend,
						unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1);
static void EPD_W21_Write(unsigned char *value, unsigned char datalen);
static void EPD_W21_PowerOnSelfTest(void);
static void EPD_W21_PowerOnInit(void);

extern void EPD_W21_Init(void);
extern void EPD_W21_Test_Graphics(unsigned char PicNum);
extern void EPD_W21_UpdataDisplay(unsigned char *imgbuff);

#endif
/***********************************************************
						end file
***********************************************************/


