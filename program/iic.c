#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


//I2C_1 init
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//PA2 SCL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);

	//PA3 SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA = 1;
	IIC_SCL = 1;
	delay_us(1);
	IIC_SDA = 0;	//START:when CLK is high,DATA change form high to low
	delay_us(1);
	IIC_SCL = 0;
}

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL = 0;
	IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
	delay_us(1);
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(1);
}


//return：1，receive ack fail
//        0，receive ack ok
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();
	IIC_SDA = 1; delay_us(1);
	IIC_SCL = 1; delay_us(1);
	while(READ_SDA) {
		ucErrTime++;
		if(ucErrTime > 250) {
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;
	return 0;
}

void IIC_Ack(void)
{
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

//1，ack
//0，no ack
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
	SDA_OUT();
    IIC_SCL = 0;
    for(t=0; t<8; t++)
    {
		if (((txd&0x80)>>7) != 0) {
			IIC_SDA = 1;
		} else {
			IIC_SDA = 0;
		}
        txd<<=1;
		delay_us(4);
		IIC_SCL = 1;
		delay_us(4);
		IIC_SCL = 0;
		delay_us(4);
    }
}

uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive=0;
	IIC_SDA = 1;
	SDA_IN();
    for(i = 0; i < 8; i++) {
        delay_us(4);
		IIC_SCL = 1;
        delay_us(4);
        receive <<= 1;
        if(READ_SDA == 0) {
			;
		} else {
			receive++;
		}
		IIC_SCL = 0;
		delay_us(4);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}


