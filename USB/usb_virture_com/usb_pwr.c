/*
*********************************************************************************************************
*	                                  
*	模块名称 : STM32 USB设备电源管理模块    
*	文件名称 : usb_hw.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : USB设备的电源管理模块
*	修改记录 :
*		版本号  日期       作者    说明
*		v0.1    2009-12-27 armfly  创建该文件，ST固件库版本为V3.1.2
*		v1.0    2011-01-11 armfly  ST固件库升级到V3.4.0版本。
*		v2.0    2011-10-16 armfly  ST固件库升级到V3.5.0版本。优化工程结构。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "usb_hw.h"

__IO uint32_t bDeviceState = UNCONNECTED; 	/* USB 设备状态 */
__IO bool fSuspendEnabled = true;  			    /* true when suspend is possible */

struct
{
	__IO RESUME_STATE eState;				/* RESUME状态自，表示USB由挂机到恢复的状态 */
	__IO uint8_t bESOFcnt;
}ResumeS;

/*
*********************************************************************************************************
*	函 数 名: usb_PowerOn
*	功能说明: USB设备上电复位
*	形    参: 无
*	返 回 值: USB_SUCCESS
*********************************************************************************************************
*/
RESULT usb_PowerOn(void)
{
	uint16_t wRegVal;
	
	/* 这个函数实质上将USB连接控制线设置为高电平，然后USB集线器就可以检测到设备了 */
	usb_CableConfig(ENABLE);
	
	/* 当集线器报告设备连接状态，并收到主机指令后，会复位USB总线，这需要一定的时间.
	这段时间内设备应该准备好处理复位指令） */
	
	/* 使能USB模块的电源。因为上电复位时，CNTR寄存器的断电控制为PDWN位是1，模块是断电的  */
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);	/* 这是一个宏，wRegVal赋值给CNTR寄存器，此时所有的中断被屏蔽 */
	
	wInterrupt_Mask = 0;		/* 全局变量在 wInterrupt_Maskusb_init.c 中定义。表示中断屏蔽配置 */
	_SetCNTR(wInterrupt_Mask);	/* wInterrupt_Mask = 0， 屏蔽所有的USB中断 */
	
	_SetISTR(0);			/* 清除可能存在的USB中断请求标志 */
	
	/* 
		使能需要的USB中断
		CNTR_RESETM 表示使能复位中断，此时集线器多半还未开始复位端口，或者或稍等片刻就会复位USB设备
		CNTR_SUSPM 表示使能USB设备挂起中断
		CNTR_SUSPM 表示使能USB设备唤醒中断
	*/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);
	
	return USB_SUCCESS;	/* 固定返回 USB_SUCCESS */
}

/*
*********************************************************************************************************
*	函 数 名: usb_PowerOff
*	功能说明: USB设备断电
*	形    参: 无
*	返 回 值: USB_SUCCESS
*********************************************************************************************************
*/
RESULT usb_PowerOff()
{
	/* 禁止所有的中断，并且强制USB复位 */
	_SetCNTR(CNTR_FRES);
	
	/* 清零中断状态寄存器 */
	_SetISTR(0);
	
	/* 断开USB D+线的上拉电阻，此时USB集线器会认为设备已拔除 */
	usb_CableConfig(DISABLE);
	
	/* 关闭USB设备电源 */
	_SetCNTR(CNTR_FRES + CNTR_PDWN);
	
	/* 可以在此处添加语句，复位一些变量 */
	
	return USB_SUCCESS;
}

/*
*********************************************************************************************************
*	函 数 名: usb_Suspend
*	功能说明: 强制USB设备挂起，进入省电模式
*	形    参: 无
*	返 回 值: USB_SUCCESS
*********************************************************************************************************
*/
void usb_Suspend(void)
{
	uint16_t wCNTR;
	
	/* 可以在此处添加USB挂起的准备工作 */
	/* ... */
	
	/* 强制USB设备进入挂起状态 */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* ------------------ 下面的部分仅用于USB总线供电的设备 ---------------------- */
	
	/* 强制进入低功耗模式 */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);
	
	/* 为降低功耗，可以在此处切换CPU主时钟 */
	
	/* 
		设置表示USB状态的全局变量，并且根据需要关闭一些中断和电源模式。 
		这个函数多数由用户自己添加相关的代码。在 usb_hw.c 文件 
	*/
	usb_EnterLowPowerMode();	
}

/*
*********************************************************************************************************
*	函 数 名: usb_ResumeInit
*	功能说明: USB设备唤醒时的回调函数。 在USB中断服务程序中被掉用。完成唤醒后的初始化工作。
*	形    参: 无
*	返 回 值: USB_SUCCESS
*********************************************************************************************************
*/
void usb_ResumeInit(void)
{
	uint16_t wCNTR;
	
	/* ------------------ 仅用于USB总线供电的设备 ---------------------- */
	
	/* 此处可添加恢复时钟的代码 */
	
	/* CNTR_LPMODE = 0， 退出低功耗模式 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);
	
	/* 退出低功耗模式，恢复系统时钟。设置必要的全局变量 */
	usb_LeaveLowPowerMode();
	
	/* 复位 FSUSP 位 */
	_SetCNTR(IMR_MSK);	/* IMR_MSK宏在usb_conf.h中定义。*/
	/* #define IMR_MSK (CNTR_CTRM  | CNTR_SOFM  | CNTR_RESETM ) */
	
	/* 此处可添加挂起准备的反操作代码 */
}

/*
*********************************************************************************************************
*	函 数 名: usb_Resume
*	功能说明: USB退出挂模式时，会调用该函数。这个是一个状态机，处理
*	形    参: eResumeSetVal  结构体变量
*	返 回 值: USB_SUCCESS
*********************************************************************************************************
*/
void usb_Resume(RESUME_STATE eResumeSetVal)
{
	uint16_t wCNTR;
	
	if (eResumeSetVal != RESUME_ESOF)
	{
		ResumeS.eState = eResumeSetVal;
	}
	
	switch (ResumeS.eState)
	{
		case RESUME_EXTERNAL:
			usb_ResumeInit();
			ResumeS.eState = RESUME_OFF;
			break;
			
		case RESUME_INTERNAL:
			usb_ResumeInit();
			ResumeS.eState = RESUME_START;
			break;
			
		case RESUME_LATER:
			ResumeS.bESOFcnt = 2;
			ResumeS.eState = RESUME_WAIT;
			break;
			
		case RESUME_WAIT:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0)
			ResumeS.eState = RESUME_START;
			break;
			
		case RESUME_START:
			wCNTR = _GetCNTR();
			wCNTR |= CNTR_RESUME;
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_ON;
			ResumeS.bESOFcnt = 10;
			break;
			
		case RESUME_ON:
			ResumeS.bESOFcnt--;
			if (ResumeS.bESOFcnt == 0)
			{
				wCNTR = _GetCNTR();
				wCNTR &= (~CNTR_RESUME);
				_SetCNTR(wCNTR);
				ResumeS.eState = RESUME_OFF;
			}
			break;
			
		case RESUME_OFF:
		case RESUME_ESOF:
		default:
			ResumeS.eState = RESUME_OFF;
			break;
	}
}
