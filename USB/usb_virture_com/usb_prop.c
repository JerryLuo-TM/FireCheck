/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB虚拟串口相关的程序
*	文件名称 : usb_istr.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : 和USB虚拟串口相关的函数集
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_hw.h"

static uint8_t s_Request = 0;

LINE_CODING linecoding =
{
	57600,  /* 波特率 */
	0x00,   /* 停止位-1 */
	0x00,   /* 检验位： 无 */
	0x08    /* 数据位：8bit */
};

DEVICE Device_Table =
{
    EP_NUM,	/* 端点个数 */
    1
};

/* 设备属性 */
DEVICE_PROP Device_Property =
{
    Virtual_Com_Port_init,
    Virtual_Com_Port_Reset,
    Virtual_Com_Port_Status_In,
    Virtual_Com_Port_Status_Out,
    Virtual_Com_Port_Data_Setup,
    Virtual_Com_Port_NoData_Setup,
    Virtual_Com_Port_Get_Interface_Setting,
    Virtual_Com_Port_GetDeviceDescriptor,
    Virtual_Com_Port_GetConfigDescriptor,
    Virtual_Com_Port_GetStringDescriptor,
    0,
    0x40 	/*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
    Virtual_Com_Port_GetConfiguration,
    Virtual_Com_Port_SetConfiguration,
    Virtual_Com_Port_GetInterface,
    Virtual_Com_Port_SetInterface,
    Virtual_Com_Port_GetStatus,
    Virtual_Com_Port_ClearFeature,
    Virtual_Com_Port_SetEndPointFeature,
    Virtual_Com_Port_SetDeviceFeature,
    Virtual_Com_Port_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
    (uint8_t*)Virtual_Com_Port_DeviceDescriptor,
    VIRTUAL_COM_PORT_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
    (uint8_t*)Virtual_Com_Port_ConfigDescriptor,
    VIRTUAL_COM_PORT_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] =
{
	{(uint8_t*)Virtual_Com_Port_StringLangID, VIRTUAL_COM_PORT_SIZ_STRING_LANGID},
	{(uint8_t*)Virtual_Com_Port_StringVendor, VIRTUAL_COM_PORT_SIZ_STRING_VENDOR},
	{(uint8_t*)Virtual_Com_Port_StringProduct, VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT},
	{(uint8_t*)Virtual_Com_Port_StringSerial, VIRTUAL_COM_PORT_SIZ_STRING_SERIAL}
};

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_init
*	功能说明: 虚拟串口初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_init(void)
{
	/* 获取设备序列号 */
	Get_SerialNum(Virtual_Com_Port_StringSerial);
	
	pInformation->Current_Configuration = 0;		/* pInformation 全局变量，在USB固件库中定义 */
	
	/* 连接USB设备。该函数会使能USB上拉电阻，并进行必要的设置 */
	usb_PowerOn();
	
	/* 执行基本的设备初始化 */
	USB_SIL_Init();
	
	bDeviceState = UNCONNECTED;		/* 更行设备状态变量 */
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_Reset
*	功能说明: 虚拟串口复位
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_Reset(void)
{
	/* 设置设备状态为：未配置 */
	pInformation->Current_Configuration = 0;
	
	/* 设置设备当前特性 */
	pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];
	
	/* 设置设备当前接口 */
	pInformation->Current_Interface = 0;
	
	SetBTABLE(BTABLE_ADDRESS);	/* 设置分组缓冲区描述报表寄存器BTABLE中的地址 */
	
	/* 初始化端点0为控制传输模式 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);
	
	/* 初始化端点1为BULK批量传输模式 */
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	
	/* 初始化端点2为中断传输模式 */
	SetEPType(ENDP2, EP_INTERRUPT);
	SetEPTxAddr(ENDP2, ENDP2_TXADDR);
	SetEPRxStatus(ENDP2, EP_RX_DIS);
	SetEPTxStatus(ENDP2, EP_TX_NAK);
	
	/* 初始化端点3为BULK批量传输模式 */
	SetEPType(ENDP3, EP_BULK);
	SetEPRxAddr(ENDP3, ENDP3_RXADDR);
	SetEPRxCount(ENDP3, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPRxStatus(ENDP3, EP_RX_VALID);
	SetEPTxStatus(ENDP3, EP_TX_DIS);
	
	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;	/* 设备已连接 */
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_SetConfiguration
*	功能说明: 更新设备状态为：已配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	
	if (pInfo->Current_Configuration != 0)
	{
		/* 设备已经配置完成 */
		bDeviceState = CONFIGURED;
	}
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_SetDeviceAddress
*	功能说明: 更新设备状态为：已寻址
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_SetDeviceAddress (void)
{
	bDeviceState = ADDRESSED;
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_Status_In
*	功能说明: Virtual COM Port Status In Routine.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_Status_In(void)
{
	if (s_Request == SET_LINE_CODING)
	{
		/* USART_Config(); */
		s_Request = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_Status_Out
*	功能说明: Virtual COM Port Status OUT Routine.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Virtual_Com_Port_Status_Out(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_Data_Setup
*	功能说明: handle the data class specific requests
*	形    参: RequestNo
*	返 回 值: USB_UNSUPPORT or USB_SUCCESS.
*********************************************************************************************************
*/
RESULT Virtual_Com_Port_Data_Setup(uint8_t RequestNo)
{
	uint8_t    *(*CopyRoutine)(uint16_t);
	
	CopyRoutine = NULL;
	
	if (RequestNo == GET_LINE_CODING)
	{
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		{
			CopyRoutine = Virtual_Com_Port_GetLineCoding;
		}
	}
	else if (RequestNo == SET_LINE_CODING)
	{
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		{
			CopyRoutine = Virtual_Com_Port_SetLineCoding;
		}
		s_Request = SET_LINE_CODING;
	}
	
	if (CopyRoutine == NULL)
	{
		return USB_UNSUPPORT;
	}
	
	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_NoData_Setup
*	功能说明:  handle the no data class specific requests.
*	形    参: RequestNo
*	返 回 值: USB_UNSUPPORT or USB_SUCCESS.
*********************************************************************************************************
*/
RESULT Virtual_Com_Port_NoData_Setup(uint8_t RequestNo)
{
	if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
	{
		if (RequestNo == SET_COMM_FEATURE)
		{
			return USB_SUCCESS;
		}
		else if (RequestNo == SET_CONTROL_LINE_STATE)
		{
			return USB_SUCCESS;
		}
	}
	
	return USB_UNSUPPORT;
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_GetDeviceDescriptor
*	功能说明:  获取设备描述符
*	形    参: Length : 长度
*	返 回 值: 设备描述符数组的地址
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_GetConfigDescriptor
*	功能说明:  获取配置描述符
*	形    参: Length : 长度
*	返 回 值: 配置描述符数组的地址
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_GetStringDescriptor
*	功能说明:  获取字符串描述符
*	形    参: Length : 长度
*	返 回 值: 字符串描述符数组的地址
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 > 4)
	{
		return NULL;
	}
	else
	{
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_Get_Interface_Setting
*	功能说明: 测试设备是否支持相应的接口
*	形    参: Interface : 接口号； AlternateSetting ：接口设置号
*	返 回 值: 字符串描述符数组的地址
*********************************************************************************************************
*/
RESULT Virtual_Com_Port_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 0)
	{
		return USB_UNSUPPORT;
	}
	else if (Interface > 1)
	{
		return USB_UNSUPPORT;
	}
	return USB_SUCCESS;
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_GetLineCoding
*	功能说明: 发送 linecoding 结构给PC主机
*	形    参: Length : 长度
*	返 回 值: linecoding 结构体的指针
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
		return NULL;
	}
	return (uint8_t *)&linecoding;
}

/*
*********************************************************************************************************
*	函 数 名: Virtual_Com_Port_SetLineCoding
*	功能说明: 设置linecoding结构体的字段
*	形    参: Length : 长度
*	返 回 值: linecoding 结构体的指针
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
		return NULL;
	}
	return(uint8_t *)&linecoding;
}
