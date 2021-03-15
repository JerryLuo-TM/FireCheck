/*
*********************************************************************************************************
*	                                  
*	ģ������ : STM32 USBӲ������ģ��    
*	�ļ����� : usb_hw.h
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� :  ͷ�ļ�
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "usb_type.h"

#define USB_TX_BUF_SIZE		2048		/* �豸->PC�����ͻ�������С */

typedef struct
{
	unsigned char aTxBuf[USB_TX_BUF_SIZE];	/* ���ͻ�����, �豸->PC */
	
	unsigned short usTxRead;					/* ���ͻ�������ָ�� */
	unsigned short usTxWrite;					/* ���ͻ�����дָ�� */	
}USB_COM_FIFO_T;

extern USB_COM_FIFO_T g_tUsbFifo;

void bsp_InitUsb(void);
void usb_EnterLowPowerMode(void);
void usb_LeaveLowPowerMode(void);
void usb_CableConfig(unsigned char _ucMode);
void Get_SerialNum(unsigned char *_pBuf);

void usb_SaveHostDataToBuf(unsigned char *_pInBuf, unsigned short _usLen);
unsigned short usb_GetTxWord(unsigned char *_pByteNum);
unsigned char usb_GetRxByte(unsigned char *_pByteNum);
void usb_SendDataToHost(unsigned char *_pTxBuf, unsigned short _usLen);

#endif
