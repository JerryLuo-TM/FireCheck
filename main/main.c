#include "Struct.h"

/* uart2 串口环形缓冲 */
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


//用于创建任务的任务
void create_app_task(void)
{
	RingBuffer_Init(&uart1_rx_ring, uart1_rx_ring_buffer, 1, sizeof(uart1_rx_ring_buffer));

	xSemaphore_uart1_rx = xSemaphoreCreateBinary();

	//创建demo task
    xTaskCreate((TaskFunction_t )demo_task,
                (const char*    )"demo_task",
                (uint16_t       )4096/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )NULL);

	//创建demo task
    xTaskCreate((TaskFunction_t )race_task,
                (const char*    )"race_task",
                (uint16_t       )2048/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )NULL);
}

int main(void)
{
	Stm32_Clock_Init(9); //系统时钟设置  8Mhz * 9 = 72MHZ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组4

	/* 延时初始化 */
	delay_init();

	KEY_Init();

	/* 初始化LED灯 */
	LED_init();

	/* 初始化串口 */
	UART1_Init(921600); //总线时钟：72Mhz

	/* 10khz的计数频率，中断一次  100us一次 */
	TIM3_Int_Init(999,7199);

	/* 创建task */
	create_app_task();

	AMG8833_Init();

	TIM2_PWM_Init(2499, 71);
	TIM_SetCompare1(TIM2, 1000);
	TIM_SetCompare2(TIM2, 1000);

	/* 开启任务调度 */
	vTaskStartScheduler();

	return 0;
}









