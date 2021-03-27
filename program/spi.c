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

//用于433Mhz模块  SX1278发射机
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//SPI1挂载在APB2 时钟总线   使能 APB2 SPI1时钟和GPIOA时钟
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  //复用推挽输出
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

	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
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

//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量
void SPI_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输

	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
}

//开启一次DMA传输
void SPI_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );
	DMA_SetCurrDataCounter(DMA_CHx,len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

//SPI1 的通讯速率设置函数
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
	SPI_Init(SPI1, &SPI1_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
}

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
	u16 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>1000)return 0;
	}
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>1000)return 0;
	}
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}




