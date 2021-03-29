#ifndef __IIC_H
#define __IIC_H

#include "stdio.h"
#include "sys.h"

#define SDA_IN()  {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=(u32)8<<12;}
#define SDA_OUT() {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=(u32)3<<12;}

#define IIC_SCL    PAout(2) //SCL
#define IIC_SDA    PAout(3) //SDA
#define READ_SDA   PAin(3)  //input SDA

#define IIC_INT    PAin(4)  //input INT


void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif


