/*
*********************************************************************************************************
*	                                  
*	模块名称 : STM32 USB硬件配置模块    
*	文件名称 : usb_hw.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 :  USB硬件配置模块 
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

#include "Struct.h"

#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_hw.h"
#include "usb_pwr.h"

/* 定义控制USB上拉电阻的GPIO, PA10 */
#define	RCC_USB_PULL_UP		RCC_APB2Periph_GPIOA
#define	PORT_USB_PULL_UP	GPIOA
#define	PIN_USB_PULL_UP		GPIO_Pin_10

#define USB_CABLE_DISABLE()	GPIO_ResetBits(PORT_USB_PULL_UP, PIN_USB_PULL_UP)	/* 连接USB设备  */
#define USB_CABLE_ENABLE()	GPIO_SetBits(PORT_USB_PULL_UP, PIN_USB_PULL_UP)		/* 断开USB设备 */

USB_COM_FIFO_T g_tUsbFifo;		/* 定义一个全局的结构体，用于FIFO */

static void IntToUnicode (uint32_t _ulValue , uint8_t *_pBuf , uint8_t _ucLen);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitUsb
*	功能说明: 初始化CPU的USB硬件设备。配置控制上拉电阻的GPIO，配置USB输入时钟48MHz；配置USB中断
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUsb(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能控制USB上拉电阻GPIO的时钟 */
	RCC_APB2PeriphClockCmd(RCC_USB_PULL_UP, ENABLE);
	
	/* 配置 USB 上拉引脚 */
	USB_CABLE_DISABLE();	/* 断开USB设备 */
	GPIO_InitStructure.GPIO_Pin = PIN_USB_PULL_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(PORT_USB_PULL_UP, &GPIO_InitStructure);

	/* 设置USB时钟源 */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	
	/* 使能USB时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
	
	/* 配置USB中断 */
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		
		NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		#if 0	/* 根据需要使能中断:USB从挂起状态到恢复 */
		{
			EXTI_InitTypeDef EXTI_InitStructure;
			
			/* 配置外部中断线18连接到USB IP中断(CPU内部连接) */
			EXTI_ClearITPendingBit(EXTI_Line18);
			EXTI_InitStructure.EXTI_Line = EXTI_Line18; /* USB从挂起状态到恢复 */
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
		}
		#endif
	}

	g_tUsbFifo.usTxWrite = 0;
	g_tUsbFifo.usTxRead = 0;

	USB_Init();	
}

/*
*********************************************************************************************************
*	函 数 名: usb_EnterLowPowerMode
*	功能说明: 进入低功耗模式。当USB设备进入挂起模式时，关闭系统时钟和电源
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void usb_EnterLowPowerMode(void)
{
	/* 设置设备状态为 suspend, bDeviceState 全局变量在 usb_pwr.c 中定义 */
	bDeviceState = SUSPENDED;

	#if 0	/* 根据需要关闭电源 （鼠标键盘设备可进入挂起状态，虚拟串口等设备最好保持常开） */
		/* 清 EXTI Line18 中断标志 */
		EXTI_ClearITPendingBit(EXTI_Line8);
		
		/* 在低功耗状态，关闭电压调节器 */
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);	
	#endif
}

/*
*********************************************************************************************************
*	函 数 名: usb_LeaveLowPowerMode
*	功能说明: 退出低功耗模式。恢复系统时钟。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void usb_LeaveLowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;  /* 全局变量Device_Info 在USB固件库 usb_init.c 中定义 */	

#if 0	/* 根据需要恢复系统时钟 */
	/* 使能 HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* 等待 HSE 就绪 */
	RCC_WaitForHSEStartUp();
	
	/* 使能 HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* 等待 HSE 就绪 */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	
	/* 使能PLL */
	RCC_PLLCmd(ENABLE);
	
	/* 等待PLL就绪 */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	
	/* 选择PLL作为系统时钟源 */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	/* 等待PLL系统时钟源稳定 */
	while (RCC_GetSYSCLKSource() != 0x08);
#endif
	
	/* 更新全局变量bDeviceState，表示usb设备状态 */
	if (pInfo->Current_Configuration != 0)
	{
		bDeviceState = CONFIGURED;	/* 配置成功 */
	}
	else
	{
		bDeviceState = ATTACHED;	/* USB设备已连接，但还未配置 */
	}	
}

/*
*********************************************************************************************************
*	函 数 名: usb_CableConfig
*	功能说明: 软件控制USB电缆连接和断开
*	形    参: _ucMode ： ENABLE表示连接，DISABLE表示断开
*	返 回 值: 无
*********************************************************************************************************
*/
void usb_CableConfig(uint8_t _ucMode)
{
	if (_ucMode == DISABLE)
	{
		USB_CABLE_DISABLE();	/* 断开USB设备 */
		vTaskDelay(configTICK_RATE_HZ/100);//保留一定的时间给Host检测
	}
	else
	{
		USB_CABLE_ENABLE();		/* 连接USB设备, 实质上将D+上拉，此时USB设备就能被集线器检测到了 */
	  vTaskDelay(configTICK_RATE_HZ/100);//保留一定的时间给Host检测
	}
}
				   
/*
*********************************************************************************************************
*	函 数 名: usb_SendBuf
*	功能说明: 向PC主机发送一组数据
*	形    参：_pBuf 输入缓冲区; 	_ucLen : 数据长度
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
void usb_SendBuf(uint8_t *_pTxBuf, uint8_t _ucLen)
{
    UserToPMABufferCopy(_pTxBuf, ENDP1_TXADDR, _ucLen);
    SetEPTxCount(ENDP1, _ucLen);
    SetEPTxValid(ENDP1); 
}

/*
*********************************************************************************************************
*	函 数 名: Get_SerialNum
*	功能说明: 获取一个设备序列号，需要转换为UNICODE格式
*	形    参：_pBuf 目标缓冲区
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
void Get_SerialNum(uint8_t *_pBuf)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
	
	Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
	Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
	Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
	
	Device_Serial0 += Device_Serial2;
	
	if (Device_Serial0 != 0)
	{
		IntToUnicode (Device_Serial0, &_pBuf[2] , 8);
		IntToUnicode (Device_Serial1, &_pBuf[18], 4);
	}
}

/*
*********************************************************************************************************
*	函 数 名: IntToUnicode
*	功能说明: 将一个32位的HEX值转换为Unicode编码格式的字符串
*	形    参：_ulValue : 32位Hex； 
*			  _pBuf : 目标缓冲区
*			 _ucLen : 目标码长度
*	返 回 值: 无
*********************************************************************************************************
*/
static void IntToUnicode (uint32_t _ulValue , uint8_t *_pBuf , uint8_t _ucLen)
{
	uint8_t idx = 0;
	
	for( idx = 0 ; idx < _ucLen ; idx ++)
	{
		if( ((_ulValue >> 28)) < 0xA )
		{
			_pBuf[2 * idx] = (_ulValue >> 28) + '0';
		}
		else
		{
			_pBuf[2 * idx] = (_ulValue >> 28) + 'A' - 10; 
		}
		
		_ulValue = _ulValue << 4;
		
		_pBuf[ 2 * idx + 1] = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: SaveHostDataToBuf
*	功能说明: 将USB主机发送的数据缓存到全局缓冲区。该函数被USB中断服务程序调用。
*	形    参: _pInBuf :输入缓冲区；PC发到设备的数据 
*			  _pBuf : 目标缓冲区
*			 _ucLen : 目标码长度
*	返 回 值: 无
*********************************************************************************************************
*/
void usb_SaveHostDataToBuf(uint8_t *_pInBuf, uint16_t _usLen)
{
  //填入缓冲区 只有空缓冲区大于待存入数据长度才允许  否则丢弃此包
	if(RingBuffer_GetFree(&TX_USB_rxring)>_usLen)
	{
		RingBuffer_InsertMult(&TX_USB_rxring,_pInBuf,_usLen);
	}
}

/*
*********************************************************************************************************
*	函 数 名: SendDataToHost
*	功能说明: 发送数据到主机。 主程序调用。
*	形    参: _pInBuf :输入缓冲区；PC发到设备的数据 
*			  _pBuf : 目标缓冲区
*			 _ucLen : 目标码长度
*	返 回 值: 无
*********************************************************************************************************
*/
void usb_SendDataToHost(uint8_t *_pTxBuf, uint16_t _usLen)
{
	uint16_t i;
	
	/* 未考虑缓冲区填满的情况 */
	/* 先将数据缓存到内存 */
	for (i = 0 ; i < _usLen; i++)
	{
		g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxWrite] = _pTxBuf[i];

		__set_PRIMASK(1);  		/* 关中断，避免USB中断程序和主程序访问 usTxWrite 变量冲突 */
		
		if (++g_tUsbFifo.usTxWrite >= USB_TX_BUF_SIZE)
		{
			g_tUsbFifo.usTxWrite = 0;
		}

		__set_PRIMASK(0);  		/* 开中断 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: usb_GetTxWord
*	功能说明: 从发送缓冲区读取1个字，2字节. 用于USB中断
*	形    参: _pByteNum : 存储读取的字节数的变量的指针
*	返 回 值: 读到字（2字节）
*********************************************************************************************************
*/
uint16_t usb_GetTxWord(uint8_t *_pByteNum)
{
	uint16_t usData;
	
	/* 发送缓冲区为空时，返回字节数 = 0 */
	if (g_tUsbFifo.usTxRead == g_tUsbFifo.usTxWrite)
	{
		*_pByteNum = 0;
		return 0;
	}
	
	/* 保存第1个字节 */
	usData = g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxRead];
	
	/* 移动读指针 */
	if (++g_tUsbFifo.usTxRead >= USB_TX_BUF_SIZE)
	{
		g_tUsbFifo.usTxRead = 0;
	}
	
	/* 不足2字节，直接返回 */
	if (g_tUsbFifo.usTxRead == g_tUsbFifo.usTxWrite)
	{
		*_pByteNum = 1;		/* 有效字节个数 = 1 */
		return usData;
	}	
	
	/* 保存第2个字节 */
	usData += g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxRead] << 8;

	/* 移动读指针 */
	if (++g_tUsbFifo.usTxRead >= USB_TX_BUF_SIZE)
	{
		g_tUsbFifo.usTxRead = 0;
	}

	*_pByteNum = 2;		/* 有效字节个数 = 2 */
	return usData;		
}
