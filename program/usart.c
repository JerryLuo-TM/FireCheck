#include "Struct.h"
#include "sys.h"
#include "usart.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


//uart1 init
void UART1_Init(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	//使能USART1，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

	USART_Init(USART1, &USART_InitStructure);		//初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启串口接收中断
	USART_Cmd(USART1, ENABLE);                    	//使能串口1 

	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//空闲中断
}

//串口2初始化
void UART2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能串口时钟和串口引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.2

	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.3

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

	USART_Init(USART2, &USART_InitStructure);		//初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启串口接收中断
	USART_Cmd(USART2, ENABLE);                    	//使能串口2

	//空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

//USART1 发送单字节
void uart1_sendbyte(unsigned char dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(u8) dat;
}

//USART2 发送单字节
void uart2_sendbyte(unsigned char dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(u8) dat;
}

//USART1 发送字符串
void uart1_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //循环发送数据
		uart1_sendbyte(*buf++);
	}
}

//USART1 发送字符串
void uart2_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //循环发送数据
		uart2_sendbyte(*buf++);
	}
}

void debug_printf( const char * format, ... )
{
	uint32_t length;
	unsigned char buffer[256];
	va_list args;

	va_start (args, format);

	length = vsnprintf ((char*)buffer, 256, format, args);

	uart1_send_string(buffer, length);

	va_end (args);
}

void debug_printf2( const char * format, ... )
{
	uint32_t length;
	unsigned char buffer[256];
	va_list args;

	va_start (args, format);

	length = vsnprintf ((char*)buffer, 256, format, args);

	uart2_send_string(buffer, length);

	va_end (args);
}

//USART1 接收中断
void USART1_IRQHandler(void) //串口1中断服务程序
{
	unsigned char Res;
	(void)Res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //读出寄存器数据
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

//USART2 接收中断
void USART2_IRQHandler(void) //串口2中断服务程序
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned char Res;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		Res = USART_ReceiveData(USART2);    //读出寄存器数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		RingBuffer_Insert(&rx_ring, &Res);
	} else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		Res = USART2->DR; // must read DR clear irq pending
		xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
