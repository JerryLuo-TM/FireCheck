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

const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};


// #define HAL_SPI_HW_ENABLE 1

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

void WriteDispData(uint8_t color)
{
	//CS0=0;
	SendDataSPI(color);
	//CS0=1;
}

// #define LCD_SCL     PAout(5)
// #define LED_SDA     PAout(7)
// #define LED_CS      PBout(0)

// #define LED_RS      PBout(2)
// #define LED_RST     PBout(1)

// #define LED_BL      PBout(10)
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

void BlockWrite(uint32_t Xstart,uint32_t Xend,uint32_t Ystart,uint32_t Yend)
{
	uint16_t x, x1;

	/* 水平翻转 */
	x = 128 - Xstart;
	x1 = 128 - Xend;

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

void DispColor(uint32_t color)
{
	uint32_t i,j;
	BlockWrite(0, COL - 1, 0, ROW - 1);
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

void DispPic(uint16_t x,uint16_t y,uint16_t w, uint16_t h,const uint8_t *p)
{
	uint32_t i,j;
	uint8_t pic_h,pic_l;

	LCD_WR_REG(0x36); //Set Scanning Direction
	LCD_WR_DATA(0xC8); //0xc8

	BlockWrite(x, x + w - 1, y, y + h - 1);
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

void LCD_SetPixel(uint16_t  x,uint16_t  y,uint16_t  color)
{
	BlockWrite(x, x, y, y);
	LCD_CS = 0;
	LCD_RS = 1;
	SendDataSPI(color>>8);
	SendDataSPI(color);
	LCD_CS = 1;
}

// x: 0-7
// y: 0-7
void fill_select_region(uint8_t x, uint8_t y, uint16_t color)
{
	uint8_t x_start, y_start;
	if (x >= 8) {return;}
	if (y >= 8) {return;}

	x_start = x * 16;
	y_start = y * 16;

	BlockWrite(x_start, x_start + 16 -1, y_start, y_start + 16 -1);

	LCD_CS = 0;
	LCD_RS = 1;
	for (int i = 0; i < 16 * 16; i++) {
		SendDataSPI(color>>8);
		SendDataSPI(color);
	}
	LCD_CS = 1;
}

//low range of the sensor (this will be blue on the screen)
#define MINTEMP (10.0f)

//high range of the sensor (this will be red on the screen)
#define MAXTEMP (80.0f)


void show_map(float (*buffer)[8], uint16_t length)
{
	uint8_t i, j, colorIndex;
	float colorTemp, Temputer;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			colorTemp = buffer[7 - i][7 - j];
			if (colorTemp >= MAXTEMP) {
				Temputer = MAXTEMP;
			} else if (colorTemp <= MINTEMP) {
				Temputer = MINTEMP;
			} else {
				Temputer = colorTemp;
			}
			colorIndex = (Temputer - MINTEMP) * 3.5f;
			fill_select_region(j, i, camColors[colorIndex]);
		}
	}
}







