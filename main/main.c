#include "Struct.h"

/* uart1 ring buffer array */
RINGBUFF_T uart1_rx_ring;
uint8_t uart1_rx_ring_buffer[256] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_uart1_rx;

uint16_t color_table[16] = {0x5555, 0x0000, 0x001F, 0XF81F, 0XFFE0, 0X07FF, 0xF800, 0X5010,
							0xF81F, 0x07E0, 0x7FFF, 0xFFE0, 0XBC40, 0XFC07, 0X8430, 0x51F1};

void demo_task(void *pvParameters)
{
	uint8_t key_num;
	uint32_t count, max_index, i, j;
	float receive_buffer[8][8];
	float line_max[8];
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

		key_num = KEY_Scan(0);
		if (key_num != 0) {
			debug_printf("Press key = %d \r\n", key_num);
		}

		AMG8833_ReadPixels((float*)&receive_buffer[0][0], 64);
		show_map(receive_buffer, 64);

		// debug_printf("temputer : \r\n");
		// for (i = 0; i < 8; i++) {
		// 	line_max[i] = receive_buffer[i][0];
		// 	for (j = 0; j < 8; j++) {
		// 		if (line_max[i] < receive_buffer[i][j]) {
		// 			line_max[i] = receive_buffer[i][j];
		// 		}
		// 		debug_printf("%2.0f ", receive_buffer[i][j]);
		// 	}
		// 	debug_printf("\r\n");
		// }
		// debug_printf("\r\n");

		max_index = 0;
		for (i = 0; i < 8 ; i++) {
			if (line_max[i] > line_max[max_index]) {
				max_index = i;
			}
		}

		// debug_printf("index = %d \r\n", max_index);
		// TIM_SetCompare1(TIM2, 1000 + (max_index * 100));

		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/10);
	}
}

void race_task(void *pvParameters)
{
	uint32_t receive_length;
	uint8_t uart_receive_buffer[128];
	while (1)
	{
		if( xSemaphoreTake( xSemaphore_uart1_rx, portMAX_DELAY) == pdPASS ) {
			receive_length = RingBuffer_GetCount(&uart1_rx_ring);
			if (receive_length > 0) {
				memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer));
				RingBuffer_PopMult(&uart1_rx_ring, &uart_receive_buffer[0], receive_length);
				if (uart_receive_buffer[0] > 200) {
					TIM_SetCompare1(TIM2, 2500);
				} else {
					TIM_SetCompare1(TIM2, 500 + (uint32_t)uart_receive_buffer[0] * 10);
				}
				// debug_printf("len[%d] value[%d] value[%d] value[%d]\r\n", receive_length,
				// 							uart_receive_buffer[0],uart_receive_buffer[1],uart_receive_buffer[2]);
				// uart1_send_string(uart_receive_buffer, receive_length);
			}
		}
	}
}


void create_app_task(void)
{
	/* uart1 rx ring buffer init */
	RingBuffer_Init(&uart1_rx_ring, uart1_rx_ring_buffer, 1, sizeof(uart1_rx_ring_buffer));

	xSemaphore_uart1_rx = xSemaphoreCreateBinary();

	//create demo task
    xTaskCreate((TaskFunction_t )demo_task,
                (const char*    )"demo_task",
                (uint16_t       )4096/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )NULL);

	//create demo task
    xTaskCreate((TaskFunction_t )race_task,
                (const char*    )"race_task",
                (uint16_t       )2048/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )NULL);
}

int main(void)
{
	Stm32_Clock_Init(6); //8Mhz * 9 = 72M
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组

	/* Delay Init */
	delay_init();

	KEY_Init();

	/* LED Init */
	LED_init();

	/* UART1 INIT */
	UART1_Init(115200); //APB2 peripheral = 72Mhz

	/* Timestamp timer init @100us*/
	TIM3_Int_Init(999,7199);

	/* create app task */
	create_app_task();

	AMG8833_Init();

	TIM2_PWM_Init(2499, 71);
	TIM_SetCompare1(TIM2, 1000);
	TIM_SetCompare2(TIM2, 1000);

	LCD_Init();
	DispColor(WHITE);

	/* 开启任务调度 */
	vTaskStartScheduler();

	return 0;
}









