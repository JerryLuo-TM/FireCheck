#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "spi.h"
#include "tft.h"
#include "spi.h"

#define ROW  128			//显示行数
#define COL  128			//显示列数

//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
uint16_t DeviceCode;


#define HAL_SPI_HW_ENABLE 1

void SendDataSPI(uint8_t dat)
{
#ifdef HAL_SPI_HW_ENABLE
	SPI1_ReadWriteByte(dat);
#else
	uint8_t i;

	for(i = 0; i < 8; i++) {
		if((dat & 0x80) != 0) {
			LCD_SDA = 1;
		} else {
			LCD_SDA = 0;
		}
		dat <<= 1;
		LCD_SCL = 0;
		LCD_SCL = 1;
	}
#endif
}

void LCD_WR_REG(uint32_t i)
{
	LCD_CS = 0;
	LCD_RS = 0; //命令
	SendDataSPI(i);
	LCD_CS = 1;
}

void LCD_WR_DATA(uint32_t i)
{
	LCD_CS = 0;
	LCD_RS = 1; //数据
	SendDataSPI(i);
	LCD_CS = 1;
}

void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

#ifdef HAL_SPI_HW_ENABLE
	SPI1_Init();
#else
	/* 模拟SPI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
#endif

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10);
}

void LCD_RESET(void)
{
	LCD_RST = 1;
	delay_ms(50);
	LCD_RST = 0;
	delay_ms(100);
	LCD_RST = 1;
	delay_ms(50);
}

void LCD_Init(void)
{
	LCD_GPIOInit();
	LCD_RESET();

	LCD_WR_REG(0x11); //Exit Sleep
	delay_ms(50);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	LCD_WR_REG(0xB3);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	//------------------------------------End ST7735S Frame Rate-----------------------------------------//
	LCD_WR_REG(0xB4); //Dot inversion
	LCD_WR_DATA(0x03); //03

	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0xa2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x84);

	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0XC5);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x8D);
	LCD_WR_DATA(0x2A);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x8D);
	LCD_WR_DATA(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA(0x03);

	LCD_WR_REG(0x3a); //Set Color Format
	LCD_WR_DATA(0x05);

	LCD_WR_REG(0x36); //Set Scanning Direction
	LCD_WR_DATA(0x28); //0xc8

	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x2c);
	LCD_WR_DATA(0x25);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x27);
	LCD_WR_DATA(0x2f);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x1c);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x2d);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x23);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x26);
	LCD_WR_DATA(0x2f);
	LCD_WR_DATA(0x3B);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0x30);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x80);

	LCD_WR_REG(0x12);

	LCD_WR_REG(0x29); // Display On
}

void LCD_BlockWrite(uint32_t Xstart,uint32_t Xend,uint32_t Ystart,uint32_t Yend)
{
	uint16_t x, x1;

	/* 水平翻转 */
	x = 128 - Xstart;
	x1 = 128 - Xend;

	// y = 128 - Ystart;
	// y1 = 128 - Yend;

	LCD_WR_REG(0x2a);
	LCD_WR_DATA(x1>>8);
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x>>8);
	LCD_WR_DATA(x);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(Ystart>>8);
	LCD_WR_DATA(Ystart);
	LCD_WR_DATA(Yend>>8);
	LCD_WR_DATA(Yend);

	LCD_WR_REG(0x2c);
}

void LCD_DispColor(uint32_t color)
{
	uint32_t i,j;
	LCD_BlockWrite(0, COL - 1, 0, ROW - 1);
	LCD_CS = 0;
	LCD_RS = 1; //写数据
	for(i = 0; i < ROW; i++) {
		for(j = 0; j < COL; j++) {
			/* 写一次数据  持续 CLK ~CLK */
			SendDataSPI(color >> 8);
			SendDataSPI(color);
		}
	}
	LCD_CS = 1;
}

void LCD_ColorBox(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t color)
{
	uint32_t temp;
	LCD_BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	LCD_CS = 0;
	LCD_RS = 1;
	for (temp=0; temp<xLong*yLong; temp++) {
		SendDataSPI(color>>8);
		SendDataSPI(color);
	}
	LCD_CS = 1;
}

void LCD_SetPixel(uint16_t  x,uint16_t  y,uint16_t  color)
{
	LCD_BlockWrite(x, x, y, y);
	LCD_CS = 0;
	LCD_RS = 1;
	SendDataSPI(color>>8);
	SendDataSPI(color);
	LCD_CS = 1;
}

void LCD_SetBigPixel(uint16_t  x,uint16_t  y,uint16_t  color)
{
	uint16_t i;
	LCD_BlockWrite(x, x+1, y, y+1);
	LCD_CS = 0;
	LCD_RS = 1;
	for (int i = 0; i < 4; i++) {
		SendDataSPI(color>>8);
		SendDataSPI(color);
	}
	LCD_CS = 1;
}

void LCD_DispPic(uint16_t x,uint16_t y,uint16_t w, uint16_t h,const uint8_t *p)
{
	uint32_t i,j;
	uint8_t pic_h,pic_l;

	LCD_WR_REG(0x36); //Set Scanning Direction
	LCD_WR_DATA(0xC8); //0xc8

	LCD_BlockWrite(x, x + w - 1, y, y + h - 1);
	LCD_CS = 0;
	LCD_RS = 1;
	for(i = 0; i < h; i++)
	{
		for(j=0;j<w;j++)
		{
			pic_l=(*p) ;
			pic_h=(*(p+1)) ;
			SendDataSPI(pic_h);
			SendDataSPI(pic_l);
			p=p+2;
		}
	}
	LCD_CS = 1;
}

// x: 0-7
// y: 0-7
void LCD_Fill_region(uint8_t x, uint8_t y, uint16_t color)
{
	uint8_t x_start, y_start;
	if (x >= 8) {return;}
	if (y >= 8) {return;}

	x_start = x * 16;
	y_start = y * 16;

	LCD_BlockWrite(x_start, x_start + 16 -1, y_start, y_start + 16 -1);

	LCD_CS = 0;
	LCD_RS = 1;
	for (int i = 0; i < 16 * 16; i++) {
		SendDataSPI(color>>8);
		SendDataSPI(color);
	}
	LCD_CS = 1;
}

void LCD_Show_raw_map(float (*buffer)[8], uint16_t length)
{
	uint8_t i, j, colorIndex;
	float colorTemp, Temputer;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			/* 调整方向 */
			colorTemp = buffer[7 - i][7 - j];
			Temputer = colorTemp;
			colorIndex = Temputer - 20.0f;
			// LCD_Fill_region(j, i, camColors[colorIndex]);
		}
	}
}







