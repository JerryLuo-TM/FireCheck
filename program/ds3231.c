#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "ds3231.h"

/******************************************************************************
定义相关的变量函数
******************************************************************************/
DS3231_DateTime g_DS3231_Time;	//定义时间数据指针

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

	IIC_Start();					//重启总线
	IIC_Send_Byte(DS3231_Read_ADD);	//发送读取命令
	IIC_Wait_Ack();

	ReData = IIC_Read_Byte(0);	//读取数据 非应答信号结束

	IIC_Stop();

	return ReData;
}

void DS3231_Operate_Register(uint8_t REG_ADD, uint8_t *pBuff, uint8_t num, uint8_t mode)
{
	uint8_t i;
	if(mode) {	//读取数据
		IIC_Start();

		IIC_Send_Byte(DS3231_Write_ADD);
		IIC_Wait_Ack(); //发送写命令并检查应答位

		IIC_Send_Byte(REG_ADD);	//定位起始寄存器地址
		IIC_Wait_Ack(); //发送写命令并检查应答位

		IIC_Start();	//重启总线
		IIC_Send_Byte(DS3231_Read_ADD);	//发送读取命令
		IIC_Wait_Ack(); //发送写命令并检查应答位

		for(i = 0; i < num; i++) {
			if(i == (num - 1))	{
				*pBuff++ = IIC_Read_Byte(0);	//读取数据 发送非应答信号
			} else {
				*pBuff++ = IIC_Read_Byte(1);	//读取数据 发送应答信号
			}
		}
		IIC_Stop();
	} else {	//写入数据
		IIC_Start();

		IIC_Send_Byte(DS3231_Write_ADD);
		IIC_Wait_Ack(); //发送写命令并检查应答位

		IIC_Send_Byte(REG_ADD);	//定位起始寄存器地址
		IIC_Wait_Ack(); //发送写命令并检查应答位

		for(i = 0;i < num; i++) {
			IIC_Send_Byte(*pBuff++);	//写入数据
			IIC_Wait_Ack(); //发送写命令并检查应答位
		}

		IIC_Stop();
	}
}

uint8_t DS3231_Check(void)
{
	if (DS3231_Read_Byte(Address_control_status) & 0x80) {  //晶振停止工作了
		return 1;  //异常
	} else if (DS3231_Read_Byte(Address_control) & 0x80) {  //或者 EOSC被禁止了
		return 1;  //异常
	}

	return 0;  //正常
}

void DS3231_SetDateTime(DS3231_DateTime *TimeVAL)
{
	uint8_t Time_Buffer[7];

	if(TimeVAL == NULL) {
		return ;
	}

	//时间日历数据
	Time_Buffer[0] = DS3231_DecToBCD(TimeVAL->Seconds);		//秒
	Time_Buffer[1] = DS3231_DecToBCD(TimeVAL->Minutes);		//分钟
	Time_Buffer[2] = DS3231_DecToBCD(TimeVAL->Hour);		//小时
	Time_Buffer[3] = DS3231_DecToBCD(TimeVAL->Week);		//星期
	Time_Buffer[4] = DS3231_DecToBCD(TimeVAL->Date);		//日
	Time_Buffer[5] = DS3231_DecToBCD(TimeVAL->Month);		//月
	Time_Buffer[6] = DS3231_DecToBCD(TimeVAL->Year);		//年

	debug_printf("\r\n");
	for (int i=0;i<7;i++) {
		debug_printf("%x ", Time_Buffer[i]);
	}
	debug_printf("\r\n");

	DS3231_Operate_Register(Address_second, Time_Buffer, 7, 0);	//从秒（0x00）开始写入7组数据

	DS3231_Write_Byte(Address_control, OSC_Enable);
	DS3231_Write_Byte(Address_control_status, Clear_OSF_Flag);
}

void DS3231_GetDateTime(DS3231_DateTime *TimeVAL)
{
	uint8_t Time_Register[8];	//定义时间缓存

	if(TimeVAL == NULL) {
		return ;
	}

	DS3231_Operate_Register(Address_second, Time_Register, 7, 1);	//从秒地址（0x00）开始读取时间日历数据
	g_DS3231_Time.Seconds = DS3231_BCDToDec(Time_Register[0]);	//秒数据
	g_DS3231_Time.Minutes = DS3231_BCDToDec(Time_Register[1]);	//分钟数据
	g_DS3231_Time.Hour    = DS3231_BCDToDec(Time_Register[2]);  //小时数据
	g_DS3231_Time.Week    = DS3231_BCDToDec(Time_Register[3]);  //星期数据
	g_DS3231_Time.Date    = DS3231_BCDToDec(Time_Register[4]);  //日数据
	g_DS3231_Time.Month   = DS3231_BCDToDec(Time_Register[5]);	//月数据
	g_DS3231_Time.Year    = DS3231_BCDToDec(Time_Register[6]);  //年数据

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

	temph = DS3231_Read_Byte(Address_temp_MSB);	//读取温度高8bits
	templ = DS3231_Read_Byte(Address_temp_LSB) >> 6;	//读取温度低2bits

	//温度值转换
	if(temph & 0x80) {	//判断温度值的正负
		//负温度值
		temph = ~temph;	//高位取反
		templ = ~templ + 0x01;	//低位取反加1
	}

	return (float)((float)temph + (float)templ * 0.25f);
}

void Time_Handle(void)
{
	float temputer;
	DS3231_DateTime ds3231_time;

	DS3231_GetDateTime(&ds3231_time);

	temputer = DS3231_Read_Temp();

	debug_printf("20%02d/%d/%d  %d:%d:%d w:%d temp:%0.2f ℃ \r\n",
			ds3231_time.Year,
			ds3231_time.Month,
			ds3231_time.Date,
			ds3231_time.Hour,
			ds3231_time.Minutes,
			ds3231_time.Seconds,
			ds3231_time.Week,
			temputer);

}


