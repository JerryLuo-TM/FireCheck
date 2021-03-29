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

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2����
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
uint8_t KEY_Scan(uint8_t mode)
{
	static uint8_t key_up = 1;//�������ɿ���־
	if(mode) key_up = 1;  //֧������
	if(key_up && (KEY1 == 0 || KEY2 == 0)) {
		delay_ms(10);//ȥ����
		key_up = 0;
		if(KEY1 == 0) {return 1;}
		else if(KEY2 == 0) {return 2;}
	} else if(KEY1 == 1 && KEY2 == 1) {
		key_up = 1;
	}

	return 0;// �ް�������
}
