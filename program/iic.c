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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��

	//PB6 SCL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_6); 	//PB6 �����

	//PB7 SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_7); 	//PB7 �����
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();	//sda�����
	IIC_SDA = 1;
	IIC_SCL = 1;
	delay_us(4);
	IIC_SDA = 0;	//START:when CLK is high,DATA change form high to low
	delay_us(4);
	IIC_SCL = 0;	//ǯסI2C���ߣ�׼�����ͻ��������
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();	//sda�����
	IIC_SCL = 0;
	IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	// IIC_SCL = 1; // ??? 24c32�����������
	IIC_SDA = 1;//����I2C���߽����ź�
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();      //SDA����Ϊ����
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

//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 1;
	delay_us(4);
	IIC_SCL = 0;
}

//������ACKӦ��
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(4);
	IIC_SCL = 1;
	delay_us(4);
	IIC_SCL = 0;
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
	SDA_OUT();
    IIC_SCL = 0; //����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
		if (((txd&0x80)>>7) != 0) {
			IIC_SDA = 1;
		} else {
			IIC_SDA = 0;
		}
        txd<<=1;
		delay_us(4); //��TEA5767��������ʱ���Ǳ����
		IIC_SCL = 1;
		delay_us(4);
		IIC_SCL = 0;
		delay_us(4);
    }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive=0;
	SDA_IN(); //SDA����Ϊ����
    for(i=0; i<8; i++) {
        IIC_SCL = 0;
        delay_us(4);
		IIC_SCL = 1;
        receive <<= 1;
        if(READ_SDA == 0) {
			;
		} else {
			receive++;
		}
		delay_us(4);
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}


