#include "Struct.h"
#include "sys.h"
#include "usart.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


//uart1 init
void UART1_Init(u32 pclk2, u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;

	GPIO_InitTypeDef GPIO_InitStructure; //GPIO端口设置
	NVIC_InitTypeDef NVIC_InitStructure; //中断配置

	temp = (float)(pclk2*1000000ull)/(bound*16);//得到USARTDIV
	mantissa = temp;				 //得到整数部分
	fraction = (temp-mantissa) * 16; //得到小数部分
    mantissa <<= 4;
	mantissa += fraction;

	RCC->APB2ENR |= 1<<3;   //使能PORTB口时钟
	RCC->APB2ENR |= 1<<14;  //使能串口1时钟

	RCC->APB2RSTR |= 1<<14;   //复位串口1
	RCC->APB2RSTR &= ~(1<<14);//停止复位

	//端口重映射
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

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//波特率设置
 	USART1->BRR  = mantissa;    //波特率设置
	USART1->CR1 |= 1<<3 | 1<<2; //使能发送 使能接收
	USART1->CR1 |= 1<<5;        //接收缓冲区非空中断使能
	USART1->CR1 |= 1<<13;  	    //串口使能

	//空闲中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

//串口2初始化  Lightbridge DBUD 协议 100Kbps 偶校验 一停止位  无流控
void UART2_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;

	GPIO_InitTypeDef GPIO_InitStructure;//GPIO端口设置
	NVIC_InitTypeDef NVIC_InitStructure;//中断配置

	temp=(float)(pclk2*1000000ull)/(bound*16); //得到USARTDIV
	mantissa=temp;				        	   //得到整数部分
	fraction=(temp-mantissa)*16; 			   //得到小数部分
	mantissa<<=4;
	mantissa+=fraction;

	//使能串口时钟和串口引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.2

	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.3

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//波特率设置
	USART2->BRR  = mantissa;		//波特率设置
	USART2->CR1 |= 1<<3 | 1<<2;		//使能发送 使能接收
	USART2->CR1 |= 1<<5;			//接收缓冲区非空中断使能
	// USART2->CR1 |= 1<<1;			//接收器正常工作

	USART2->CR1|=1<<13;				//串口使能

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

// void debug_printf2( const char * format, ... )
// {
// 	uint32_t length;
// 	unsigned char buffer[256];
// 	va_list args;

// 	va_start (args, format);

// 	length = vsnprintf ((char*)buffer, 256, format, args);

// 	uart2_send_string(buffer, length);

// 	va_end (args);
// }

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
