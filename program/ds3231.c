#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "ds3231.h"

/******************************************************************************
������صı�������
******************************************************************************/
DS3231_DateTime g_DS3231_Time;	//����ʱ������ָ��

void DS3231_Write_Byte(uint8_t REG_ADD, uint8_t dat)
{
	IIC_Start();

	IIC_Send_Byte(DS3231_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(dat);
	IIC_Wait_Ack();

	IIC_Stop();
}

uint8_t DS3231_Read_Byte(uint8_t REG_ADD)
{
	uint8_t ReData;
	IIC_Start();

	IIC_Send_Byte(DS3231_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Start();					//��������
	IIC_Send_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
	IIC_Wait_Ack();

	ReData = IIC_Read_Byte(0);	//��ȡ���� ��Ӧ���źŽ���

	IIC_Stop();

	return ReData;
}

void DS3231_Operate_Register(uint8_t REG_ADD, uint8_t *pBuff, uint8_t num, uint8_t mode)
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

uint8_t DS3231_Check(void)
{
	if (DS3231_Read_Byte(Address_control_status) & 0x80) {  //����ֹͣ������
		return 1;  //�쳣
	} else if (DS3231_Read_Byte(Address_control) & 0x80) {  //���� EOSC����ֹ��
		return 1;  //�쳣
	}

	return 0;  //����
}

void DS3231_SetDateTime(DS3231_DateTime *TimeVAL)
{
	uint8_t Time_Buffer[7];

	if(TimeVAL == NULL) {
		return ;
	}

	//ʱ����������
	Time_Buffer[0] = DS3231_DecToBCD(TimeVAL->Seconds);		//��
	Time_Buffer[1] = DS3231_DecToBCD(TimeVAL->Minutes);		//����
	Time_Buffer[2] = DS3231_DecToBCD(TimeVAL->Hour);		//Сʱ
	Time_Buffer[3] = DS3231_DecToBCD(TimeVAL->Week);		//����
	Time_Buffer[4] = DS3231_DecToBCD(TimeVAL->Date);		//��
	Time_Buffer[5] = DS3231_DecToBCD(TimeVAL->Month);		//��
	Time_Buffer[6] = DS3231_DecToBCD(TimeVAL->Year);		//��

	debug_printf("\r\n");
	for (int i=0;i<7;i++) {
		debug_printf("%x ", Time_Buffer[i]);
	}
	debug_printf("\r\n");

	DS3231_Operate_Register(Address_second, Time_Buffer, 7, 0);	//���루0x00����ʼд��7������

	DS3231_Write_Byte(Address_control, OSC_Enable);
	DS3231_Write_Byte(Address_control_status, Clear_OSF_Flag);
}

void DS3231_GetDateTime(DS3231_DateTime *TimeVAL)
{
	uint8_t Time_Register[8];	//����ʱ�仺��

	if(TimeVAL == NULL) {
		return ;
	}

	DS3231_Operate_Register(Address_second, Time_Register, 7, 1);	//�����ַ��0x00����ʼ��ȡʱ����������
	g_DS3231_Time.Seconds = DS3231_BCDToDec(Time_Register[0]);	//������
	g_DS3231_Time.Minutes = DS3231_BCDToDec(Time_Register[1]);	//��������
	g_DS3231_Time.Hour    = DS3231_BCDToDec(Time_Register[2]);  //Сʱ����
	g_DS3231_Time.Week    = DS3231_BCDToDec(Time_Register[3]);  //��������
	g_DS3231_Time.Date    = DS3231_BCDToDec(Time_Register[4]);  //������
	g_DS3231_Time.Month   = DS3231_BCDToDec(Time_Register[5]);	//������
	g_DS3231_Time.Year    = DS3231_BCDToDec(Time_Register[6]);  //������

	memcpy((uint8_t*)TimeVAL, (uint8_t*)&g_DS3231_Time, sizeof(DS3231_DateTime));
}

void DS3231_SetAlarm1(DS3231_Alarm1 *DS3231_WriteAlarm1)
{
	DS3231_Write_Byte(Address_second_Alarm1, DS3231_DecToBCD(DS3231_WriteAlarm1->Seconds));// send second
	DS3231_Write_Byte(Address_minute_Alarm1, DS3231_DecToBCD(DS3231_WriteAlarm1->Minutes));// send Minutes
	DS3231_Write_Byte(Address_hour_Alarm1,	 DS3231_DecToBCD(DS3231_WriteAlarm1->Hour));// send Hour
	DS3231_Write_Byte(Address_week_Alarm1,   DS3231_DecToBCD(DS3231_WriteAlarm1->DY_DT));// send Day
	// DS3231_Write_Byte(Address_week_Alarm1,   DS3231_DecToBCD(DS3231_WriteAlarm1->Date));// send Date
}

void DS3231_GetAlarm1(DS3231_Alarm1 *DS3231_ReadAlarm1)
{
	DS3231_ReadAlarm1->Seconds = DS3231_BCDToDec(DS3231_Read_Byte(Address_second_Alarm1) & 0x7F);// receive seconds
	DS3231_ReadAlarm1->Minutes = DS3231_BCDToDec(DS3231_Read_Byte(Address_minute_Alarm1) & 0x7F);// receive Month
	DS3231_ReadAlarm1->Hour = DS3231_BCDToDec(DS3231_Read_Byte(Address_hour_Alarm1) & 0x3F);// receive Hour
	DS3231_ReadAlarm1->DY_DT = DS3231_BCDToDec(DS3231_Read_Byte(Address_week_Alarm1) & 0x3F);// receive Day or Date
	//DS3231_ReadAlarm1->Date = DS3231_BCDToDec(DS3231_Read_Byte(Address_week_Alarm1) & 0x7F);// receive Date
}

void DS3231_SetAlarm2(DS3231_Alarm2 *DS3231_WriteAlarm2)
{
	DS3231_Write_Byte(Address_minute_Alarm2, DS3231_DecToBCD(DS3231_WriteAlarm2->Minutes));// send Minutes
	DS3231_Write_Byte(Address_hour_Alarm2, DS3231_DecToBCD(DS3231_WriteAlarm2->Hour));// send Hour
	DS3231_Write_Byte(Address_week_Alarm2, DS3231_DecToBCD(DS3231_WriteAlarm2->DY_DT));// send Day
	//DS3231_Write_Byte(Address_week_Alarm2, DS3231_DecToBCD(DS3231_WriteAlarm2->Date));// send Date
}

void DS3231_GetAlarm2(DS3231_Alarm2 *DS3231_ReadAlarm2)
{
	DS3231_ReadAlarm2->Minutes = DS3231_BCDToDec(DS3231_Read_Byte(Address_minute_Alarm2) & 0x7F);// receive Month
	DS3231_ReadAlarm2->Hour = DS3231_BCDToDec(DS3231_Read_Byte(Address_hour_Alarm2) & 0x3F);// receive Hour
	DS3231_ReadAlarm2->DY_DT = DS3231_BCDToDec(DS3231_Read_Byte(Address_week_Alarm2) & 0x3F);// receive Day or date
	//DS3231_ReadAlarm2->Date = DS3231_BCDToDec(DS3231_Read_Byte(Address_week_Alarm2) & 0x7F);// receive Date
}

uint8_t DS3231_GetFlag(uint8_t DS3231_FLAG_STATUS)
{
	uint8_t calculate = DS3231_Read_Byte(Address_control_status) & DS3231_FLAG_STATUS;// read Flag status

	return calculate;
}

void DS3231_ClearFlag(uint8_t DS3231_FLAG_STATUS)
{
	DS3231_Write_Byte(Address_control_status, ~DS3231_FLAG_STATUS);// send FLAG bit for clear bit
}

uint8_t DS3231_GetCotrolRegister(void)
{
	uint8_t readCtrlReg = DS3231_Read_Byte(Address_control);// read control register

	return readCtrlReg;
}

void DS3231_SetCtrlRegister(uint8_t DS3231_FLAG_CONTROL)
{
	DS3231_FLAG_CONTROL = DS3231_GetCotrolRegister() | DS3231_FLAG_CONTROL;

	DS3231_Write_Byte(Address_control, DS3231_FLAG_CONTROL);// send data for control register
}

uint8_t DS3231_DecToBCD(uint8_t value)
{
	return ((value / 10) << 4) + (value % 10);// convert decimal to BCD
}

uint8_t DS3231_BCDToDec(uint8_t value)
{
	return ((value >> 4) * 10) + (value & 0x0F);// convert BCD(Binary Coded Decimal) to Decimal
}

float DS3231_Read_Temp(void)
{
	uint8_t temph,templ;

	temph = DS3231_Read_Byte(Address_temp_MSB);	//��ȡ�¶ȸ�8bits
	templ = DS3231_Read_Byte(Address_temp_LSB) >> 6;	//��ȡ�¶ȵ�2bits

	//�¶�ֵת��
	if(temph & 0x80) {	//�ж��¶�ֵ������
		//���¶�ֵ
		temph = ~temph;	//��λȡ��
		templ = ~templ + 0x01;	//��λȡ����1
	}

	return (float)((float)temph + (float)templ * 0.25f);
}

void Time_Handle(void)
{
	float temputer;
	DS3231_DateTime ds3231_time;

	DS3231_GetDateTime(&ds3231_time);

	temputer = DS3231_Read_Temp();

	debug_printf("20%02d/%d/%d  %d:%d:%d w:%d temp:%0.2f �� \r\n",
			ds3231_time.Year,
			ds3231_time.Month,
			ds3231_time.Date,
			ds3231_time.Hour,
			ds3231_time.Minutes,
			ds3231_time.Seconds,
			ds3231_time.Week,
			temputer);

}


