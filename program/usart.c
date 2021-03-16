#include "Struct.h"
#include "sys.h"
#include "usart.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"


//uart1 init
void UART1_Init(uint32_t bound)
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART1, &USART_InitStructure);

	// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启串口接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//空闲中断
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//使能串口DMA接收

	USART_Cmd(USART1, ENABLE);                    	//使能串口1

	MYDMA_Config(DMA1_Channel5);
}

//串口2初始化
void UART2_Init(uint32_t bound)
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
	// USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启串口接收中断
	USART_Cmd(USART2, ENABLE);                    	//使能串口2

	//空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

uint16_t DMA1_MEM_LEN;

uint8_t uart1_dma_rx_buffer[512] __attribute__ ((aligned (4)));

//DMA1 道配5 UART RX
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	//DMA 配置
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart1_dma_rx_buffer;			//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//数据传输方向
	DMA_InitStructure.DMA_BufferSize = sizeof(uart1_dma_rx_buffer);		//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//DMA通道 x拥有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//使能DMA通道
	DMA_Cmd(DMA_CHx, ENABLE);  

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//配置DMA传输完成后产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)) {
		//清除全部中断标志
        DMA_ClearITPendingBit(DMA1_IT_GL5); 
		/* 接收模式buffer 足够大一般不会触发该终端 */
    }
}


//USART1 发送单字节
void uart1_sendbyte(uint8_t dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(uint8_t) dat;
}

//USART2 发送单字节
void uart2_sendbyte(uint8_t dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(uint8_t) dat;
}

//USART1 发送字符串
void uart1_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //循环发送数据
		uart1_sendbyte(*buf++);
	}
}

//USART1 发送字符串
void uart2_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //循环发送数据
		uart2_sendbyte(*buf++);
	}
}

void debug_printf( const char * format, ... )
{
	uint32_t length;
	uint8_t buffer[256];
	va_list args;

	va_start (args, format);

	length = vsnprintf ((char*)buffer, 256, format, args);

	uart1_send_string(buffer, length);

	va_end (args);
}

void debug_printf2( const char * format, ... )
{
	uint32_t length;
	uint8_t buffer[256];
	va_list args;

	va_start (args, format);

	length = vsnprintf ((char*)buffer, 256, format, args);

	uart2_send_string(buffer, length);

	va_end (args);
}

//USART1 接收中断
void USART1_IRQHandler(void) //串口1中断服务程序
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		Res = USART1->SR;
		Res = USART1->DR;
		DMA_Cmd(DMA1_Channel5, DISABLE); //关闭USART1
		count = sizeof(uart1_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		RingBuffer_InsertMult(&rx_ring, uart1_dma_rx_buffer, count);
		DMA_SetCurrDataCounter(DMA1_Channel5, sizeof(uart1_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel5, ENABLE); //启动USART1
		xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //读出寄存器数据
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	// if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
	// 	Res = USART1->DR;    //读出寄存器数据
	// 	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	// } else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
	// 	Res = USART1->SR;
	// 	Res = USART1->DR;
	// 	DMA_Cmd(DMA1_Channel5, DISABLE); //关闭USART1
	// 	count = DMA_GetCurrDataCounter(DMA1_Channel5);
	// 	RingBuffer_InsertMult(&rx_ring, uart1_dma_rx_buffer, count);
	// 	DMA_Cmd(DMA1_Channel5, ENABLE); //启动USART1
	// 	xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
	// 	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	// }
}

//USART2 接收中断
void USART2_IRQHandler(void) //串口2中断服务程序
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t Res;
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
