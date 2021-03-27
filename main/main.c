#include "Struct.h"

/* uart2 ���ڻ��λ��� */
RINGBUFF_T uart1_rx_ring;
uint8_t uart1_rx_ring_buffer[256] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_uart1_rx;

void demo_task(void *pvParameters)
{
	uint8_t key_num;
	uint32_t count;

	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		if (++count%2 == 0) {
			LED_B = 1;
			LED_TEST = 1;
		} else {
			LED_B = 0;
			LED_TEST = 0;
		}

		// key_num = KEY_Scan(0);
		// debug_printf("Hello world\r\n");

	
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/2);
	}
}

void race_task(void *pvParameters)
{
	uint32_t receive_length;
	uint8_t receive_buffer[128];
	while (1)
	{
		if( xSemaphoreTake( xSemaphore_uart1_rx, portMAX_DELAY) == pdPASS ) {
			receive_length = RingBuffer_GetCount(&uart1_rx_ring);
			if (receive_length > 0) {
				memset(receive_buffer, 0, sizeof(receive_buffer));
				RingBuffer_PopMult(&uart1_rx_ring, &receive_buffer[0], receive_length);
				if (receive_buffer[0] > 100) {
					TIM_SetCompare1(TIM2, 2000);
				} else {
					TIM_SetCompare1(TIM2, 1000 + (uint32_t)receive_buffer[0] * 10);
				}
				debug_printf("len[%d] value[%d]\r\n", receive_length, receive_buffer[0]);
				// uart1_send_string(receive_buffer, receive_length);
			}
		}
	}
}


//���ڴ������������
void create_app_task(void)
{
	RingBuffer_Init(&uart1_rx_ring, uart1_rx_ring_buffer, 1, sizeof(uart1_rx_ring_buffer));

	xSemaphore_uart1_rx = xSemaphoreCreateBinary();

	//����demo task
    xTaskCreate((TaskFunction_t )demo_task,
                (const char*    )"demo_task",
                (uint16_t       )4096/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )NULL);

	//����demo task
    xTaskCreate((TaskFunction_t )race_task,
                (const char*    )"race_task",
                (uint16_t       )2048/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )NULL);
}

int main(void)
{
	Stm32_Clock_Init(9); //ϵͳʱ������  8Mhz * 9 = 72MHZ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����4

	/* ��ʱ��ʼ�� */
	delay_init();

	KEY_Init();

	/* ��ʼ��LED�� */
	LED_init();

	/* ��ʼ������ */
	UART1_Init(921600); //����ʱ�ӣ�72Mhz

	/* 10khz�ļ���Ƶ�ʣ��ж�һ��  100usһ�� */
	TIM3_Int_Init(999,7199);

	/* ����task */
	create_app_task();

	AMG8833_Init();

	TIM2_PWM_Init(2499, 71);
	TIM_SetCompare1(TIM2, 1000);
	TIM_SetCompare2(TIM2, 1000);

	/* ����������� */
	vTaskStartScheduler();

	return 0;
}









