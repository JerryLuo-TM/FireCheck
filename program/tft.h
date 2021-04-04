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
void LCD_DispColor(uint32_t color);
void LCD_BlockWrite(uint32_t Xstart,uint32_t Xend,uint32_t Ystart,uint32_t Yend);
void LCD_ColorBox(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t color);

void LCD_SetPixel(uint16_t  x,uint16_t  y,uint16_t  color);
void LCD_SetBigPixel(uint16_t  x,uint16_t  y,uint16_t  color);
void LCD_DispPic(uint16_t x,uint16_t y,uint16_t w, uint16_t h,const uint8_t *p);



void LCD_Fill_region(uint8_t x, uint8_t y, uint16_t color);
void LCD_Show_raw_map(float (*buffer)[8], uint16_t length);

#endif


