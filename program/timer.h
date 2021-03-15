#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

//全局时间戳变量
extern unsigned long long timestamp;

void TIM3_Int_Init(u16 arr,u16 psc); 
 
#endif
