#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

#include "tft.h"


#define ROW  128			//显示行数
#define COL  128			//显示列数	

//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

void TFT_Init(void)
{
	IIC_Init();
}






