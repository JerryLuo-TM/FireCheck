#include "Struct.h"

/* uart1 ring buffer array */
RINGBUFF_T uart1_rx_ring;
uint8_t uart1_rx_ring_buffer[256] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_uart1_rx;


uint16_t PriData[8][8];
long data[PixLg][PixLg];
long ext[3];
uint8_t ext_add[2];

uint8_t fram_rate_count, fram_rate;

void demo_task(void *pvParameters)
{
	uint32_t count;
	unsigned char str_buf[16];
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint32_t key_value, pwm_value = 1500;
	float max_temp;
	float min_temp;
	uint8_t max_index;
	uint8_t min_index;
	while (1)
	{
		if (++count%2 == 0) {
			LED_B = 1;
			LED_TEST = 1;
		} else {
			LED_B = 0;
			LED_TEST = 0;
		}

		key_value = KEY_Scan(1);
		if (key_value == 1) {
			pwm_value += 50;
		} else if (key_value == 2) {
			pwm_value -= 50;
		}

		if (pwm_value > 2500) {
			pwm_value = 2500;
			debug_printf("pwm max value = %d \r\n", pwm_value);
		} else if (pwm_value < 500) {
			pwm_value = 500;
			debug_printf("pwm mini value = %d \r\n", pwm_value);
		}

		// TIM_SetCompare1(TIM2, pwm_value);

		/* 获取原始数据 */
		AMG8833_get_Pixels(PriData);
		/* 转换图像 */
		AMG8833_get_Img();
		/* 图像输出 */
		AMG8833_draw_Img();
		/* 帧率统计 */
		fram_rate_count += 1;
		/* 计算参数 */
		max_temp = (float)ext[0] * 0.25f;
		min_temp = (float)ext[1] * 0.25f;
		max_index = ext_add[0];
		min_index = ext_add[1];

		/* show max temputer */
		POINT_COLOR = WHITE, BACK_COLOR = BLACK;
		sprintf((char*)str_buf, "%.1f", max_temp);
		LCD_ShowStr(2 + 0,  82 + 5, "Max: ", 16);
		LCD_ShowStr(2 + 40, 82 + 5, str_buf, 16);
		sprintf((char*)str_buf, "(%d,%d)", 7 - max_index/8, max_index%8);
		LCD_ShowStr(2 + 40 + 40, 82 + 5, str_buf, 16);

		/* show min temputer */
		POINT_COLOR = WHITE, BACK_COLOR = BLACK;
		sprintf((char*)str_buf, "%.1f", min_temp);
		LCD_ShowStr(2,  82 + 5 + 20, "Min: ", 16);
		LCD_ShowStr(2 + 40, 82 + 5 + 20, str_buf, 16);
		sprintf((char*)str_buf, "(%d,%d)", min_index/8, min_index%8);
		LCD_ShowStr(2 + 40 + 40, 82 + 5 + 20, str_buf, 16);

		/* show Frame rate */
		sprintf((char*)str_buf, "%0.2d", fram_rate);
		LCD_ShowStr(82 + 2, 2, "FPS", 16);
		LCD_ShowStr(82 + 24 + 2 ,  2, str_buf, 16);

		{
			if (max_temp > 35.0f) {
				LASER_Switch = 1;
				TIM_SetCompare1(TIM2, 500 + ((max_index / 8) * 250));
			} else {
				LASER_Switch = 0;
				// TIM_SetCompare1(TIM2, 1500);
			}
		}

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
				debug_printf("len[%d] value[%d] value[%d] value[%d]\r\n", receive_length,
											uart_receive_buffer[0],uart_receive_buffer[1],uart_receive_buffer[2]);
			}
		}
	}
}

void caculate_task(void *pvParameters)
{
	uint32_t count;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		count += 1;
		if ((count % 20) == 0) {
			fram_rate = fram_rate_count;
			fram_rate_count = 0;
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/20);
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
                (uint16_t       )1024/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )NULL);

	//create caculate task
    xTaskCreate((TaskFunction_t )caculate_task,
                (const char*    )"caculate_task",
                (uint16_t       )1024/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )NULL);
}

int main(void)
{
	/* reset handle had init sysclk */
	/* modify ext freq HSE_VALUE stm32f10x.h */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组

	/* Delay Init */
	delay_init();

	KEY_Init();

	/* LED Init */
	LED_init();

	/* UART1 INIT */
	UART1_Init(921600); //APB2 peripheral = 72Mhz

	/* Timestamp timer init @100us*/
	TIM3_Int_Init(999,7199);

	/* create app task */
	create_app_task();

	AMG8833_Init();

	TIM2_PWM_Init(2499, 71);
	TIM_SetCompare1(TIM2, 1500);
	TIM_SetCompare2(TIM2, 1500);

	LCD_Init();
	LCD_DispColor(BLACK);

	/* 开启任务调度 */
	vTaskStartScheduler();

	return 0;
}









