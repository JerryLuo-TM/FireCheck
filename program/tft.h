#ifndef __TFT_H
#define __TFT_H

#include "stdio.h"
#include "sys.h"

#define LCD_SCL     PAout(5)
#define LCD_SDA     PAout(7)
#define LCD_CS      PBout(0)

#define LCD_RS      PBout(2)
#define LCD_RST     PBout(1)

#define LCD_BL      PBout(10)

#define WHITE       0xFFFF
#define BLACK      	0x0000
#define BLUE       	0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40 //棕色
#define BRRED       0XFC07 //棕红色
#define GRAY        0X8430 //灰色

void LCD_GPIOInit(void);
void LCD_Init(void);
void DispColor(uint32_t color);
void DispFrame(void);
void DispBand(void);
void DispGrayHor16(void);
void DispGrayHor32(void);
void BlockWrite(uint32_t Xstart,uint32_t Xend,uint32_t Ystart,uint32_t Yend);
void DrawGird(uint32_t color);
void PutPixel(uint32_t x,uint32_t y,uint32_t color);
void DispStr(uint8_t *str,uint32_t Xstart,uint32_t Ystart,uint32_t TextColor,uint32_t BackColor);
uint32_t ReadData(void);
void DispInt(uint32_t i,uint32_t Xstart,uint32_t Ystart,uint32_t TextColor,uint32_t BackColor);
void DispPic(uint16_t x,uint16_t y,uint16_t w, uint16_t h,const uint8_t *p);



#endif


