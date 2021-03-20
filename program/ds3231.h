#ifndef __DS3231_H
#define __DS3231_H

#include "stdio.h"
#include "sys.h"

/******************************************************************************
                        DS3231日历时钟寄存器结构定义
******************************************************************************/

typedef struct {
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hour;
	uint8_t Week;// Range 1-7
	uint8_t Date;// Range 1-31
	uint8_t Month;
	uint8_t Year;// Range 0-99
} DS3231_DateTime;

typedef struct {
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hour;
	uint8_t DY_DT;//Day;// Range 1-7
	uint8_t Date;// Range 1-31
} DS3231_Alarm1;

typedef struct {
	uint8_t Minutes;
	uint8_t Hour;
	uint8_t DY_DT;  //Day;// Range 1-7
	uint8_t Date;   // Range 1-31
} DS3231_Alarm2;

extern DS3231_DateTime g_DS3231_Time;	//定义时间数据缓存

#define DS3231_FLAG_STATUS_A1F (1 << 0)
#define DS3231_FLAG_STATUS_A2F (1 << 1)

#define DS3231_FLAG_CONTROL_A1IE (1 << 0)
#define DS3231_FLAG_CONTROL_A2IE (1 << 1)
#define DS3231_FLAG_CONTROL_INTCN (1 << 2)
#define DS3231_FLAG_CONTROL_RS1 (1 << 3)
#define DS3231_FLAG_CONTROL_RS2 (1 << 4)
#define DS3231_FLAG_CONTROL_CONV (1 << 5)
#define DS3231_FLAG_CONTROL_BBSQW (1 << 6)
#define DS3231_FLAG_CONTROL_EOSC (1 << 7)

#define DS3231_ALARM_NO_MATCH			0x50
#define DS3231_ALARM_MATCH				0x00
#define DS3231_ALARM_DAY_MATCH			0x28// DY
#define DS3231_ALARM_DATE_MATCH			0x00// DT

/******************************************************************************
                                参数宏定义
******************************************************************************/
#define DS3231_ADD_BASS					0xD0	//器件基地址
#define DS3231_Write_ADD				(DS3231_ADD_BASS | 0x00)	//写DS3231
#define DS3231_Read_ADD					(DS3231_ADD_BASS | 0x01)	//读DS3231

/******************************************************************************
                            参数寄存器地址宏定义
******************************************************************************/

#define Address_second					0x00	//秒
#define Address_minute					0x01	//分
#define Address_hour					0x02	//时
#define Address_week					0x03	//星期
#define Address_date					0x04	//日
#define Address_month					0x05	//月
#define Address_year					0x06	//年

#define Address_second_Alarm1			0x07	//秒闹铃
#define Address_minute_Alarm1			0x08	//分闹铃
#define Address_hour_Alarm1				0x09	//时闹铃
#define Address_week_Alarm1				0x0A	//日闹铃、星期闹铃

#define Address_minute_Alarm2			0x0B	//分闹铃
#define Address_hour_Alarm2				0x0C	//时闹铃
#define Address_week_Alarm2				0x0D	//日闹铃、星期闹铃


#define Address_control					0x0E	//控制
#define Address_control_status			0x0F	//控制和状态标志

#define Address_offset					0x10	//Aging Offset

#define Address_temp_MSB				0x11	//温度高8位
#define Address_temp_LSB				0x12	//温度低8位


/******************************************************************************
                                参数命令定义
******************************************************************************/

//小时寄存器
#define Hour_Mode12					(1<<6)	//12小时格式
#define Hour_Mode24					(0<<6)	//24小时格式

//秒闹铃寄存器1
#define Alarm_second_open			(1<<7)	//秒闹铃开
#define Alarm_second_close			(0<<7)	//秒闹铃关

//分闹铃寄存器1&2
#define Alarm_minute_open			(1<<7)	//分闹铃开
#define Alarm_minute_close			(0<<7)	//分闹铃关

//时闹铃寄存器1&2（可设置为12小时模式或者24小时模式）
#define Alarm_hour_open12			(3<<6)	//时闹铃开，12小时格式
#define Alarm_hour_close12			(1<<6)	//时闹铃关，24小时格式

#define Alarm_hour_open24			(2<<6)	//时闹铃开，12小时格式
#define Alarm_hour_close24			(0<<6)	//时闹铃关，24小时格式

//星期、日闹铃寄存器1&2（可选择星期闹铃或则日期闹铃）
#define Alarm_week_open				(3<<6)	//星期闹铃开
#define Alarm_week_close			(1<<6)	//星期闹铃关

#define Alarm_date_open				(2<<6)	//星期闹铃开
#define Alarm_date_close			(0<<6)	//星期闹铃关


//晶振控制寄存器
#define OSC_Enable					(0<<7)	//启动晶振
#define OSC_Disable					(1<<7)	//停止晶振

#define SET_BBSQW					(1<<6)	//Vbat pin
#define RESET_BBSQW					(0<<6)	//int/sqw高阻

#define Temp_CONV_SET				(1<<5)	//强制温度转换位数字码
#define Temp_CONV_Clear				(0<<5)

#define SQW_OUT1Hz					(0<<3)	//1Hz
#define SQW_OUT1024Hz				(1<<3)	//1.024KHz
#define SQW_OUT4096Hz				(2<<3)	//4.096KHz
#define SQW_OUT8192Hz				(3<<3)	//8.192KHz

#define OUTPUT_INTSQW				(0<<2)	//输出方波，上电该位置1（INTCN位）

#define A2IE_Enable					(1<<1)	//enable alarm 2
#define A2IE_Disable				(0<<1)	//disable alarm 2

#define A1IE_Enable					(1<<0)	//enable alarm 1
#define A1IE_Disable				(0<<0)	//disable alarm 1

//控制、状态寄存器
#define Clear_OSF_Flag				(0<<7)	//clear OSF flag

#define Enable_OSC32768				(1<<3)	//EN32KHz EN
#define Disable_OSC32768			(0<<3)	//EN32KHz高阻

#define Clear_A2IE_Flag				(0<<1)	//清除闹铃2中断标志
#define Clear_A1IE_Flag				(0<<0)	//清除闹铃2中断标志

/******************************************************************************
                                外部功能函数
******************************************************************************/


void DS3231_Write_Byte(uint8_t REG_ADD, uint8_t dat);
uint8_t DS3231_Read_Byte(uint8_t REG_ADD);
void DS3231_Operate_Register(uint8_t REG_ADD, uint8_t *pBuff, uint8_t num, uint8_t mode);

uint8_t DS3231_Check(void);
void DS3231_SetDateTime(DS3231_DateTime *TimeVAL);
void DS3231_GetDateTime(DS3231_DateTime *TimeVAL);
void DS3231_SetAlarm1(DS3231_Alarm1 *DS3231_WriteAlarm1);
void DS3231_GetAlarm1(DS3231_Alarm1 *DS3231_ReadAlarm1);
void DS3231_SetAlarm2(DS3231_Alarm2 *DS3231_WriteAlarm2);
void DS3231_GetAlarm2(DS3231_Alarm2 *DS3231_ReadAlarm2);
uint8_t DS3231_GetFlag(uint8_t DS3231_FLAG_STATUS);
void DS3231_ClearFlag(uint8_t DS3231_FLAG_STATUS);
uint8_t DS3231_GetCotrolRegister(void);
void DS3231_SetCtrlRegister(uint8_t DS3231_FLAG_CONTROL);

uint8_t DS3231_DecToBCD(uint8_t value);
uint8_t DS3231_BCDToDec(uint8_t value);

float DS3231_Read_Temp(void);	//读取芯片温度寄存器
void Time_Handle(void);			//时间日历数据处理函数



#endif


