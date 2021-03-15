/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB中断服务程序模块    
*	文件名称 : usb_istr.h
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __USB_ISTR_H
#define __USB_ISTR_H

#include "usb_conf.h"

void USB_Istr(void);

void EP1_IN_Callback(void);
void EP2_IN_Callback(void);
void EP3_IN_Callback(void);
void EP4_IN_Callback(void);
void EP5_IN_Callback(void);
void EP6_IN_Callback(void);
void EP7_IN_Callback(void);

void EP1_OUT_Callback(void);
void EP2_OUT_Callback(void);
void EP3_OUT_Callback(void);
void EP4_OUT_Callback(void);
void EP5_OUT_Callback(void);
void EP6_OUT_Callback(void);
void EP7_OUT_Callback(void);

#ifdef CTR_CALLBACK
	void CTR_Callback(void);
#endif

#ifdef DOVR_CALLBACK
	void DOVR_Callback(void);
#endif

#ifdef ERR_CALLBACK
	void ERR_Callback(void);
#endif

#ifdef WKUP_CALLBACK
	void WKUP_Callback(void);
#endif

#ifdef SUSP_CALLBACK
	void SUSP_Callback(void);
#endif

#ifdef RESET_CALLBACK
	void RESET_Callback(void);
#endif

#ifdef SOF_CALLBACK
	void SOF_Callback(void);
#endif

#ifdef ESOF_CALLBACK
	void ESOF_Callback(void);
#endif

#endif /*__USB_ISTR_H*/
