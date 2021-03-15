#include "Struct.h"
#include "ws2812.h"
#include "stm32f10x_adc.h"
#include "ring_buffer.h"

RINGBUFF_T tx_ring;
RINGBUFF_T rx_ring;
uint8_t tx_buffer[512]  __attribute__ ((aligned (4)));
uint8_t rx_buffer[1024] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_rx;

typedef struct {
	uint8_t  head;
	uint8_t  type;
	uint16_t length;
	uint16_t id;
} __attribute__ ((packed)) RACE_header_Hdr;

typedef struct {
	uint8_t  head;
	uint8_t  type;
	uint16_t length;
	uint16_t id;
	uint8_t  data[0];
} __attribute__ ((packed)) RACE_Hdr;

typedef struct {
	uint32_t magic_number; //55A55BB
	uint32_t enlarge;
	float    current;  // mA
	float    voltage;  // V
	
} __attribute__ ((packed)) SYS_PARAMETER_Hdr;

SYS_PARAMETER_Hdr sys_par;

//05 5A 02 00 00 0F     校准零点
//05 5C 03 00 00 0F 00  校准成功

//05 5A 02 00 00 1F     校准1毫欧
//05 5C 03 00 00 1F 00  校准成功

//05 5A 02 00 10 0F					获取恒流参数
//05 5C 07 00 10 0F 00 1E 00 00 00 	返回

//05 5A 06 00 11 0F 1E 00 00 00		设置恒流参数
//05 5C 03 00 11 0F 00  			设置成功





void calibrate_zero_parameter(void);
void calibrate_1mr_parameter(void);
void get_current_parameter(void);
void set_current_parameter(uint32_t val);
void load_flash_parameter(void);
void save_flash_parameter(void);

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3
void  Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1 , ENABLE ); //使能ADC1通道时钟


	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA6 2.5V ±1mV 基准输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);  //复位ADC0,将外设 ADC0 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1

	ADC_ResetCalibration(ADC1);	//使能复位校准

	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

	ADC_StartCalibration(ADC1);	 //开启AD校准

	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


void demo_task(void *pvParameters)
{
	uint32_t count;
	uint32_t ADC_IN, ADC_REF;
	float V_sys0, V_sys1, V_Res, Res;

	uint8_t res_buffer[64];
	typedef struct {
		RACE_header_Hdr hdr;
		uint32_t v_sys;		// 系统电压
		uint32_t v_offset;	// 零点偏置电压
		uint32_t Res;		// 检测电阻阻值 
	} __attribute__ ((packed)) RES, *p_RES;

	p_RES p_point = (p_RES)res_buffer;
	Data data;

	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		if (++count%2 == 0) {
			LED_1 = 1; LED_G = 1;;
		} else {
			LED_1 = 0; LED_G = 0;;
		}

		// ADC_IN  = Get_Adc(0);
		// ADC_REF = Get_Adc(6);

		V_sys0 = (3.300f / 4096.0f) * (float)ADC_IN;  //直接计算偿值
		V_sys1 = (4096.0f * 2.500f) / (float)ADC_REF; //基准计算值

		V_Res = (2.500f * (float)ADC_IN) / (float)ADC_REF + sys_par.voltage;
		Res = (V_Res / sys_par.current / 1000.0f ) * 1000.0f;

		/* pack */
		p_point->hdr.head   = 0x05;
		p_point->hdr.type   = 0x5C;
		p_point->hdr.length = sizeof(RES) - 4;
		p_point->hdr.id     = 0x0F20;
		Data_Conversion(V_sys1, data, (uint8_t*)&p_point->v_sys);
		Data_Conversion(sys_par.voltage, data, (uint8_t*)&p_point->v_offset);
		Data_Conversion(Res, data, (uint8_t*)&p_point->Res);
		uart2_send_string(res_buffer, sizeof(RES));

		/* print log */
		// debug_printf("\r\n");
		// debug_printf("ADC_IN=%d  V_sys0=%f V_sys1=%f \r\n", ADC_IN, V_sys0, V_sys1);
		debug_printf("V_Res = %f Res = %0.5f \r\n", V_Res, Res);

		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1);
	}
}

void race_task(void *pvParameters)
{
	uint32_t recevice_length;
	uint8_t recevice_buffer[64];
	RACE_Hdr *p_race_hdr;
	while (1)
	{
		if( xSemaphoreTake( xSemaphore_rx, portMAX_DELAY) == pdPASS ) {
			recevice_length = RingBuffer_GetCount(&rx_ring);
			if (recevice_length > 4) {
				memset(recevice_buffer, 0, sizeof(recevice_buffer));
				RingBuffer_PopMult(&rx_ring, &recevice_buffer[0], recevice_length);
				p_race_hdr = (RACE_Hdr*)&recevice_buffer[0];
				if ((p_race_hdr->head == 0x05) && (p_race_hdr->type == 0x5A) && (p_race_hdr->length != 0)) {
					if (p_race_hdr->id == 0x0F00) { // 校准零点命令
						calibrate_zero_parameter();
					} else if (p_race_hdr->id == 0x0F01) {
						// calibrate_1mr_parameter();
					} else if (p_race_hdr->id == 0x0F10) { // 获取电流参数命令
						get_current_parameter();
					} else if (p_race_hdr->id == 0x0F11) { // 设置电流参数命令
						if (p_race_hdr->length == 6) {
							uint32_t data;
							data = *(uint32_t*)p_race_hdr->data;
							set_current_parameter(data);
						}
					}
				}
			}
		}
	}
}


//用于创建任务的任务
void create_app_task(void)
{
	RingBuffer_Init(&tx_ring, tx_buffer, 1, sizeof(tx_buffer));
	RingBuffer_Init(&rx_ring, rx_buffer, 1, sizeof(rx_buffer));

	xSemaphore_rx = xSemaphoreCreateBinary();

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

void calibrate_zero_parameter(void)
{
	uint32_t i;
	uint32_t ADC_IN, ADC_REF;
	float V_Res = 0;

	uint8_t res_buffer[64];

	typedef struct {
		RACE_header_Hdr hdr;
		uint8_t status;
	} __attribute__ ((packed)) RES, *p_RES;

	for (i = 0; i < 10; i++) {
		ADC_IN  = Get_Adc(0);
		ADC_REF = Get_Adc(6);

		V_Res += (2.500f * (float)ADC_IN) / (float)ADC_REF;
		debug_printf("ADC_IN=%d ADC_REF=%d V_Res = %f \r\n", ADC_IN, ADC_REF, V_Res);
	}

	sys_par.voltage  = 0.0f - (V_Res / 10.0f);
	debug_printf("calibrate done vol_offset = %f \r\n", -sys_par.voltage);
	save_flash_parameter();

	/* respose */
	p_RES p_point= (p_RES)res_buffer;
	p_point->hdr.head   = 0x05;
	p_point->hdr.type   = 0x5C;
	p_point->hdr.length = sizeof(RES) - 4;
	p_point->hdr.id     = 0x0F00;
	p_point->status	  = 0x00;
	uart2_send_string(res_buffer, sizeof(RES));
}

void calibrate_1mr_parameter(void)
{
	uint32_t i;
	uint32_t ADC_IN, ADC_REF;
	float V_Res = 0;

	uint8_t res_buffer[64];

	typedef struct {
		RACE_header_Hdr hdr;
		uint8_t status;
	} __attribute__ ((packed)) RES, *p_RES;

	for (i = 0; i < 10; i++) {
		ADC_IN  = Get_Adc(0);
		ADC_REF = Get_Adc(6);

		V_Res += (2.500f * (float)ADC_IN) / (float)ADC_REF;
	}

	V_Res = V_Res / 10.0f + sys_par.voltage;

	debug_printf("ADC_IN=%d ADC_REF=%d V_Res = %f \r\n", ADC_IN, ADC_REF, V_Res);

	sys_par.enlarge = (uint32_t)((V_Res * 1000000.0f) / sys_par.current);

	debug_printf("calibrate 1mr done enlarge = %d \r\n", sys_par.enlarge);

	save_flash_parameter();

	/* respose */
	p_RES p_point = (p_RES)res_buffer;
	p_point->hdr.head   = 0x05;
	p_point->hdr.type   = 0x5C;
	p_point->hdr.length = sizeof(RES) - 4;
	p_point->hdr.id     = 0x0F01;
	p_point->status	  = 0x00;
	uart2_send_string(res_buffer, sizeof(RES));
}

void get_current_parameter(void)
{
	uint8_t res_buffer[64];

	typedef struct {
		RACE_header_Hdr hdr;
		uint8_t status;
		unsigned int current;
	} __attribute__ ((packed)) RES, *p_RES;

	/* respose */
	p_RES p_point = (p_RES)res_buffer;
	p_point->hdr.head   = 0x05;
	p_point->hdr.type   = 0x5C;
	p_point->hdr.length = sizeof(RES) - 4;
	p_point->hdr.id     = 0x0F10;
	p_point->status	  = 0x00;
	p_point->current    = (int)sys_par.current;
	uart2_send_string(res_buffer, sizeof(RES));

	debug_printf("get current=%fmA \r\n", sys_par.current);
}

void set_current_parameter(uint32_t val)
{
	uint8_t res_buffer[64];

	typedef struct {
		RACE_header_Hdr hdr;
		uint8_t status;
	} __attribute__ ((packed)) RES, *p_RES;

	sys_par.current = (float)val;

	/* respose */
	p_RES p_point = (p_RES)res_buffer;
	p_point->hdr.head   = 0x05;
	p_point->hdr.type   = 0x5C;
	p_point->hdr.length = sizeof(RES) - 4;
	p_point->hdr.id     = 0x0F11;
	p_point->status	  = 0x00;
	uart2_send_string(res_buffer, sizeof(RES));

	save_flash_parameter();

	debug_printf("set current=%fmA \r\n", sys_par.current);
}

void load_flash_parameter(void)
{
	uint8_t buffer[64];
	Data data;

	STMFLASH_Read(FLASH_SAVE_ADDR, (uint16_t*)&buffer[0], sizeof(SYS_PARAMETER_Hdr)/2);
	memcpy((uint8_t*)&sys_par.magic_number, (uint8_t*)&buffer[0], 4);
	memcpy((uint8_t*)&sys_par.enlarge,      (uint8_t*)&buffer[4], 4);

	if (sys_par.magic_number != 0x55AA55BB) {
		debug_printf("load_flash_parameter fail \r\n");
		/* default patameter */
		sys_par.magic_number = 0x55AA55BB;
		sys_par.enlarge      = 1000;
		sys_par.current = 30.0f;
		sys_par.voltage = 0.0f;

		/* conversion float data */
		memcpy((uint8_t*)&buffer[0], (uint8_t*)&sys_par.magic_number, 4);
		Data_Conversion(sys_par.current, data, (uint8_t*)&buffer[8]);
		Data_Conversion(sys_par.voltage, data, (uint8_t*)&buffer[12]);

		/* write flash */
		STMFLASH_Write(FLASH_SAVE_ADDR, (uint16_t*)&buffer[0], sizeof(SYS_PARAMETER_Hdr)/2);
	} else {
		debug_printf("load_flash_parameter ok \r\n");
		memcpy((uint8_t*)&sys_par.magic_number, (uint8_t*)&buffer[0], 4);
		memcpy((uint8_t*)&sys_par.enlarge,      (uint8_t*)&buffer[4], 4);
		sys_par.current = Data_Conversion_float((uint8_t*)&buffer[8]),
		sys_par.voltage = Data_Conversion_float((uint8_t*)&buffer[12]);
	}

	debug_printf("magic=0x%08x enlarge=%d vol=%fV current=%fmA \r\n", 
						sys_par.magic_number, sys_par.enlarge, sys_par.voltage, sys_par.current);
}

void save_flash_parameter(void)
{
	uint8_t buffer[64];
	Data data;

	/* default patameter */
	sys_par.magic_number = 0x55AA55BB;

	/* conversion float data */
	memcpy((uint8_t*)&buffer[0], (uint8_t*)&sys_par.magic_number, 4);
	memcpy((uint8_t*)&buffer[4], (uint8_t*)&sys_par.enlarge, 4);
	Data_Conversion(sys_par.current, data, (uint8_t*)&buffer[8]);
	Data_Conversion(sys_par.voltage, data, (uint8_t*)&buffer[12]);

	/* write flash */
	STMFLASH_Write(FLASH_SAVE_ADDR, (uint16_t*)&buffer[0], sizeof(SYS_PARAMETER_Hdr)/2);

	debug_printf("magic=0x%08x enlarge=%d vol=%fV current=%fmA \r\n", 
						sys_par.magic_number, sys_par.enlarge, sys_par.voltage, sys_par.current);
}

int main(void)
{
	// Stm32_Clock_Init(6); //系统时钟设置  12Mhz * 6 = 72MHZ
	Stm32_Clock_Init(9); //系统时钟设置  8Mhz * 9 = 72MHZ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组4

	/* 初始化LED灯 */
	LED_init();

	/* 初始化串口 */
	UART1_Init(921600); //总线时钟：72Mhz
	UART2_Init(921600); //总线时钟：36Mhz

	/* 10khz的计数频率，中断一次  100us一次 */
	TIM3_Int_Init(999,7199);

	/* 初始化ADC */
	Adc_Init();

	/* 从flash中加载计算参数 */
	load_flash_parameter();

	/* 创建task */
	create_app_task();

	/* 开启任务调度 */
	vTaskStartScheduler();

	return 0;
}









