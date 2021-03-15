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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;					//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure);		//��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    	//ʹ�ܴ���1 

	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//�����ж�
}

//����2��ʼ��
void UART2_Init(u32 bound)
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
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    	//ʹ�ܴ���2

	//�����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

//USART1 ���͵��ֽ�
void uart1_sendbyte(unsigned char dat)
{
	while((USART1->SR&0x40)==0);
	USART1->DR=(u8) dat;
}

//USART2 ���͵��ֽ�
void uart2_sendbyte(unsigned char dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(u8) dat;
}

//USART1 �����ַ���
void uart1_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //ѭ����������
		uart1_sendbyte(*buf++);
	}
}

//USART1 �����ַ���
void uart2_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //ѭ����������
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

//USART1 �����ж�
void USART1_IRQHandler(void) //����1�жϷ������
{
	unsigned char Res;
	(void)Res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		Res = USART1->DR;    //�����Ĵ�������
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

//USART2 �����ж�
void USART2_IRQHandler(void) //����2�жϷ������
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned char Res;
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
