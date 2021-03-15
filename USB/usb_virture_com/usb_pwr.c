/*
*********************************************************************************************************
*	                                  
*	ģ������ : STM32 USB�豸��Դ����ģ��    
*	�ļ����� : usb_hw.c
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� : USB�豸�ĵ�Դ����ģ��
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*		v2.0    2011-10-16 armfly  ST�̼���������V3.5.0�汾���Ż����̽ṹ��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "usb_hw.h"

__IO uint32_t bDeviceState = UNCONNECTED; 	/* USB �豸״̬ */
__IO bool fSuspendEnabled = true;  			    /* true when suspend is possible */

struct
{
	__IO RESUME_STATE eState;				/* RESUME״̬�ԣ���ʾUSB�ɹһ����ָ���״̬ */
	__IO uint8_t bESOFcnt;
}ResumeS;

/*
*********************************************************************************************************
*	�� �� ��: usb_PowerOn
*	����˵��: USB�豸�ϵ縴λ
*	��    ��: ��
*	�� �� ֵ: USB_SUCCESS
*********************************************************************************************************
*/
RESULT usb_PowerOn(void)
{
	uint16_t wRegVal;
	
	/* �������ʵ���Ͻ�USB���ӿ���������Ϊ�ߵ�ƽ��Ȼ��USB�������Ϳ��Լ�⵽�豸�� */
	usb_CableConfig(ENABLE);
	
	/* �������������豸����״̬�����յ�����ָ��󣬻ḴλUSB���ߣ�����Ҫһ����ʱ��.
	���ʱ�����豸Ӧ��׼���ô���λָ� */
	
	/* ʹ��USBģ��ĵ�Դ����Ϊ�ϵ縴λʱ��CNTR�Ĵ����Ķϵ����ΪPDWNλ��1��ģ���Ƕϵ��  */
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);	/* ����һ���꣬wRegVal��ֵ��CNTR�Ĵ�������ʱ���е��жϱ����� */
	
	wInterrupt_Mask = 0;		/* ȫ�ֱ����� wInterrupt_Maskusb_init.c �ж��塣��ʾ�ж��������� */
	_SetCNTR(wInterrupt_Mask);	/* wInterrupt_Mask = 0�� �������е�USB�ж� */
	
	_SetISTR(0);			/* ������ܴ��ڵ�USB�ж������־ */
	
	/* 
		ʹ����Ҫ��USB�ж�
		CNTR_RESETM ��ʾʹ�ܸ�λ�жϣ���ʱ��������뻹δ��ʼ��λ�˿ڣ����߻��Ե�Ƭ�̾ͻḴλUSB�豸
		CNTR_SUSPM ��ʾʹ��USB�豸�����ж�
		CNTR_SUSPM ��ʾʹ��USB�豸�����ж�
	*/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);
	
	return USB_SUCCESS;	/* �̶����� USB_SUCCESS */
}

/*
*********************************************************************************************************
*	�� �� ��: usb_PowerOff
*	����˵��: USB�豸�ϵ�
*	��    ��: ��
*	�� �� ֵ: USB_SUCCESS
*********************************************************************************************************
*/
RESULT usb_PowerOff()
{
	/* ��ֹ���е��жϣ�����ǿ��USB��λ */
	_SetCNTR(CNTR_FRES);
	
	/* �����ж�״̬�Ĵ��� */
	_SetISTR(0);
	
	/* �Ͽ�USB D+�ߵ��������裬��ʱUSB����������Ϊ�豸�Ѱγ� */
	usb_CableConfig(DISABLE);
	
	/* �ر�USB�豸��Դ */
	_SetCNTR(CNTR_FRES + CNTR_PDWN);
	
	/* �����ڴ˴������䣬��λһЩ���� */
	
	return USB_SUCCESS;
}

/*
*********************************************************************************************************
*	�� �� ��: usb_Suspend
*	����˵��: ǿ��USB�豸���𣬽���ʡ��ģʽ
*	��    ��: ��
*	�� �� ֵ: USB_SUCCESS
*********************************************************************************************************
*/
void usb_Suspend(void)
{
	uint16_t wCNTR;
	
	/* �����ڴ˴����USB�����׼������ */
	/* ... */
	
	/* ǿ��USB�豸�������״̬ */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* ------------------ ����Ĳ��ֽ�����USB���߹�����豸 ---------------------- */
	
	/* ǿ�ƽ���͹���ģʽ */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);
	
	/* Ϊ���͹��ģ������ڴ˴��л�CPU��ʱ�� */
	
	/* 
		���ñ�ʾUSB״̬��ȫ�ֱ��������Ҹ�����Ҫ�ر�һЩ�жϺ͵�Դģʽ�� 
		��������������û��Լ������صĴ��롣�� usb_hw.c �ļ� 
	*/
	usb_EnterLowPowerMode();	
}

/*
*********************************************************************************************************
*	�� �� ��: usb_ResumeInit
*	����˵��: USB�豸����ʱ�Ļص������� ��USB�жϷ�������б����á���ɻ��Ѻ�ĳ�ʼ��������
*	��    ��: ��
*	�� �� ֵ: USB_SUCCESS
*********************************************************************************************************
*/
void usb_ResumeInit(void)
{
	uint16_t wCNTR;
	
	/* ------------------ ������USB���߹�����豸 ---------------------- */
	
	/* �˴�����ӻָ�ʱ�ӵĴ��� */
	
	/* CNTR_LPMODE = 0�� �˳��͹���ģʽ */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);
	
	/* �˳��͹���ģʽ���ָ�ϵͳʱ�ӡ����ñ�Ҫ��ȫ�ֱ��� */
	usb_LeaveLowPowerMode();
	
	/* ��λ FSUSP λ */
	_SetCNTR(IMR_MSK);	/* IMR_MSK����usb_conf.h�ж��塣*/
	/* #define IMR_MSK (CNTR_CTRM  | CNTR_SOFM  | CNTR_RESETM ) */
	
	/* �˴�����ӹ���׼���ķ��������� */
}

/*
*********************************************************************************************************
*	�� �� ��: usb_Resume
*	����˵��: USB�˳���ģʽʱ������øú����������һ��״̬��������
*	��    ��: eResumeSetVal  �ṹ�����
*	�� �� ֵ: USB_SUCCESS
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
