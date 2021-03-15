#include "Struct.h"

//地面站端的 433 向飞机发送数据
void TX_Tx_Task(void *pvParameters)
{
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}

//地面站端的 915 接收数据
void TX_Rx_Task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
	}
}

//接收到数据后校验通过  单片机通过模拟串口发送给PC
void TX_Uart_Send(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
	}
}

void Start_TX_Task(void)
{
	//RingBuffer_Init(&RF_rxring,&RF_buffer,1,sizeof(RF_buffer));//用于将整合好的数据包发出
	xTaskCreate(TX_Tx_Task, "TX_Tx_Task", 256, NULL, 2, NULL);
	xTaskCreate(TX_Rx_Task, "TX_Rx_Task", 256, NULL, 2, NULL);
}

