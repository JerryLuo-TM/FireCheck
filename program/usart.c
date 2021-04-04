#include "Struct.h"
#include "sys.h"
#include "usart.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

uint8_t g_uart1_dma_tx_buffer[256] __attribute__ ((aligned (4)));
uint8_t g_uart1_dma_rx_buffer[32] __attribute__ ((aligned (4)));

volatile bool g_uart1_tx_is_transfer = false;

// 串口1 初始化
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
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	//使能串口DMA TX
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//使能串口DMA RX

	USART_Cmd(USART1, ENABLE);	//使能串口1

	UART1_TX_DMA_Config(DMA1_Channel4);
	UART1_RX_DMA_Config(DMA1_Channel5);
}

// DMA1 道配4 UART1 TX
// DMA1_Channel4
void UART1_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	//DMA 配置
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart1_dma_tx_buffer;	//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;					//数据传输方向
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart1_dma_tx_buffer);		//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//DMA通道 x拥有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//配置DMA传输完成后产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
	//配置DMA传输错误产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TE, ENABLE);
}

// DMA1 道配5 UART1 RX
// DMA1_Channel5
void UART1_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	//DMA 配置
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart1_dma_rx_buffer;			//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//数据传输方向
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart1_dma_rx_buffer);		//DMA通道的DMA缓存的大小
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
	//配置DMA传输错误产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TE, ENABLE);
}

// 开启一次DMA传输
void UART1_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE);
	DMA_SetCurrDataCounter(DMA_CHx, len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

// USART1 发送单字节
void uart1_sendbyte(uint8_t dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(uint8_t) dat;
}

// USART1 发送字符串
void uart1_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //循环发送数据
		uart1_sendbyte(*buf++);
	}
}

void debug_printf( const char * format, ... )
{
	uint32_t error_count = 0;
	uint32_t length;
	va_list args;

	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) { /* task schedule not start */
		va_start (args, format);
		length = vsnprintf ((char*)g_uart1_dma_tx_buffer, 256, format, args);
		if (length > sizeof(g_uart1_dma_tx_buffer)) {
			length = sizeof(g_uart1_dma_tx_buffer) - 2;
		}
		va_end (args);
		uart1_send_string(g_uart1_dma_tx_buffer, length);
	} else { /* task is running or suspend */
		/* suspend wait send done */
		do {
			vTaskDelay(pdMS_TO_TICKS(10));
		} while((g_uart1_tx_is_transfer == true) && (error_count++ < 10));
		/* check if dma is done */
		__disable_irq();
		if (g_uart1_tx_is_transfer == false)  {
			__enable_irq();
			va_start (args, format);
			length = vsnprintf ((char*)g_uart1_dma_tx_buffer, sizeof(g_uart1_dma_tx_buffer), format, args);
			if (length > sizeof(g_uart1_dma_tx_buffer)) {
				length = sizeof(g_uart1_dma_tx_buffer) - 2;
			}
			UART1_DMA_Enable(DMA1_Channel4, length);
			va_end (args);
		} else {
			__enable_irq();
			return ;
		}
	}
}

// USART1 接收中断
void USART1_IRQHandler(void) //串口1中断服务程序
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	(void)Res;

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		Res = USART1->SR;
		Res = USART1->DR;
		DMA_Cmd(DMA1_Channel5, DISABLE); //关闭USART1
		count = sizeof(g_uart1_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		RingBuffer_InsertMult(&uart1_rx_ring, g_uart1_dma_rx_buffer, count);
		DMA_SetCurrDataCounter(DMA1_Channel5, sizeof(g_uart1_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel5, ENABLE); //启动USART1
		if (xSemaphore_uart1_rx != NULL) {
			xSemaphoreGiveFromISR( xSemaphore_uart1_rx, &xHigherPriorityTaskWoken );
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	} else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //读出寄存器数据
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

// UART1 DMA TX
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4)) { /* 传输完成 */
		//清除传输完成中断
        DMA_ClearITPendingBit(DMA1_IT_TC4);
		g_uart1_tx_is_transfer = false;
    } else if(DMA_GetITStatus(DMA1_IT_TE4)) { /* 传输错误 */
		//清除传输错误中断
        DMA_ClearITPendingBit(DMA1_IT_TE4);
		g_uart1_tx_is_transfer = false;
	} else {	/* 其它中断 */
		//清除全部中断标志
        DMA_ClearITPendingBit(DMA1_IT_GL4);
	}
}

// UART1 DMA RX
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)) { /* 传输完成 */
		//清除传输完成中断
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    } else if(DMA_GetITStatus(DMA1_IT_TE5)) { /* 传输错误 */
		//清除传输错误中断
        DMA_ClearITPendingBit(DMA1_IT_TE5);
	} else {	/* 其它中断 */
		//清除全部中断标志
        DMA_ClearITPendingBit(DMA1_IT_GL5);
	}
}








