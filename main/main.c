#include "Struct.h"

/* uart1 ring buffer array */
RINGBUFF_T uart1_rx_ring;
uint8_t uart1_rx_ring_buffer[256] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_uart1_rx;


int16_t PriData[8][8];
uint16_t data[PixLg][PixLg];
int16_t ext[3];
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

		debug_printf("max:%f min:%f \r\n", max_temp, min_temp);
		{
			if (max_temp > 35.0f) {
				LASER_Switch = 1;
				TIM_SetCompare1(TIM2, 500 + ((max_index / 8) * 250));
			} else {
				LASER_Switch = 0;
				// TIM_SetCompare1(TIM2, 1500);
			}
		}

		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/20);
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
		if ((count % 50) == 0) {
			fram_rate = fram_rate_count;
			fram_rate_count = 0;
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/50);
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
                (uint16_t       )1024/sizeof(StackType_t),
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

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// 把 RCC 外设初始化成复位状态，这句是必须的
	RCC_DeInit();

	//使能 HSI
	RCC_HSICmd(ENABLE);
	
	// 等待 HSI 就绪
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	
	// 只有 HSI 就绪之后则继续往下执行
	if (HSIStartUpStatus == RCC_CR_HSIRDY) {
	//-------------------------------------------------------------//

	// 使能 FLASH 预存取缓冲区
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
	// SYSCLK 周期与闪存访问时间的比例设置，这里统一设置成 2
	// 设置成 2 的时候，SYSCLK 低于 48M 也可以工作，如果设置成 0 或者 1 的时候，
	// 如果配置的 SYSCLK 超出了范围的话，则会进入硬件错误，程序就死了
	// 0：0 < SYSCLK <= 24M
	// 1：24< SYSCLK <= 48M
	// 2：48< SYSCLK <= 72M
	FLASH_SetLatency(FLASH_Latency_2);
	//------------------------------------------------------------//
	
	// AHB 预分频因子设置为 1 分频，HCLK = SYSCLK
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
	// APB2 预分频因子设置为 1 分频，PCLK2 = HCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);
	
	// APB1 预分频因子设置为 1 分频，PCLK1 = HCLK/2
	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	//-----------设置各种频率主要就是在这里设置-------------------//
	// 设置 PLL 时钟来源为 HSE，设置 PLL 倍频因子
	// PLLCLK = 4MHz * pllmul
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
	//-- -----------------------------------------------------//
	
	// 开启 PLL
	RCC_PLLCmd(ENABLE);
	
	// 等待 PLL 稳定
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
	}
	
	// 当 PLL 稳定之后，把 PLL 时钟切换为系统时钟 SYSCLK
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	// 读取时钟切换状态位，确保 PLLCLK 被选为系统时钟
	while (RCC_GetSYSCLKSource() != 0x08) {
	}
	} else {
	// 如果 HSI 开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
	// 当 HSE 开启失败或者故障的时候，单片机会自动把 HSI 设置为系统时钟，
	// HSI 是内部的高速时钟，8MHZ
	while (1) {
	}
	}
}

int main(void)
{
	/* reset handle had init sysclk */
	/* modify ext freq HSE_VALUE stm32f10x.h */

	HSI_SetSysClock(RCC_PLLMul_9);

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









