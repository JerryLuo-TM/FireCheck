#ifndef __AMG8833_H
#define __AMG8833_H

#include "stdio.h"
#include "sys.h"

/******************************************************************************
                                参数宏定义
******************************************************************************/
#define AMG8833_ADD_BASS		0xD2	//器件基地址
#define AMG8833_Write_ADD		(AMG8833_ADD_BASS | 0x00)	//写
#define AMG8833_Read_ADD		(AMG8833_ADD_BASS | 0x01)	//读

#define SIZEx5 5
#define SIZEx8 8  //插值数量选择

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


