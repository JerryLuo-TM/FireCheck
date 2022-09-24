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

	/* 0x80 ~ 0xFF 0x81(H) 0x80(L) */
}

float SignedMag12ToFloat(uint16_t val)
{
	//take first 11 bits as absolute val
	uint16_t absVal = (val & 0x7FF);

	return (val & 0x800) ? 0 - (float)absVal : (float)absVal ;
}

uint16_t To_HSB(uint8_t num){
	uint8_t R=0,G=0,B=0;
	uint8_t ColrModeBuf;
	float a;
	uint32_t b;

	//判断部分伪彩色
	// ColrModeBuf = 1;	// 1
	ColrModeBuf = 2;	// 2
	// ColrModeBuf = 3;		// 3

	switch (ColrModeBuf) {
		case 1:{
			a=0.7*num;
			a+=20;
			num=(uint8_t)a;
			if (num < 64) {
				B = (unsigned char)(num * 4);
				G = 0;
				R = 0;
			}
			else if (num < 96) {
				B = 255;
				G = 0;
				R = (unsigned char)(4 * (num - 64));
			}
			else if (num < 128) {
				B = (unsigned char)(256 - 8 * (num - 95));
				G = 0;
				R = (unsigned char)(4 * (num - 64) - 1);
			}
			else if (num < 191) {
				B = 0;
				G = (unsigned char)(4 * (num - 128));
				R = 255;
			}
			else {
				B = (unsigned char)(4 * (num - 191));
				G = 255;
				R = 255;
			}
			break;
		}

		case 2:{
			b = 240*num;
			b /= 255;
			num = (uint8_t)b;
			if (num < 60) {
				B = 255;
				G = num*4;
				R = 0;
			}
			else if (num < 120) {
				B = (120-num)*4;
				G = 255;
				R = 0;
			}
			else if (num < 180) {
				B = 0;
				G = 255;
				R = (num-119)*4;
			}else{
				B = 0;
				G = (240-num)*4;
				R = 255;
			}
			break;
		}

		case 3:{
			R=G=B=num;
			break;
		}

		default:
			R=G=B=num;

	}
	// RRRRR GGGGG+G BBBBB
	return 0xFFFF&((B&0xf8)>>3|(G&0xfC)<<3|(R&0xf8)<<8);
}

void AMG8833_get_Pixels(int16_t (*buffer)[8])
{
	uint8_t raw_array[128];
	uint8_t i;
	int16_t raw_temp;

	/* 连续读取 */
	AMG8833_Read_Buf_Len(0x80, raw_array, 128);

	/* 转换十六位数据 */
	for(i = 0; i < 64; i++) {
		raw_temp = ((uint16_t)raw_array[(i * 2) + 1] << 8) | ((uint16_t)raw_array[i * 2]);
		if ((raw_temp & 0x800) == 0x800) {
			raw_temp = ~raw_temp;
			raw_temp &= 0x0FFF;
			raw_temp += 1;
			raw_temp = 0x0 - raw_temp;
		} else if (raw_temp > 0x200) {
			raw_temp = 0x200;
		}
		buffer[i%8][7 - i/8] = raw_temp;
	}
}

void data_push(int16_t (*buffer)[8])
{
	uint8_t i;
	ext[0] = buffer[0][0];
	ext[1] = buffer[0][0];
	ext_add[0] = 0;
	ext_add[1] = 0;

	for(i = 0; i < 64; i++) {
		if(buffer[i/8][i%8] > ext[0]) {	//遍历最大值
			ext[0] = buffer[i/8][i%8];
			ext_add[0] = i;
		}
		if(buffer[i/8][i%8] < ext[1]) { //遍历最小值
			ext[1] = buffer[i/8][i%8];
			ext_add[1] = i;
		}
#if (Size == SIZEx5)
		data[PixLg-1-(i / 8 * PixGain + 2)][i % 8 * PixGain + 2] = buffer[i/8][i%8];//数据填充
#elif (Size == SIZEx8)
		data[PixLg-1-(i / 8 * PixGain + 1)][i % 8 * PixGain + 1] = buffer[i/8][i%8];
#endif
	}
	// debug_printf("ext[0]=%0.2f ext[1]=%0.2f ext_add[0]=(%d,%d) ext_add[1]=(%d,%d) \r\n",
	// 											(float)ext[0] * 0.25, (float)ext[1] * 0.25f,
	// 											ext_add[0]/8, ext_add[0]%8,
	// 											ext_add[1]/8, ext_add[1]%8);
}

void get_img(void)
{
	uint16_t i;
	long diff = ext[0] - ext[1] + 2;
	if(diff<20)	diff = 20;
	for(i=0;i<PixLg*PixLg;i++){
		data[i/PixLg][i%PixLg]=To_HSB(0xff&((data[i/PixLg][i%PixLg]-ext[1]+1)*0xff/diff));
	}
}

#if (Size == SIZEx5)

void AMG8833_draw_Img(void)
{
	uint16_t i;
	for (i = 0; i < PixLg*PixLg; i++) {
		LCD_ColorBox(2+i/PixLg*2, 2+i%PixLg*2, 2, 2, data[i/PixLg][i%PixLg]);
	}
}

void blowup(void)
{
	int i;
	for (i = 0; i < 8 * 7; i++) {
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 1] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t4 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t1;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 2] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t3 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t2;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 3] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t2 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t3;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 4] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t1 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t4;
	}
	for (i = 0; i < 7 * 36; i++) {
		data[i % 7 * 5 + 2 + 1][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t4 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t1;
		data[i % 7 * 5 + 2 + 2][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t3 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t2;
		data[i % 7 * 5 + 2 + 3][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t2 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t3;
		data[i % 7 * 5 + 2 + 4][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t1 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t4;
	}
	for (i = 0; i < 36; i++) {
		data[0][i + 2] = data[1][i + 2] = data[2][i + 2];
		data[39][i + 2] = data[38][i + 2] = data[37][i + 2];
	}
	for (i = 0; i < 40; i++) {
		data[i][0] = data[i][1] = data[i][2];
		data[i][39] = data[i][38] = data[i][37];
	}
	ext[2]=data[19][19];
}
#elif (Size == SIZEx8)

void AMG8833_draw_Img(void) {
	uint16_t i;
	for (i = 0; i < PixLg*PixLg; i++){
		LCD_ColorBox(1 + i/PixLg * 2, 1 + i%PixLg*2, 2, 2, data[i/PixLg][i%PixLg]);
	}
}

void blowup(void)
{
	int i;
	for (i = 0; i < 8 * 7; i++) {
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 1] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t7 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t1;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 2] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t6 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t2;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 3] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t5 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t3;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 4] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t4 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t4;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 5] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t3 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t5;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 6] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t2 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t6;
		data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 7] = 1+ data[i / 7 * 8 + 1][i % 7 * 8 + 1] * t1 + data[i / 7 * 8 + 1][i % 7 * 8 + 1 + 8] * t7;
	}
	for (i = 0; i < 7 * 57; i++) {
		data[i % 7 * 8 + 1 + 1][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t7 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t1;
		data[i % 7 * 8 + 1 + 2][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t6 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t2;
		data[i % 7 * 8 + 1 + 3][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t5 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t3;
		data[i % 7 * 8 + 1 + 4][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t4 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t4;
		data[i % 7 * 8 + 1 + 5][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t3 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t5;
		data[i % 7 * 8 + 1 + 6][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t2 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t6;
		data[i % 7 * 8 + 1 + 7][i / 7 + 1] = 1+ data[i % 7 * 8 + 1][i / 7 + 1] * t1 + data[i % 7 * 8 + 1 + 8][i / 7 + 1] * t7;
	}
	for (i = 0; i < 57; i++) {
		data[0][i + 1] = data[1][i + 1];
		data[58][i + 1] = data[57][i + 1];
	}
	for (i = 0; i < 59; i++) {
		data[i][0] = data[i][1];
		data[i][58] = data[i][57];
	}
	ext[2]=data[29][29];
}
#endif

void AMG8833_get_Img(void)
{
	data_push(PriData);	//数据转移
	blowup();			//插值
	get_img();			//插值转换为rgb图片
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



