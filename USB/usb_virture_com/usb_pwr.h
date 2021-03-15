/*
*********************************************************************************************************
*	                                  
*	模块名称 : STM32 USB设备电源管理模块    
*	文件名称 : usb_hw.h
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __USB_PWR_H
#define __USB_PWR_H

#include "usb_lib.h"
#include "stdbool.h"

typedef enum _RESUME_STATE
{
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
}RESUME_STATE;

typedef enum _DEVICE_STATE
{
	UNCONNECTED,
	ATTACHED,
	POWERED,
	SUSPENDED,
	ADDRESSED,
	CONFIGURED
} DEVICE_STATE;

void usb_Suspend(void);
void usb_ResumeInit(void);
void usb_Resume(RESUME_STATE eResumeSetVal);
RESULT usb_PowerOn(void);
RESULT usb_PowerOff(void);

extern  __IO uint32_t bDeviceState;		/* USB设备状态 */
extern __IO bool fSuspendEnabled;		/* true 表示允许挂起 */

#endif
