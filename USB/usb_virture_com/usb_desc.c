/*
*********************************************************************************************************
*	                                  
*	ģ������ : USB�豸������ģ��    
*	�ļ����� : usb_istr.c
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� : �������⴮���豸��������
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

#include "usb_lib.h"
#include "usb_desc.h"

/* USB ��׼�豸������ */
const uint8_t Virtual_Com_Port_DeviceDescriptor[] =
{
	0x12,   /* bLength  */  													 //���������ĳ���
	USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */  //���������� 0x01=�豸������
	0x00, 
	0x02,   /* bcdUSB = 2.00 */ 										   //���豸ʹ�õ�USBЭ��汾
	0x02,   /* bDeviceClass: CDC */										 //�����
	0x00,   /* bDeviceSubClass */											 //�������
	0x00,   /* bDeviceProtocol */											 //Э����
	0x40,   /* bMaxPacketSize0 */											 //�˵�0������ 0x40=64�ֽ�
	0x83,
	0x04,   /* idVendor = 0x0483 */										 //VID ����ID
	0x40,
	0x57,   /* idProduct = 0x7540 */									 //PID ��ƷID 
	0x00,
	0x02,   /* bcdDevice = 2.00 */										 //�豸�汾��
	1,              /* Index of string descriptor describing manufacturer */ 							//�������ҵ��ַ�������
	2,              /* Index of string descriptor describing product */										//������Ʒ���ַ�������
	3,              /* Index of string descriptor describing the device's serial number *///��Ʒ���кŵ��ַ�������
	0x01    /* bNumConfigurations */									 //���ܵ�������
};

//����������
const uint8_t Virtual_Com_Port_ConfigDescriptor[] =
{
	/*Configuration Descriptor*/
	0x09,   /* bLength: Configuration Descriptor size */															//���������ĳ���
	USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */			//���������� 0x02=����������
	VIRTUAL_COM_PORT_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */		//���á��ӿڡ��˵㡢���������ֽ��ܺ�
	0x00,
	0x02,   /* bNumInterfaces: 2 interface */																					//֧�ֵĽӿ�����
	0x01,   /* bConfigurationValue: Configuration value */ 														//��������������ʶ
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */  //����������˵���ַ�������
	0xC0,   /* bmAttributes: self powered */				//��Դ������
	0x32,   /* MaxPower 0 mA */											//�豸�ĵ����
	
	//---�ӿ�������---//
	/*Interface Descriptor  �ӿ������� */
	0x09,   /* bLength: Interface Descriptor size */   					        //���������ĳ���
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */    //�豸���������� 0x04=�ӿ�������
	/* Interface descriptor type */	
	0x00,   /* bInterfaceNumber: Number of Interface */									//���ӿ���������ʶ
	0x00,   /* bAlternateSetting: Alternate setting */									//
	0x01,   /* bNumEndpoints: One endpoints used */											//�ӿڶ˵���
	0x02,   /* bInterfaceClass: Communication Interface Class */				//�ӿ������
	0x02,   /* bInterfaceSubClass: Abstract Control Model */						//�������� 1=boot 0=no boot
	0x01,   /* bInterfaceProtocol: Common AT commands */								//0=none 1=keyboard 2=mouse
	0x00,   /* iInterface: */																					  //�ӿ�������˵�ַ�������
	
  
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
//��Ӧ��������
const uint8_t Virtual_Com_Port_StringVendor[VIRTUAL_COM_PORT_SIZ_STRING_VENDOR] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
};
//��Ʒ������
const uint8_t Virtual_Com_Port_StringProduct[VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Virtual COM Port" */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'V', 0, 'i', 0,
    'r', 0, 't', 0, 'u', 0, 'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0,
    'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0, ' ', 0, ' ', 0
};
//����������
uint8_t Virtual_Com_Port_StringSerial[VIRTUAL_COM_PORT_SIZ_STRING_SERIAL] =
{
    VIRTUAL_COM_PORT_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, '1', 0, '0', 0
};
