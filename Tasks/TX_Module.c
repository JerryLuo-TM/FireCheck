#include "Struct.h"

//����վ�˵� 433 ��ɻ���������
void TX_Tx_Task(void *pvParameters)
{
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}

//����վ�˵� 915 ��������
void TX_Rx_Task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
	}
}

//���յ����ݺ�У��ͨ��  ��Ƭ��ͨ��ģ�⴮�ڷ��͸�PC
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
	//RingBuffer_Init(&RF_rxring,&RF_buffer,1,sizeof(RF_buffer));//���ڽ����Ϻõ����ݰ�����
	xTaskCreate(TX_Tx_Task, "TX_Tx_Task", 256, NULL, 2, NULL);
	xTaskCreate(TX_Rx_Task, "TX_Rx_Task", 256, NULL, 2, NULL);
}

