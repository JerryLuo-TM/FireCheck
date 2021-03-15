/*
*********************************************************************************************************
*	                                  
*	模块名称 : STM32 USB硬件配置模块    
*	文件名称 : usb_hw.h
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 :  头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "usb_type.h"

#define USB_TX_BUF_SIZE		2048		/* 设备->PC，发送缓冲区大小 */

typedef struct
{
	unsigned char aTxBuf[USB_TX_BUF_SIZE];	/* 发送缓冲区, 设备->PC */
	
	unsigned short usTxRead;					/* 发送缓冲区读指针 */
	unsigned short usTxWrite;					/* 发送缓冲区写指针 */	
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
