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
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);	//ʹ�ܴ���DMA����

	USART_Cmd(USART1, ENABLE);                    	//ʹ�ܴ���1

	MYDMA_Config(DMA1_Channel5);
}

//����2��ʼ��
void UART2_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʹ�ܴ���ʱ�Ӻʹ�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.2

	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.3

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;					//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure);		//��ʼ������2
	// USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    	//ʹ�ܴ���2

	//�����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

uint16_t DMA1_MEM_LEN;

uint8_t uart1_dma_rx_buffer[512] __attribute__ ((aligned (4)));

//DMA1 ����5 UART RX
void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	//DMA ����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart1_dma_rx_buffer;			//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//���ݴ��䷽��
	DMA_InitStructure.DMA_BufferSize = sizeof(uart1_dma_rx_buffer);		//DMAͨ����DMA����Ĵ�С
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
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)) {
		//���ȫ���жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_GL5); 
		/* ����ģʽbuffer �㹻��һ�㲻�ᴥ�����ն� */
    }
}


//USART1 ���͵��ֽ�
void uart1_sendbyte(uint8_t dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(uint8_t) dat;
}

//USART2 ���͵��ֽ�
void uart2_sendbyte(uint8_t dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(uint8_t) dat;
}

//USART1 �����ַ���
void uart1_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //ѭ����������
		uart1_sendbyte(*buf++);
	}
}

//USART1 �����ַ���
void uart2_send_string(uint8_t *buf,unsigned short length)
{
	uint16_t i;
	for(i=0; i<length; i++) { //ѭ����������
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

//USART1 �����ж�
void USART1_IRQHandler(void) //����1�жϷ������
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		Res = USART1->SR;
		Res = USART1->DR;
		DMA_Cmd(DMA1_Channel5, DISABLE); //�ر�USART1
		count = sizeof(uart1_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		RingBuffer_InsertMult(&rx_ring, uart1_dma_rx_buffer, count);
		DMA_SetCurrDataCounter(DMA1_Channel5, sizeof(uart1_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel5, ENABLE); //����USART1
		xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //�����Ĵ�������
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	// if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
	// 	Res = USART1->DR;    //�����Ĵ�������
	// 	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	// } else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
	// 	Res = USART1->SR;
	// 	Res = USART1->DR;
	// 	DMA_Cmd(DMA1_Channel5, DISABLE); //�ر�USART1
	// 	count = DMA_GetCurrDataCounter(DMA1_Channel5);
	// 	RingBuffer_InsertMult(&rx_ring, uart1_dma_rx_buffer, count);
	// 	DMA_Cmd(DMA1_Channel5, ENABLE); //����USART1
	// 	xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
	// 	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	// }
}

//USART2 �����ж�
void USART2_IRQHandler(void) //����2�жϷ������
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint8_t Res;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		Res = USART_ReceiveData(USART2);    //�����Ĵ�������
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		RingBuffer_Insert(&rx_ring, &Res);
	} else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		Res = USART2->DR; // must read DR clear irq pending
		xSemaphoreGiveFromISR( xSemaphore_rx, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
