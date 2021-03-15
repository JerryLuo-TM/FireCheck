/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB设备描述符模块    
*	文件名称 : usb_istr.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : 定义虚拟串口设备的描述符
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

/* USB 标准设备描述符 */
const uint8_t Virtual_Com_Port_DeviceDescriptor[] =
{
	0x12,   /* bLength  */  													 //该描述符的长度
	USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */  //描述符类型 0x01=设备描述符
	0x00, 
	0x02,   /* bcdUSB = 2.00 */ 										   //本设备使用的USB协议版本
	0x02,   /* bDeviceClass: CDC */										 //类代码
	0x00,   /* bDeviceSubClass */											 //子类代码
	0x00,   /* bDeviceProtocol */											 //协议码
	0x40,   /* bMaxPacketSize0 */											 //端点0最大包长 0x40=64字节
	0x83,
	0x04,   /* idVendor = 0x0483 */										 //VID 厂家ID
	0x40,
	0x57,   /* idProduct = 0x7540 */									 //PID 产品ID 
	0x00,
	0x02,   /* bcdDevice = 2.00 */										 //设备版本号
	1,              /* Index of string descriptor describing manufacturer */ 							//描述厂家的字符串索引
	2,              /* Index of string descriptor describing product */										//描述产品的字符串索引
	3,              /* Index of string descriptor describing the device's serial number *///产品序列号的字符串索引
	0x01    /* bNumConfigurations */									 //可能的配置数
};

//配置描述符
const uint8_t Virtual_Com_Port_ConfigDescriptor[] =
{
	/*Configuration Descriptor*/
	0x09,   /* bLength: Configuration Descriptor size */															//该描述符的长度
	USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */			//描述符类型 0x02=配置描述符
	VIRTUAL_COM_PORT_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */		//配置、接口、端点、类描述符字节总和
	0x00,
	0x02,   /* bNumInterfaces: 2 interface */																					//支持的接口数量
	0x01,   /* bConfigurationValue: Configuration value */ 														//本配置描述符标识
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */  //配置描述符说明字符串索引
	0xC0,   /* bmAttributes: self powered */				//电源及唤醒
	0x32,   /* MaxPower 0 mA */											//设备耗电电流
	
	//---接口描述符---//
	/*Interface Descriptor  接口描述符 */
	0x09,   /* bLength: Interface Descriptor size */   					        //该描述符的长度
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */    //设备描述符类型 0x04=接口描述符
	/* Interface descriptor type */	
	0x00,   /* bInterfaceNumber: Number of Interface */									//本接口描述符标识
	0x00,   /* bAlternateSetting: Alternate setting */									//
	0x01,   /* bNumEndpoints: One endpoints used */											//接口端点数
	0x02,   /* bInterfaceClass: Communication Interface Class */				//接口类代码
	0x02,   /* bInterfaceSubClass: Abstract Control Model */						//启动类型 1=boot 0=no boot
	0x01,   /* bInterfaceProtocol: Common AT commands */								//0=none 1=keyboard 2=mouse
	0x00,   /* iInterface: */																					  //接口描述符说字符串索引
	
  
	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,
	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	0x01,   /* bDataInterface: 1 */
	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	0x02,   /* bmCapabilities */
	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	0x00,   /* bMasterInterface: Communication class interface */
	0x01,   /* bSlaveInterface0: Data Class Interface */
	/*Endpoint 2 Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	0x82,   /* bEndpointAddress: (IN2) */
	0x03,   /* bmAttributes: Interrupt */
	VIRTUAL_COM_PORT_INT_SIZE,      /* wMaxPacketSize: */
	0x00,
	0xFF,   /* bInterval: */
	/*Data class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
	0x01,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x02,   /* bNumEndpoints: Two endpoints used */
	0x0A,   /* bInterfaceClass: CDC */
	0x00,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */
	/*Endpoint 3 Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	0x03,   /* bEndpointAddress: (OUT3) */
	0x02,   /* bmAttributes: Bulk */
	VIRTUAL_COM_PORT_DATA_SIZE,             /* wMaxPacketSize: */
	0x00,
	0x00,   /* bInterval: ignore for Bulk transfer */
	/*Endpoint 1 Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	0x81,   /* bEndpointAddress: (IN1) */
	0x02,   /* bmAttributes: Bulk */
	VIRTUAL_COM_PORT_DATA_SIZE,             /* wMaxPacketSize: */
	0x00,
	0x00    /* bInterval */
};

/* USB String Descriptors */
const uint8_t Virtual_Com_Port_StringLangID[VIRTUAL_COM_PORT_SIZ_STRING_LANGID] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
};
//供应商描述符
const uint8_t Virtual_Com_Port_StringVendor[VIRTUAL_COM_PORT_SIZ_STRING_VENDOR] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
};
//产品描述符
const uint8_t Virtual_Com_Port_StringProduct[VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Virtual COM Port" */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'V', 0, 'i', 0,
    'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0,
    'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0, ' ', 0, ' ', 0
};
//串行描述符
uint8_t Virtual_Com_Port_StringSerial[VIRTUAL_COM_PORT_SIZ_STRING_SERIAL] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, '1', 0, '0', 0
};
