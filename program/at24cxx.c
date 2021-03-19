#include "Struct.h"
#include "sys.h"
#include "iic.h"
#include "AT24CXX.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


#define AT24CXX_ADDRESS 0xA0

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp=0;
    IIC_Start();
	if(EE_TYPE > AT24C16) {
		IIC_Send_Byte(AT24CXX_ADDRESS); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		IIC_Wait_Ack();
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((ReadAddr/256)<<1)); //����������ַ0XA0,д����
	}

	// IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24CXX_ADDRESS|0x1); //�������ģʽ
	IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();//����һ��ֹͣ����

	return temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    IIC_Start();
	if(EE_TYPE > AT24C16) {
		IIC_Send_Byte(AT24CXX_ADDRESS);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((WriteAddr/256)<<1));   //����������ַ0XA0,д����
	}
	IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);     //�����ֽ�
	IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
	delay_ms(10);
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{
	uint8_t t;
	for(t=0;t<Len;t++) {
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{
	uint8_t t;
	uint32_t temp=0;
	for(t = 0; t < Len; t++) {
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
	}
	return temp;
}

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX
	if(temp == 0X45) {
		return 0;
	} else { //�ų���һ�γ�ʼ�������
		AT24CXX_WriteOneByte(255,0X45);
		temp=AT24CXX_ReadOneByte(255);
		if(temp==0X45) {
			return 0;
		}
	}
	return 1;
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead) {
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--) {
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void AT24CXX_Read_Byte_Len(uint16_t ReadAddr, uint8_t *buf, uint16_t len)
{
	uint16_t i;

	IIC_Start();

	if(EE_TYPE > AT24C16) {
		IIC_Send_Byte(AT24CXX_ADDRESS); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		IIC_Wait_Ack();
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((ReadAddr/256)<<1)); //����������ַ0XA0,д����
	}

	IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(AT24CXX_ADDRESS|0x1); //�������ģʽ
	IIC_Wait_Ack();

	for(i=0; i<len; i++) {
		if(i == (len-1)) {
			*buf++=IIC_Read_Byte(0);
		} else {
			*buf++=IIC_Read_Byte(1);
		}
	}
	IIC_Stop();
}
