#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "amg8833.h"

void AMG8833_Write_Byte(uint8_t REG_ADD, uint8_t dat)
{
	IIC_Start();

	IIC_Send_Byte(AMG8833_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(dat);
	IIC_Wait_Ack();

	IIC_Stop();
}

uint8_t AMG8833_Read_Byte(uint8_t REG_ADD)
{
	uint8_t ReData;
	IIC_Start();

	IIC_Send_Byte(AMG8833_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(AMG8833_Read_ADD);
	IIC_Wait_Ack();

	ReData = IIC_Read_Byte(0);

	IIC_Stop();

	return ReData;
}

void AMG8833_Write_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len)
{
	uint32_t i;

	IIC_Start();

	IIC_Send_Byte(AMG8833_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	for(i = 0;i < len; i++) {
		IIC_Send_Byte(*pBuff++);
		IIC_Wait_Ack();
	}

	IIC_Stop();
}

void AMG8833_Read_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len)
{
	uint32_t i;

	IIC_Start();

	IIC_Send_Byte(AMG8833_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(AMG8833_Read_ADD);
	IIC_Wait_Ack();

	for(i = 0; i < len; i++) {
		if(i == (len - 1))	{
			*pBuff++ = IIC_Read_Byte(0);
		} else {
			*pBuff++ = IIC_Read_Byte(1);
		}
	}
	IIC_Stop();
}


void AMG8833_Init(void)
{
	IIC_Init();

	/* system mode
		0x00 normal
		0x10 sleep
	*/
	AMG8833_Write_Byte(0x00, 0x00);  // normal
	/* reset reg */
	AMG8833_Write_Byte(0x01, 0x3F);  // reset
	/* config freq 0: 10fps 1: 1fps */
	AMG8833_Write_Byte(0x02, 0x00);  // 10 fps
	/* config interrupt */
	// AMG8833_Write_Byte(0x03, 0x00);

	debug_printf("address[2]:%d\r\n",AMG8833_Read_Byte(0x02));

	/* 0x80 ~ 0xFF 0x81(H) 0x80(L) */
}

void AMG8833_ShutDown(void)
{
	AMG8833_Write_Byte(0x00, 0x10);
}


float SignedMag12ToFloat(uint16_t val)
{
	//take first 11 bits as absolute val
	uint16_t absVal = (val & 0x7FF);

	return (val & 0x800) ? 0 - (float)absVal : (float)absVal ;
}

void AMG8833_ReadPixels(float *buf, uint8_t size)
{
	uint16_t recast;
	float converted;
	uint8_t rawArray[128], i;

	AMG8833_Read_Buf_Len(0x80, rawArray, 128);

	for(i = 0; i < size; i++) {
		recast = ((uint16_t)rawArray[(i * 2) + 1] << 8) | ((uint16_t)rawArray[i * 2]);
		converted = SignedMag12ToFloat(recast) * 0.25f;
		buf[i] = converted;
	}
}


// void Draw_data(void){
// 	int max = (int)(ext[0])*10/4;
// 	int min = (int)(ext[1])*10/4;
// 	int mid = (int)(ext[2])*10/4;

// #if (Draw_Size == SIZEx5)
// 	if(SysState.DispMeas == Midd) {            //mid
// 		Draw_Tab(61,99,Black);
// 		Lcd_ColorBox(72,82,24,35,White);
// 		Draw_Num(75,84,1,mid);
// 	}else if(SysState.DispMeas == Exts) {
// // 		Lcd_ColorBox(10+(7-ext_add[0]/8)*15,46+(ext_add[0]%8)*15,3,3,Black);
// // 		Lcd_ColorBox(10+(7-ext_add[1]/8)*15,46+(ext_add[1]%8)*15,3,3,White);
// 		Draw_Tab(10 + (7 - ext_add[0] / 8) * 15, 47 + (ext_add[0] % 8) * 15, Black);
// 		Draw_Tab(10 + (7 - ext_add[1] / 8) * 15, 47 + (ext_add[1] % 8) * 15, White);
// 	}
// #elif (Draw_Size == SIZEx8)
// 	if(SysState.DispMeas == Midd) {            //mid
// 		Draw_Tab(61,99,Black);
// 		Lcd_ColorBox(72,82,24,35,White);
// 		Draw_Num(75,84,1,mid);
// 	}else if(SysState.DispMeas == Exts) {
// // 		Lcd_ColorBox(7+(7-ext_add[0]/8)*16,43+(ext_add[0]%8)*16,2,2,Black);
// // 		Lcd_ColorBox(7+(7-ext_add[1]/8)*16,43+(ext_add[1]%8)*16,2,2,White);
// 		Draw_Tab(7+(7-ext_add[0]/8)*16,44+(ext_add[0]%8)*16,Black);
// 		Draw_Tab(7+(7-ext_add[1]/8)*16,44+(ext_add[1]%8)*16,White);
// 	}

// #endif

// 	Draw_Num(0, 0, 0, max);
// 	Draw_Num(110, 0, 0, min);
// }




