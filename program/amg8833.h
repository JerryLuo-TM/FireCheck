#ifndef __AMG8833_H
#define __AMG8833_H

#include "stdio.h"
#include "sys.h"

/******************************************************************************
                                define
******************************************************************************/
#define AMG8833_ADD_BASS		0xD2	//address
#define AMG8833_Write_ADD		(AMG8833_ADD_BASS | 0x00)	//write
#define AMG8833_Read_ADD		(AMG8833_ADD_BASS | 0x01)	//read

#define SIZEx5 5
#define SIZEx8 8  

#define Size SIZEx5     //插值数量选择

#if (Size == SIZEx5)

	#define PixLg 40   //軟件分辨率
	#define PixGain 5    //軟件分辨率

	#define t1 1000/5000     //gain
	#define t2 2000/5000
	#define t3 3000/5000
	#define t4 4000/5000

#elif (Size == SIZEx8)

	#define PixLg 59   //軟件分辨率
	#define PixGain 8    //軟件分辨率

	#define t1 1000/8000     //gain
	#define t2 2000/8000
	#define t3 3000/8000
	#define t4 4000/8000
	#define t5 5000/8000
	#define t6 6000/8000
	#define t7 7000/8000

#endif

extern int16_t PriData[8][8];
extern long data[PixLg][PixLg];
extern int16_t ext[3];
extern uint8_t  ext_add[2];


void AMG8833_Write_Byte(uint8_t REG_ADD, uint8_t dat);
uint8_t AMG8833_Read_Byte(uint8_t REG_ADD);
void AMG8833_Write_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);
void AMG8833_Read_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);

void AMG8833_Init(void);
float SignedMag12ToFloat(uint16_t val);
void AMG8833_ReadPixels(float *buf, uint8_t size);


void AMG8833_get_Pixels(int16_t (*buffer)[8]);
void AMG8833_get_Img(void);
void AMG8833_draw_Img(void);

#endif


