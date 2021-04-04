#include "Struct.h"
#include "sys.h"
#include "usart.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

uint8_t g_uart1_dma_tx_buffer[256] __attribute__ ((aligned (4)));
uint8_t g_uart1_dma_rx_buffer[32] __attribute__ ((aligned (4)));

volatile bool g_uart1_tx_is_transfer = false;

// ����1 ��ʼ��
void UART1_Init(uint32_t bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	//ʹ��USART1��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;					//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);

	// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//�������ڽ����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//�����ж�
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	//ʹ�ܴ���DMA TX
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//ʹ�ܴ���DMA RX

	USART_Cmd(USART1, ENABLE);	//ʹ�ܴ���1

	UART1_TX_DMA_Config(DMA1_Channel4);
	UART1_RX_DMA_Config(DMA1_Channel5);
}

// DMA1 ����4 UART1 TX
// DMA1_Channel4
void UART1_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	//DMA ����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart1_dma_tx_buffer;	//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;					//���ݴ��䷽��
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart1_dma_tx_buffer);		//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//DMAͨ�� xӵ�������ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����DMA������ɺ�����ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
	//����DMA�����������ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TE, ENABLE);
}

// DMA1 ����5 UART1 RX
// DMA1_Channel5
void UART1_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	//DMA ����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart1_dma_rx_buffer;			//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//���ݴ��䷽��
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart1_dma_rx_buffer);		//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//DMAͨ�� xӵ�������ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//ʹ��DMAͨ��
	DMA_Cmd(DMA_CHx, ENABLE);

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����DMA������ɺ�����ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
	//����DMA�����������ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TE, ENABLE);
}

// ����һ��DMA����
void UART1_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE);
	DMA_SetCurrDataCounter(DMA_CHx, len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

// USART1 ���͵��ֽ�
void uart1_sendbyte(uint8_t dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(uint8_t) dat;
}

// USART1 �����ַ���
void uart1_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //ѭ����������
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

// USART1 �����ж�
void USART1_IRQHandler(void) //����1�жϷ������
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	(void)Res;

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		Res = USART1->SR;
		Res = USART1->DR;
		DMA_Cmd(DMA1_Channel5, DISABLE); //�ر�USART1
		count = sizeof(g_uart1_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		RingBuffer_InsertMult(&uart1_rx_ring, g_uart1_dma_rx_buffer, count);
		DMA_SetCurrDataCounter(DMA1_Channel5, sizeof(g_uart1_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel5, ENABLE); //����USART1
		if (xSemaphore_uart1_rx != NULL) {
			xSemaphoreGiveFromISR( xSemaphore_uart1_rx, &xHigherPriorityTaskWoken );
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	} else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //�����Ĵ�������
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

// UART1 DMA TX
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TC4);
		g_uart1_tx_is_transfer = false;
    } else if(DMA_GetITStatus(DMA1_IT_TE4)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TE4);
		g_uart1_tx_is_transfer = false;
	} else {	/* �����ж� */
		//���ȫ���жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_GL4);
	}
}

// UART1 DMA RX
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    } else if(DMA_GetITStatus(DMA1_IT_TE5)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TE5);
	} else {	/* �����ж� */
		//���ȫ���жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_GL5);
	}
}








