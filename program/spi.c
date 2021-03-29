#include "stm32f10x.h"
#include "spi.h"

//SpeedSet: APB2 = 72Mhz
//SPI_BaudRatePrescaler_2         (SPI 36M@sys			72M)
//SPI_BaudRatePrescaler_4         (SPI 18M@sys			72M)
//SPI_BaudRatePrescaler_8         (SPI 9M@sys			72M)
//SPI_BaudRatePrescaler_16        (SPI 4.5M@sys			72M)
//SPI_BaudRatePrescaler_32        (SPI 2.25M@sys		72M)
//SPI_BaudRatePrescaler_64        (SPI 1.125M@sys 		72M)
//SPI_BaudRatePrescaler_128       (SPI 562.50Kbps@sys 72M)
//SPI_BaudRatePrescaler_256       (SPI 281.25Kbps@sys 72M)

SPI_InitTypeDef  SPI1_InitStructure;

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);

	SPI1_InitStructure.SPI_Direction	= SPI_Direction_2Lines_FullDuplex;
	SPI1_InitStructure.SPI_Mode			= SPI_Mode_Master;
	SPI1_InitStructure.SPI_DataSize		= SPI_DataSize_8b;
	SPI1_InitStructure.SPI_CPOL			= SPI_CPOL_Low;
	SPI1_InitStructure.SPI_CPHA			= SPI_CPHA_1Edge;
	SPI1_InitStructure.SPI_NSS			= SPI_NSS_Soft;
	SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI1_InitStructure.SPI_FirstBit				= SPI_FirstBit_MSB;
	SPI1_InitStructure.SPI_CRCPolynomial		= 7;
	SPI_Init(SPI1, &SPI1_InitStructure);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	SPI_Cmd(SPI1, ENABLE);
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

void SPI_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);

	DMA_DeInit(DMA_CHx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = cndtr;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA_CHx, &DMA_InitStructure);
}

void SPI_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );
	DMA_SetCurrDataCounter(DMA_CHx,len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

void SPI1_SetSpeed(uint8_t SpeedSet)
{
	SPI1_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI1, &SPI1_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
}

uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	uint16_t retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>1000)return 0;
	}
	SPI_I2S_SendData(SPI1, TxData);
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>1000)return 0;
	}
	return SPI_I2S_ReceiveData(SPI1);
}




