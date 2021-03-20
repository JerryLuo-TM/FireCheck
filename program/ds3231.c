#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "ds3231.h"
#include "AT24CXX.h"

int second_last;
int sec_flag;

/******************************************************************************
* @ File name --> ds3231.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 02 - 01 - 2014
* @ Brief     --> �߾���ʼ��оƬDS3231��������
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.
* @ Author    -->
* @ Date      -->
* @ Revise    -->
*
******************************************************************************/

/******************************************************************************
						������ʾʱ���ʽ
					Ҫ�ı���ʾ�ĸ�ʽ���޸Ĵ�����
******************************************************************************/

uint8_t Display_Time[8] = {0x30,0x30,0x3a,0x30,0x30,0x3a,0x30,0x30};
					//ʱ����ʾ����   ��ʽ  00:00:00

uint8_t Display_Date[13] = {0x32,0x30,0x31,0x33,0x2f,0x31,0x30,0x2f,0x32,0x30,0x20,0x37,0x57};
					//������ʾ����   ��ʽ  2013/10/20 7W

/******************************************************************************
������صı�������
******************************************************************************/

Time_Typedef TimeValue;	//����ʱ������ָ��


uint8_t Time_Buffer[7];	//ʱ���������ݻ���


/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ���д��һ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
*                   dat��Ҫд�������
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Write_Byte(uint8_t REG_ADD,uint8_t dat)
{
	IIC_Start();

	IIC_Send_Byte(DS3231_Write_ADD);
	IIC_Wait_Ack(); //����д������Ӧ��λ

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(dat);	//��������
	IIC_Wait_Ack();

	IIC_Stop();
}
/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ�����ȡһ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
* Output        --> none
* Reaturn       --> ��ȡ���ļĴ�������ֵ
******************************************************************************/
uint8_t DS3231_Read_Byte(uint8_t REG_ADD)
{
	uint8_t ReData;
	IIC_Start();

	IIC_Send_Byte(DS3231_Write_ADD);
	IIC_Wait_Ack(); //����д������Ӧ��λ

	IIC_Send_Byte(REG_ADD);	//ȷ��Ҫ�����ļĴ���
	IIC_Wait_Ack(); //����д������Ӧ��λ

	IIC_Start();	//��������
	IIC_Send_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
	IIC_Wait_Ack(); //����д������Ӧ��λ

	ReData = IIC_Read_Byte(0);	//��ȡ���� ���ͷ�Ӧ���źŽ������ݴ���

	IIC_Stop();

	return ReData;
}
/******************************************************************************
* Function Name --> DS3231��ʱ�������Ĵ���������д�����ݻ��߶�ȡ����
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
*                   *WBuff��д�����ݻ���
*                   num��д����������
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Operate_Register(uint8_t REG_ADD,uint8_t *pBuff,uint8_t num,uint8_t mode)
{
	uint8_t i;
	if(mode) {	//��ȡ����
		IIC_Start();

		IIC_Send_Byte(DS3231_Write_ADD);
		IIC_Wait_Ack(); //����д������Ӧ��λ

		IIC_Send_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
		IIC_Wait_Ack(); //����д������Ӧ��λ

		IIC_Start();	//��������
		IIC_Send_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
		IIC_Wait_Ack(); //����д������Ӧ��λ

		for(i = 0; i < num; i++) {
			if(i == (num - 1))	{
				*pBuff++ = IIC_Read_Byte(0);	//��ȡ���� ���ͷ�Ӧ���ź�
			} else {
				*pBuff++ = IIC_Read_Byte(1);	//��ȡ���� ����Ӧ���ź�
			}
		}
		IIC_Stop();
	} else {	//д������
		IIC_Start();

		IIC_Send_Byte(DS3231_Write_ADD);
		IIC_Wait_Ack(); //����д������Ӧ��λ

		IIC_Send_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
		IIC_Wait_Ack(); //����д������Ӧ��λ

		for(i = 0;i < num; i++) {
			IIC_Send_Byte(*pBuff++);	//д������
			IIC_Wait_Ack(); //����д������Ӧ��λ
		}

		IIC_Stop();
	}
}
/******************************************************************************
* Function Name --> DS3231��ȡ����д��ʱ����Ϣ
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> *pBuff��д�����ݻ���
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_ReadWrite_Time(uint8_t mode)
{
	uint8_t Time_Register[8];	//����ʱ�仺��

	if(mode) {	//��ȡʱ����Ϣ

		DS3231_Operate_Register(Address_second, Time_Register, 7, 1);	//�����ַ��0x00����ʼ��ȡʱ����������
		// for (uint32_t i = 0; i < 7; i++) {
		// 	Time_Register[i] = DS3231_Read_Byte(i);
		// }

		/******�����ݸ��Ƶ�ʱ��ṹ���У��������������******/
		TimeValue.second = Time_Register[0] & Shield_secondBit;	//������
		if(TimeValue.second != second_last) {
			sec_flag = 1;
			second_last = TimeValue.second;
		} else {
			sec_flag=0;
		}
		debug_printf("%x/%x/%x  %x:%x:%x %d \r\n", 
				Time_Register[6],
				Time_Register[5],
				Time_Register[4],
				Time_Register[2],
				Time_Register[1],
				Time_Register[0],
				Time_Register[3]);
		TimeValue.minute = Time_Register[1] & Shield_minuteBit;	//��������
		TimeValue.hour   = Time_Register[2] & Shield_hourBit;	//Сʱ����
		TimeValue.week   = Time_Register[3] & Shield_weekBit;	//��������
		TimeValue.date   = Time_Register[4] & Shield_dateBit;	//������
		TimeValue.month  = Time_Register[5] & Shield_monthBit;	//������
		TimeValue.year   = Time_Register[6] | DS3231_YEARDATA;	//������
	} else {
		/******��ʱ��ṹ���и������ݽ���******/
		Time_Register[0] = TimeValue.second;	//��
		Time_Register[1] = TimeValue.minute;	//����
		Time_Register[2] = TimeValue.hour | Hour_Mode24;	//Сʱ
		Time_Register[3] = TimeValue.week;	//����
		Time_Register[4] = TimeValue.date;	//��
		Time_Register[5] = TimeValue.month;	//��
		Time_Register[6] = (uint8_t)TimeValue.year;	//��
		
		for (uint32_t i = 0; i < 7; i++) {
			DS3231_Write_Byte(i, Time_Register[0]);
		}
		// DS3231_Operate_Register(Address_second,Time_Register,7,0);	//�����ַ��0x00����ʼд��ʱ����������
	}
}
/******************************************************************************
* Function Name --> ʱ��������ʼ��
* Description   --> none
* Input         --> *TimeVAL��RTCоƬ�Ĵ���ֵָ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Time_Init(Time_Typedef *TimeVAL)
{
	//ʱ����������
	Time_Buffer[0] = TimeVAL->second;
	Time_Buffer[1] = TimeVAL->minute;
	Time_Buffer[2] = TimeVAL->hour;
	Time_Buffer[3] = TimeVAL->week;
	Time_Buffer[4] = TimeVAL->date;
	Time_Buffer[5] = TimeVAL->month;
	Time_Buffer[6] = (uint8_t)TimeVAL->year;
	DS3231_Operate_Register(Address_second,Time_Buffer,7,0);	//���루0x00����ʼд��7������
	DS3231_Write_Byte(Address_control, OSC_Enable);
	DS3231_Write_Byte(Address_control_status, Clear_OSF_Flag);
}
/******************************************************************************
* Function Name --> DS3231��⺯��
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> 0: ����
*                   1: ������������Ҫ��ʼ��ʱ����Ϣ
******************************************************************************/
uint8_t DS3231_Check(void)
{
	if(DS3231_Read_Byte(Address_control_status) & 0x80)  //����ֹͣ������
	{
		return 1;  //�쳣
	}
	else if(DS3231_Read_Byte(Address_control) & 0x80)  //���� EOSC����ֹ��
	{
		return 1;  //�쳣
	}
	else	return 0;  //����
}
/******************************************************************************
* Function Name --> ʱ���������ݴ�����
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void Time_Handle(void)
{
	uint32_t temp;
	/******************************************************
	��ȡʱ��������Ϣ
	******************************************************/

	DS3231_ReadWrite_Time(1);	//��ȡʱ����������

	/******************************************************
	ʱ����Ϣת��ΪASCII������ַ�
	******************************************************/

	Display_Time[6] = (TimeValue.second >> 4) + 0x30;
	Display_Time[7] = (TimeValue.second & 0x0f) + 0x30;	//Second

	Display_Time[3] = (TimeValue.minute >> 4) + 0x30;
	Display_Time[4] = (TimeValue.minute & 0x0f) + 0x30;	//Minute

	Display_Time[0] = (TimeValue.hour >> 4) + 0x30;
	Display_Time[1] = (TimeValue.hour & 0x0f) + 0x30;	//Hour

	Display_Date[8] = (TimeValue.date >> 4) + 0x30;
	Display_Date[9] = (TimeValue.date & 0x0f) + 0x30;	//Date

	Display_Date[5] = (TimeValue.month >> 4) + 0x30;
	Display_Date[6] = (TimeValue.month & 0x0f) + 0x30;	//Month

	Display_Date[0] = (uint8_t)(TimeValue.year >> 12) + 0x30;
	Display_Date[1] = (uint8_t)((TimeValue.year & 0x0f00) >> 8) + 0x30;
	Display_Date[2] = (TimeValue.year >> 4) + 0x30;
	Display_Date[3] = (TimeValue.year & 0x0f) + 0x30;	//Year

	Display_Date[11] = (TimeValue.week & 0x0f) + 0x30;	//week

	debug_printf("%x/%x/%x  %x:%x:%x %d \r\n", 
			TimeValue.year,
			TimeValue.month,
			TimeValue.date,
			TimeValue.hour,
			TimeValue.minute,
			TimeValue.second,
			TimeValue.week);

	DS3231_Read_Temp((uint8_t*)&temp);
}
/******************************************************************************
* Function Name --> ��ȡоƬ�¶ȼĴ���
* Description   --> �¶ȼĴ�����ַΪ0x11��0x12�������Ĵ���Ϊֻ��
* Input         --> none
* Output        --> *Temp�������¶���ʾ�ַ�����
* Reaturn       --> none
******************************************************************************/
void DS3231_Read_Temp(uint8_t *Temp)
{
	uint8_t temph,templ;
	float temp_dec;

	temph = DS3231_Read_Byte(Address_temp_MSB);	//��ȡ�¶ȸ�8bits
	templ = DS3231_Read_Byte(Address_temp_LSB) >> 6;	//��ȡ�¶ȵ�2bits

	//�¶�ֵת��
	if(temph & 0x80)	//�ж��¶�ֵ������
	{	//���¶�ֵ
		temph = ~temph;	//��λȡ��
		templ = ~templ + 0x01;	//��λȡ����1
		Temp[0] = 0x2d;	//��ʾ��-��
	}
	else	Temp[0] = 0x20;	//���¶Ȳ���ʾ���ţ���ʾ������0x2b

	//С�����ּ��㴦��
	temp_dec = (float)templ * (float)0.25;	//0.25��ֱ���

	debug_printf("%x, %x temp:%d \r\n\r\n", temph, templ, temp_dec);
	// //�������ּ��㴦��
	// temph = temph & 0x70;	//ȥ������λ
	// Temp[1] = temph % 1000 / 100 + 0x30;	//��λ
	// Temp[2] = temph % 100 / 10 + 0x30;	//ʮλ
	// Temp[3] = temph % 10 + 0x30;	//��λ
	// Temp[4] = 0x2e;	//.

	// //С�����ִ���
	// Temp[5] = (uint8_t)(temp_dec * 10) + 0x30;	//С�����һλ
	// Temp[6] = (uint8_t)(temp_dec * 100) % 10 + 0x30;	//С������λ

	// if(Temp[1] == 0x30)	Temp[1] = 0x20;	//��λΪ0ʱ����ʾ
	// if(Temp[2] == 0x30)	Temp[2] = 0x20;	//ʮλΪ0ʱ����ʾ
}


