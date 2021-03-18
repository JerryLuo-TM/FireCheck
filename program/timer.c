#include "stm32f10x.h"
#include "timer.h"

//全局时间戳变量
unsigned long long g_timestamp = 0;

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//APB1时钟二分频  APB1总线时钟36Mhz  定时器时钟72Mhz
	RCC_PCLK1Config(RCC_HCLK_Div2);

	//时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period        = arr;    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler     = psc;    //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);   //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//使能或者失能指定的TIM中断
	//TIM2  时钟更新       使能
	TIM_ITConfig( TIM3, TIM_IT_Update ,ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);	//使能TIMx外设
}

//时间戳刷新中断  优先级最高   100us中断一次
void TIM3_IRQHandler(void)   //TIM3中断
{
	//检查指定的TIM中断发生与否:TIM 中断源
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		//清除TIMx的中断待处理位:TIM 中断源
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		g_timestamp += 100ull;
	}
}




