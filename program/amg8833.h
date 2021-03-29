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

#define Draw_Size SIZEx8


void AMG8833_Write_Byte(uint8_t REG_ADD, uint8_t dat);
uint8_t AMG8833_Read_Byte(uint8_t REG_ADD);
void AMG8833_Write_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);
void AMG8833_Read_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);

void AMG8833_Init(void);
void AMG8833_ShutDown(void);
float SignedMag12ToFloat(uint16_t val);
void AMG8833_ReadPixels(float *buf, uint8_t size);


#endif


