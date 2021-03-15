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

	GPIO_InitTypeDef GPIO_InitStructure; //GPIO�˿�����
	NVIC_InitTypeDef NVIC_InitStructure; //�ж�����

	temp = (float)(pclk2*1000000ull)/(bound*16);//�õ�USARTDIV
	mantissa = temp;				 //�õ���������
	fraction = (temp-mantissa) * 16; //�õ�С������
    mantissa <<= 4;
	mantissa += fraction;

	RCC->APB2ENR |= 1<<3;   //ʹ��PORTB��ʱ��
	RCC->APB2ENR |= 1<<14;  //ʹ�ܴ���1ʱ��

	RCC->APB2RSTR |= 1<<14;   //��λ����1
	RCC->APB2RSTR &= ~(1<<14);//ֹͣ��λ

	//�˿���ӳ��
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

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����������
 	USART1->BRR  = mantissa;    //����������
	USART1->CR1 |= 1<<3 | 1<<2; //ʹ�ܷ��� ʹ�ܽ���
	USART1->CR1 |= 1<<5;        //���ջ������ǿ��ж�ʹ��
	USART1->CR1 |= 1<<13;  	    //����ʹ��

	//�����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

//����2��ʼ��  Lightbridge DBUD Э�� 100Kbps żУ�� һֹͣλ  ������
void UART2_Init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;

	GPIO_InitTypeDef GPIO_InitStructure;//GPIO�˿�����
	NVIC_InitTypeDef NVIC_InitStructure;//�ж�����

	temp=(float)(pclk2*1000000ull)/(bound*16); //�õ�USARTDIV
	mantissa=temp;				        	   //�õ���������
	fraction=(temp-mantissa)*16; 			   //�õ�С������
	mantissa<<=4;
	mantissa+=fraction;

	//ʹ�ܴ���ʱ�Ӻʹ�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.2

	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.3

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����������
	USART2->BRR  = mantissa;		//����������
	USART2->CR1 |= 1<<3 | 1<<2;		//ʹ�ܷ��� ʹ�ܽ���
	USART2->CR1 |= 1<<5;			//���ջ������ǿ��ж�ʹ��
	// USART2->CR1 |= 1<<1;			//��������������

	USART2->CR1|=1<<13;				//����ʹ��

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
