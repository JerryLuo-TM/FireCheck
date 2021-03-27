#ifndef __IIC_H
#define __IIC_H

#include "stdio.h"
#include "sys.h"

//IO方向设置
#define SDA_IN()  {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=(u32)8<<12;}
#define SDA_OUT() {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=(u32)3<<12;}

//IO操作函数
#define IIC_SCL    PAout(2) //SCL
#define IIC_SDA    PAout(3) //SDA
#define READ_SDA   PAin(3)  //输入SDA

#define IIC_INT    PAin(4)  //输入INT

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif


