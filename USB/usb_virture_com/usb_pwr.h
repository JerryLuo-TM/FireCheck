/*
*********************************************************************************************************
*	                                  
*	ģ������ : STM32 USB�豸��Դ����ģ��    
*	�ļ����� : usb_hw.h
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
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

extern  __IO uint32_t bDeviceState;		/* USB�豸״̬ */
extern __IO bool fSuspendEnabled;		/* true ��ʾ������� */

#endif
