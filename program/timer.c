#include "stm32f10x.h"
#include "timer.h"

//ȫ��ʱ�������
unsigned long long g_timestamp = 0;

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//APB1ʱ�Ӷ���Ƶ  APB1����ʱ��36Mhz  ��ʱ��ʱ��72Mhz
	RCC_PCLK1Config(RCC_HCLK_Div2);

	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period        = arr;    //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler     = psc;    //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);   //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//ʹ�ܻ���ʧ��ָ����TIM�ж�
	//TIM2  ʱ�Ӹ���       ʹ��
	TIM_ITConfig( TIM3, TIM_IT_Update ,ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);	//ʹ��TIMx����
}

//ʱ���ˢ���ж�  ���ȼ����   100us�ж�һ��
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	//���ָ����TIM�жϷ������:TIM �ж�Դ
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		//���TIMx���жϴ�����λ:TIM �ж�Դ
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		g_timestamp += 100ull;
	}
}




