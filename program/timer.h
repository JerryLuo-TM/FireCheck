#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

//ȫ��ʱ�������
extern unsigned long long timestamp;

void TIM3_Int_Init(u16 arr,u16 psc); 
 
#endif
