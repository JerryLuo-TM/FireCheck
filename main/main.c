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

//05 5A 02 00 00 0F     У׼���
//05 5C 03 00 00 0F 00  У׼�ɹ�

//05 5A 02 00 00 1F     У׼1��ŷ
//05 5C 03 00 00 1F 00  У׼�ɹ�

//05 5A 02 00 10 0F					��ȡ��������
//05 5C 07 00 10 0F 00 1E 00 00 00 	����

//05 5A 06 00 11 0F 1E 00 00 00		���ú�������
//05 5C 03 00 11 0F 00  			���óɹ�





void calibrate_zero_parameter(void);
void calibrate_1mr_parameter(void);
void get_current_parameter(void);
void set_current_parameter(uint32_t val);
void load_flash_parameter(void);
void save_flash_parameter(void);

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3
void  Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1 , ENABLE ); //ʹ��ADC1ͨ��ʱ��


	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA6 2.5V ��1mV ��׼����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);  //��λADC0,������ ADC0 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼

	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

	ADC_StartCalibration(ADC1);	 //����ADУ׼

	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}


void demo_task(void *pvParameters)
{
	uint32_t count;
	uint32_t ADC_IN, ADC_REF;
	float V_sys0, V_sys1, V_Res, Res;

	uint8_t res_buffer[64];
	typedef struct {
		RACE_header_Hdr hdr;
		uint32_t v_sys;		// ϵͳ��ѹ
		uint32_t v_offset;	// ���ƫ�õ�ѹ
		uint32_t Res;		// ��������ֵ 
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

		V_sys0 = (3.300f / 4096.0f) * (float)ADC_IN;  //ֱ�Ӽ��㳥ֵ
		V_sys1 = (4096.0f * 2.500f) / (float)ADC_REF; //��׼����ֵ

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
					if (p_race_hdr->id == 0x0F00) { // У׼�������
						calibrate_zero_parameter();
					} else if (p_race_hdr->id == 0x0F01) {
						// calibrate_1mr_parameter();
					} else if (p_race_hdr->id == 0x0F10) { // ��ȡ������������
						get_current_parameter();
					} else if (p_race_hdr->id == 0x0F11) { // ���õ�����������
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


//���ڴ������������
void create_app_task(void)
{
	RingBuffer_Init(&tx_ring, tx_buffer, 1, sizeof(tx_buffer));
	RingBuffer_Init(&rx_ring, rx_buffer, 1, sizeof(rx_buffer));

	xSemaphore_rx = xSemaphoreCreateBinary();

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
	// Stm32_Clock_Init(6); //ϵͳʱ������  12Mhz * 6 = 72MHZ
	Stm32_Clock_Init(9); //ϵͳʱ������  8Mhz * 9 = 72MHZ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����4

	/* ��ʼ��LED�� */
	LED_init();

	/* ��ʼ������ */
	UART1_Init(921600); //����ʱ�ӣ�72Mhz
	UART2_Init(921600); //����ʱ�ӣ�36Mhz

	/* 10khz�ļ���Ƶ�ʣ��ж�һ��  100usһ�� */
	TIM3_Int_Init(999,7199);

	/* ��ʼ��ADC */
	Adc_Init();

	/* ��flash�м��ؼ������ */
	load_flash_parameter();

	/* ����task */
	create_app_task();

	/* ����������� */
	vTaskStartScheduler();

	return 0;
}









