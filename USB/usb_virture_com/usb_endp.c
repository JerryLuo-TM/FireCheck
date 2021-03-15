/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB端点事务处理模块    
*	文件名称 : usb_istr.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : USB端点事务处理子程序
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

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "usb_hw.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/*
*********************************************************************************************************
*	函 数 名: EP1_IN_Callback
*	功能说明: 端点1 IN包（设备->PC）回调函数
*	形    参: 无
*	返 回 值: 无
//EP_TX_NAK    发送完成
//EP_TX_VALID  发送未完成
*********************************************************************************************************
*/
void EP1_IN_Callback (void)
{
	uint16_t i;
	uint16_t usWord;
	uint8_t ucByteNum;
	uint16_t *pdwVal;
	uint16_t usTotalSize;
	
	usTotalSize = 0;
	pdwVal = (uint16_t *)(ENDP1_TXADDR * 2 + PMAAddr);	
	for (i = 0 ; i < VIRTUAL_COM_PORT_DATA_SIZE / 2; i++)
	{
		usWord = usb_GetTxWord(&ucByteNum);
		if (ucByteNum == 0)
		{
			break;
		}
		
		usTotalSize += ucByteNum;
		
		*pdwVal++ = usWord;
		pdwVal++;		
	}
	
	if (usTotalSize == 0)
	{
		return;
	}
	
	SetEPTxCount(ENDP1, usTotalSize);
	SetEPTxValid(ENDP1); 
	//while(GetEPTxStatus(ENDP1)!=EPTX_STAT);//wait for send finished
	
//	if(usTotalSize==64)
//	{
//	  SetEPTxCount(ENDP1, 0);
//	  SetEPTxValid(ENDP1); 
//	}
//	while(GetEPTxStatus(ENDP1)!=EP_TX_NAK);
}

/*
*********************************************************************************************************
*	函 数 名: EP3_OUT_Callback
*	功能说明: 端点3 OUT包（PC->设备）回调函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void EP3_OUT_Callback(void)
{
	uint16_t usRxCnt;
	uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
	
	/* 将USB端点3收到的数据存储到USB_Rx_Buffer， 数据大小保存在USB_Rx_Cnt */
	usRxCnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
	
	/* 立即将接收到的数据缓存到内存 */
	usb_SaveHostDataToBuf(USB_Rx_Buffer, usRxCnt);
	
	/* 允许 EP3 端点接收数据 */
	SetEPRxValid(ENDP3);
}

/*
*********************************************************************************************************
*	函 数 名: SOF_Callback
*	功能说明: SOF回调函数  .SOF是host用来指示frame的开头的。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SOF_Callback(void)
{
	static uint32_t FrameCount = 0;
	
	if (bDeviceState == CONFIGURED)
	{
		if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
		{
			/* Reset the frame counter */
			FrameCount = 0;
			
			/* Check the data to be sent through IN pipe */
			EP1_IN_Callback();
			//Handle_USBAsynchXfer();
		}
	}  
}

