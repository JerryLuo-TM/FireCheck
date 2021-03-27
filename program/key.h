#ifndef __KEY_H
#define __KEY_H
#include "sys.h"


#define KEY1  PCin(13) //GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)//读取按键1
#define KEY2  PCin(14) //GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)//读取按键2


#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

void KEY_Init(void);//IO初始化
uint8_t KEY_Scan(uint8_t mode);

#endif
