#include "Struct.h"
#include "sys.h"
#include "iic.h"
#include "AT24CXX.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


/* t WR Write Cycle Time 20 10 10 ms */
/* EEPROM �ֲ���������ÿ��д�����ݽ�������һ��д��ʱ�䣬���ڼ��ڽ�ֹ���з��ʲ��� */

/* The 32K/64K EEPROM is capable of 32-byte page writes */
/* EEPROM �ֲ���������һ��֧������д���ַ32�ֽڶ��룬����С��32�ֽڵ����ݣ��������Ƚ�ѭ��ҳ */

#define AT24CXX_ADDRESS 0xA0

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp = AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX
	if(temp == 0X45) {
		return 0;
	} else { //�ų���һ�γ�ʼ�������
		AT24CXX_WriteOneByte(255,0X45);
		temp = AT24CXX_ReadOneByte(255);
		if(temp == 0X45) {
			return 0;
		}
	}
	return 1;
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
		IIC_Send_Byte(ReadAddr >> 8); //���͸ߵ�ַ
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((ReadAddr/256) << 1)); //����������ַ0XA0,д����
	}
	IIC_Wait_Ack();

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
		IIC_Send_Byte(WriteAddr >> 8);//���͸ߵ�ַ
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((WriteAddr/256) << 1));   //����������ַ0XA0,д����
	}
	IIC_Wait_Ack();

    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();

	IIC_Send_Byte(DataToWrite);     //�����ֽ�
	IIC_Wait_Ack();

    IIC_Stop(); //����һ��ֹͣ����

	delay_ms(10); //д�����ȴ�ʱ��
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{
	uint8_t t;
	for(t = 0; t < Len; t++) {
		AT24CXX_WriteOneByte(WriteAddr+t, (DataToWrite>>(8*t)) & 0xff);
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
		temp <<= 8;
		temp += AT24CXX_ReadOneByte(ReadAddr + Len-t-1);
	}
	return temp;
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

void AT24CXX_Write_Page(uint16_t WriteAddr, uint8_t *buf, uint16_t len)
{
	IIC_Start();

	if(EE_TYPE > AT24C16) {
		IIC_Send_Byte(AT24CXX_ADDRESS); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8); //���͸ߵ�ַ
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((WriteAddr/256)<<1)); //����������ַ0XA0,д����
	}
	IIC_Wait_Ack();

	IIC_Send_Byte(WriteAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();

	for(uint32_t i = 0; i < len; i++) {
		IIC_Send_Byte(*buf++);
		IIC_Wait_Ack();
	}
	IIC_Stop();
	delay_ms(100);
}

/* ˳���ȡ */
void AT24CXX_Read_Sequence(uint16_t ReadAddr, uint8_t *buf, uint16_t len)
{
	IIC_Start();

	if(EE_TYPE > AT24C16) {
		IIC_Send_Byte(AT24CXX_ADDRESS);
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr >> 8); //���͸ߵ�ַ
	} else {
		IIC_Send_Byte(AT24CXX_ADDRESS + ((ReadAddr/256) << 1)); //����������ַ0XA0,д����
	}
	IIC_Wait_Ack();

    IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_Wait_Ack();

	IIC_Start();

	IIC_Send_Byte(AT24CXX_ADDRESS|0x1); //�������ģʽ
	IIC_Wait_Ack();

	for(uint8_t i = 0; i < len; i++) {
		if(i == (len-1)) {
			*buf++=IIC_Read_Byte(0);
		} else {
			*buf++=IIC_Read_Byte(1);
		}
		IIC_Start();

		IIC_Send_Byte(AT24CXX_ADDRESS|0x1); //�������ģʽ
		IIC_Wait_Ack();
	}

	IIC_Stop();
}


#if 0
	// ҳ ��/д ��������
	uint8_t write_buf[256]�� read_buf[256];
	for (int i = 0; i < 256; i++) {
		write_buf[i] = i;
		read_buf[i] = 0;
	}

	AT24CXX_Write_Page(0, write_buf, 32);
	AT24CXX_Write_Page(0x20, &write_buf[32], 32);
	AT24CXX_Write_Page(0x40, &write_buf[64], 32);
	AT24CXX_Write_Page(0x60, &write_buf[96], 32);
	AT24CXX_Read_Sequence(0, read_buf, 128);

	for (int i = 0; i < 128; i++) {
		if (write_buf[i] != read_buf[i]) {
			debug_printf("%d error w:%x r:%x \r\n", i, write_buf[i], read_buf[i]);
		} else {
			debug_printf("%d pass w:%x r:%x \r\n", i, write_buf[i], read_buf[i]);
		}
	}
#endif




