#ifndef __AT24CXX_H
#define __AT24CXX_H

#include "stdio.h"
#include "sys.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

#define EE_TYPE AT24C32

uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);//指定地址开始写入指定长度的数据
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);   	//从指定地址开始读出指定长度的数据

uint8_t AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void); //初始化IIC

void AT24CXX_Read_Byte_Len(uint16_t ReadAddr, uint8_t *buf, uint16_t len);

#endif


