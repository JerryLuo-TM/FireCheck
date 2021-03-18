#include "stm32f10x.h"
#include "spi.h"

//������ SPI1 ģ��ĳ�ʼ�����룬���ó�����ģʽ

//SpeedSet: APB2 = 72Mhz
//SPI_BaudRatePrescaler_2         (SPI 36M@sys     		72M)
//SPI_BaudRatePrescaler_4         (SPI 18M@sys   			72M)
//SPI_BaudRatePrescaler_8         (SPI 9M@sys    			72M)
//SPI_BaudRatePrescaler_16        (SPI 4.5M@sys  			72M)
//SPI_BaudRatePrescaler_32        (SPI 2.25M@sys 			72M)
//SPI_BaudRatePrescaler_64        (SPI 1.125M@sys 		72M)
//SPI_BaudRatePrescaler_128       (SPI 562.50Kbps@sys 72M)
//SPI_BaudRatePrescaler_256       (SPI 281.25Kbps@sys 72M)

SPI_InitTypeDef  SPI1_InitStructure;

//����433Mhzģ��  SX1278�����
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//SPI1������APB2 ʱ������   ʹ�� APB2 SPI1ʱ�Ӻ�GPIOAʱ��
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);

	SPI1_InitStructure.SPI_Direction	= SPI_Direction_2Lines_FullDuplex;	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI1_InitStructure.SPI_Mode			= SPI_Mode_Master;	//����SPI����ģʽ:����Ϊ��SPI
	SPI1_InitStructure.SPI_DataSize		= SPI_DataSize_8b;	//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI1_InitStructure.SPI_CPOL			= SPI_CPOL_High;	//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI1_InitStructure.SPI_CPHA			= SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI1_InitStructure.SPI_NSS			= SPI_NSS_Soft;	//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI1_InitStructure.SPI_FirstBit				= SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI1_InitStructure.SPI_CRCPolynomial		= 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI1_InitStructure);	//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);

	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
}


void SPI1_DMA_IT_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruture;
	DMA_ITConfig(SPI_TX_DMA_CHANNEL,DMA_IT_TC,ENABLE);

	NVIC_InitStruture.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStruture.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruture.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruture);
}

//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ�����
void SPI_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����

	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
}

//����һ��DMA����
void SPI_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );
	DMA_SetCurrDataCounter(DMA_CHx,len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

//SPI1 ��ͨѶ�������ú���
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI1, &SPI1_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
}

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
	u16 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>1000)return 0;
	}
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>1000)return 0;
	}
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����
}

//-----------------------------------�ָ���-----------------------------------


//������SPI2ģ��ĳ�ʼ�����룬���ó�����ģʽ
//SpeedSet:  APB1 = 36Mhz
//SPI_BaudRatePrescaler_2         (SPI 18M@sys   			 36M)
//SPI_BaudRatePrescaler_4         (SPI 9M@sys    			 36M)
//SPI_BaudRatePrescaler_8         (SPI 4.5M@sys  			 36M)
//SPI_BaudRatePrescaler_16        (SPI 2.25M@sys 		   36M)
//SPI_BaudRatePrescaler_32        (SPI 1.125M@sys 		 36M)
//SPI_BaudRatePrescaler_64        (SPI 562.50Kbps@sys  36M)
//SPI_BaudRatePrescaler_128       (SPI 281.25Kbps@sys  36M)
//SPI_BaudRatePrescaler_256       (SPI 140.625Kbps@sys 36M)

SPI_InitTypeDef  SPI2_InitStructure;
//����915Mhz SX1276���ջ�
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//SPI2������APB1 ʱ������   ʹ�� APB1 SPI2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE );
	//ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

	SPI2_InitStructure.SPI_Direction	= SPI_Direction_2Lines_FullDuplex;	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI2_InitStructure.SPI_Mode	= SPI_Mode_Master;	//����SPI����ģʽ:����Ϊ��SPI
	SPI2_InitStructure.SPI_DataSize	= SPI_DataSize_8b;	//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI2_InitStructure.SPI_CPOL	= SPI_CPOL_Low;	//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI2_InitStructure.SPI_CPHA	= SPI_CPHA_1Edge;	//���ݲ����ڵڶ���ʱ����
	SPI2_InitStructure.SPI_NSS	= SPI_NSS_Soft;	//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI2_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //���岨����Ԥ��Ƶ��ֵ
	SPI2_InitStructure.SPI_FirstBit	= SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI2_InitStructure.SPI_CRCPolynomial	= 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI2_InitStructure);	//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����

	SPI2_ReadWriteByte(0xff);//��������
}

//SPI2 ��ͨѶ�������ú���
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI2_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI2, &SPI2_InitStructure);
	SPI_Cmd(SPI2,ENABLE);
}

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
	u16 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>1000)return 0;
	}
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>1000)return 0;
	}
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����
}


//---------------------END--------------------------------




