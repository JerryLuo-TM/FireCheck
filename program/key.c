#include "key.h"
#include "sys.h"
#include "delay.h"

void KEY_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key_up = 1;//按键按松开标志
	if(mode) key_up = 1;  //支持连按
	if(key_up && (KEY1 == 0 || KEY2 == 0)) {
		delay_ms(10);//去抖动
		key_up = 0;
		if(KEY1 == 0) {return 1;}
		else if(KEY2 == 0) {return 2;}
	} else if(KEY1 == 1 && KEY2 == 1) {
		key_up = 1;
	}

	return 0;// 无按键按下
}
