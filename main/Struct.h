#ifndef _Struct_H_
#define _Struct_H_

#include "stm32f10x.h"
#include <string.h>
#include "sys.h"

#include "ring_buffer.h"
#include "stdbool.h"
#include "timer.h"
#include "LED.h"
#include "delay.h"
#include "stmflash.h"
#include "usart.h"


//操作系统相关头文件
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
#include "semphr.h"

//设置FLASH 保存地址
//(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
//64K 开始的位置
#define FLASH_SAVE_ADDR   0x08010000   //频率参数

extern SemaphoreHandle_t xSemaphore_rx;

extern RINGBUFF_T tx_ring;
extern RINGBUFF_T rx_ring;


typedef union
{
	float d;
	unsigned char outuchar[4];
} Data;


void Data_Conversion(float dat, Data d1, uint8_t *buf);
float Data_Conversion_float(uint8_t *buf);



#endif
