#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "spi.h"
#include "tft.h"
#include "spi.h"

#include "font.h"
#include "tft.h"

#define ROW  128			//显示行数
#define COL  128			//显示列数

//画笔颜色,背景颜色 WHITE
uint16_t POINT_COLOR = WHITE, BACK_COLOR = BLACK;
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
	/* FRMCTR1 (B1h): Frame Rate Control (In normal mode/ Full colors) */
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	/* FRMCTR2 (B2h): Frame Rate Control (In Idle mode/ 8-colors) */
	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	/* FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors) */
	LCD_WR_REG(0xB3);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x36);

	//------------------------------------End ST7735S Frame Rate-----------------------------------------//
	LCD_WR_REG(0xB4); //Dot inversion
	LCD_WR_DATA(0x00); //03

	/* PWCTR1 (C0h): Power Control 1 */
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0xa2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x84);

	/* PWCTR2 (C1h): Power Control 2 */
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0XC5);

	/* PWCTR3 (C2h): Power Control 3 (in Normal mode/ Full colors) */
	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x00);

	/* PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors) */
	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x8D);
	LCD_WR_DATA(0x2A);

	/* PWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors) */
	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x8D);
	LCD_WR_DATA(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//

	/* VMCTR1 (C5h): VCOM Control 1 */
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

#if 1
	/* 水平翻转 */
	x = 127 - Xstart;
	x1 = 127 - Xend;

	LCD_WR_REG(0x2a);
	LCD_WR_DATA(x1>>8);
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x>>8);
	LCD_WR_DATA(x);
#else
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(Xstart>>8);
	LCD_WR_DATA(Xstart);
	LCD_WR_DATA(Xend>>8);
	LCD_WR_DATA(Xend);
#endif

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
	(void)colorIndex;
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

//显示一个字符 16 32
void LCD_ShowChar(uint32_t x, uint32_t y, uint8_t num, uint8_t size)
{
	uint8_t i, j;
	uint8_t temp;
	uint8_t csize = (size / 8 + ((size % 8) ? 1:0)) * (size / 2);//得到字体一个字符对应点阵集所占的字节数

	//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	num = num - ' ';
	//选择范围
	if (size==16)     {LCD_BlockWrite(x,x+8-1,y, y+16-1); }
	else if(size==32) {LCD_BlockWrite(x,x+16-1,y,y+32-1); }
	else {return;}

	LCD_CS = 0;
	LCD_RS = 1;
	//刷入数据
	if (size == 16) {
		for(i = 0; i < csize; i++) {
			temp = asc2_1608[num][i]; //调用1608字体
			for(j = 0; j < 8; j++) {
				if(((temp)&(1<<j)) != 0) {
					SendDataSPI(POINT_COLOR>>8);
					SendDataSPI(POINT_COLOR);
				} else {
					SendDataSPI(BACK_COLOR>>8);
					SendDataSPI(BACK_COLOR);
				}
			}
		}
	} else if (size == 32){
		for(i = 0; i < 32; i++) {
			temp = asc2_3216[num][32 + i];//调用3216字体
			for(j = 0; j < 8; j++) {
				if(((temp)&(1<<j)) != 0) {
					SendDataSPI(POINT_COLOR>>8);
					SendDataSPI(POINT_COLOR);
				} else {
					SendDataSPI(BACK_COLOR>>8);
					SendDataSPI(BACK_COLOR);
				}
			}
			temp = asc2_3216[num][i];//调用3216字体
			for(j = 0; j < 8; j++) {
				if(((temp)&(1<<j)) != 0) {
					SendDataSPI(POINT_COLOR>>8);
					SendDataSPI(POINT_COLOR);
				} else {
					SendDataSPI(BACK_COLOR>>8);
					SendDataSPI(BACK_COLOR);
				}
			}
		}
	}
	LCD_CS = 1;
}

//显示字符串
void LCD_ShowStr(uint32_t x,uint32_t y,uint8_t *p,uint8_t size)
{
	//判断是不是非法字符!
    while (*p != '\0') {
        LCD_ShowChar(x, y, *p++, size);
        x += size / 2;
    }
}



