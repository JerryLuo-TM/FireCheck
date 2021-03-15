/*
*********************************************************************************************************
*	                                  
*	ģ������ : USB���⴮����صĳ���
*	�ļ����� : usb_istr.c
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� : ��USB���⴮����صĺ�����
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_hw.h"

static uint8_t s_Request = 0;

LINE_CODING linecoding =
{
	57600,  /* ������ */
	0x00,   /* ֹͣλ-1 */
	0x00,   /* ����λ�� �� */
	0x08    /* ����λ��8bit */
};

DEVICE Device_Table =
{
    EP_NUM,	/* �˵���� */
    1
};

/* �豸���� */
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
*	�� �� ��: Virtual_Com_Port_init
*	����˵��: ���⴮�ڳ�ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Virtual_Com_Port_init(void)
{
	/* ��ȡ�豸���к� */
	Get_SerialNum(Virtual_Com_Port_StringSerial);
	
	pInformation->Current_Configuration = 0;		/* pInformation ȫ�ֱ�������USB�̼����ж��� */
	
	/* ����USB�豸���ú�����ʹ��USB�������裬�����б�Ҫ������ */
	usb_PowerOn();
	
	/* ִ�л������豸��ʼ�� */
	USB_SIL_Init();
	
	bDeviceState = UNCONNECTED;		/* �����豸״̬���� */
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_Reset
*	����˵��: ���⴮�ڸ�λ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Virtual_Com_Port_Reset(void)
{
	/* �����豸״̬Ϊ��δ���� */
	pInformation->Current_Configuration = 0;
	
	/* �����豸��ǰ���� */
	pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];
	
	/* �����豸��ǰ�ӿ� */
	pInformation->Current_Interface = 0;
	
	SetBTABLE(BTABLE_ADDRESS);	/* ���÷��黺������������Ĵ���BTABLE�еĵ�ַ */
	
	/* ��ʼ���˵�0Ϊ���ƴ���ģʽ */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);
	
	/* ��ʼ���˵�1ΪBULK��������ģʽ */
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	
	/* ��ʼ���˵�2Ϊ�жϴ���ģʽ */
	SetEPType(ENDP2, EP_INTERRUPT);
	SetEPTxAddr(ENDP2, ENDP2_TXADDR);
	SetEPRxStatus(ENDP2, EP_RX_DIS);
	SetEPTxStatus(ENDP2, EP_TX_NAK);
	
	/* ��ʼ���˵�3ΪBULK��������ģʽ */
	SetEPType(ENDP3, EP_BULK);
	SetEPRxAddr(ENDP3, ENDP3_RXADDR);
	SetEPRxCount(ENDP3, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPRxStatus(ENDP3, EP_RX_VALID);
	SetEPTxStatus(ENDP3, EP_TX_DIS);
	
	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;	/* �豸������ */
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_SetConfiguration
*	����˵��: �����豸״̬Ϊ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Virtual_Com_Port_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	
	if (pInfo->Current_Configuration != 0)
	{
		/* �豸�Ѿ�������� */
		bDeviceState = CONFIGURED;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_SetDeviceAddress
*	����˵��: �����豸״̬Ϊ����Ѱַ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Virtual_Com_Port_SetDeviceAddress (void)
{
	bDeviceState = ADDRESSED;
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_Status_In
*	����˵��: Virtual COM Port Status In Routine.
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: Virtual_Com_Port_Status_Out
*	����˵��: Virtual COM Port Status OUT Routine.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Virtual_Com_Port_Status_Out(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_Data_Setup
*	����˵��: handle the data class specific requests
*	��    ��: RequestNo
*	�� �� ֵ: USB_UNSUPPORT or USB_SUCCESS.
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
*	�� �� ��: Virtual_Com_Port_NoData_Setup
*	����˵��:  handle the no data class specific requests.
*	��    ��: RequestNo
*	�� �� ֵ: USB_UNSUPPORT or USB_SUCCESS.
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
*	�� �� ��: Virtual_Com_Port_GetDeviceDescriptor
*	����˵��:  ��ȡ�豸������
*	��    ��: Length : ����
*	�� �� ֵ: �豸����������ĵ�ַ
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_GetConfigDescriptor
*	����˵��:  ��ȡ����������
*	��    ��: Length : ����
*	�� �� ֵ: ��������������ĵ�ַ
*********************************************************************************************************
*/
uint8_t *Virtual_Com_Port_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*
*********************************************************************************************************
*	�� �� ��: Virtual_Com_Port_GetStringDescriptor
*	����˵��:  ��ȡ�ַ���������
*	��    ��: Length : ����
*	�� �� ֵ: �ַ�������������ĵ�ַ
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
*	�� �� ��: Virtual_Com_Port_Get_Interface_Setting
*	����˵��: �����豸�Ƿ�֧����Ӧ�Ľӿ�
*	��    ��: Interface : �ӿںţ� AlternateSetting ���ӿ����ú�
*	�� �� ֵ: �ַ�������������ĵ�ַ
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
*	�� �� ��: Virtual_Com_Port_GetLineCoding
*	����˵��: ���� linecoding �ṹ��PC����
*	��    ��: Length : ����
*	�� �� ֵ: linecoding �ṹ���ָ��
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
*	�� �� ��: Virtual_Com_Port_SetLineCoding
*	����˵��: ����linecoding�ṹ����ֶ�
*	��    ��: Length : ����
*	�� �� ֵ: linecoding �ṹ���ָ��
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
