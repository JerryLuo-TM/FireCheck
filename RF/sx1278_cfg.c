#include "struct.h"

unsigned char   SX1278_Lora_Rate_Sel = 0x02;  //扩频因子为7
unsigned char   SX1278_BandWide_Sel  = 0x08;  //500KHZ

//扩频因子越大，传播时间越长

/* 扩频因子 */
const unsigned char SX1278_SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

/* 带宽 */
const unsigned char SX1278_LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

//关闭CRC reg:0x30 bit4 [0：关  1：开]
//        reg:0x1E bit2 [0：关  1：开]
void SX1278_Config(void)
{
	  SX1278_LoRa_SetMode(LORA_MODE_SLEEP);//reg:0x01 进入睡眠模式
	  SX1278_SetChannel(SX1278_frequency); //设置载波频率
		//设置基本参数 
	  SX1278_SetMode( MODE_LORA );		        //进入LOAR 模式 访问LOAR寄存器
		SX1278_WriteReg(REG_LR_PACONFIG,0xFF);  //0x09 TX 设置输出增益   20dBm
	  SX1278_WriteReg(REG_LR_PARAMP,0x09);    //0x0A TX LORA模式下 斜升/斜降
		SX1278_WriteReg(REG_LR_OCP,0x0B);       //0x0B TX 过流保护控制，关闭PA过流保护
		//SX1278_WriteReg(REG_LR_LNA,0x38);     //0x0C RX LAN增益设置为最大 提升灵敏度
    SX1278_WriteReg(REG_LR_PAYLOADMAXLENGTH,0xFF); //0x23 负载长度最大值  超出将会被滤除
	  //设置接收机  FSK  模式
	  SX1278_SetMode( MODE_FSK );              //0x01 进入FSK调试
	  SX1278_WriteReg(REG_RXCONFIG,0x1E);      //0x0D
	  SX1278_WriteReg(REG_RSSICONFIG,0xD2);    //0x0E
	  SX1278_WriteReg(REG_AFCFEI,0x01);        //0x1A
	  SX1278_WriteReg(REG_PREAMBLEDETECT,0xAA);//0x1f
	  SX1278_WriteReg(REG_OSC,0x07);           //0x24 RC震荡器 CLOCKOUT关闭
	  SX1278_WriteReg(REG_SYNCCONFIG,0x12);    //0x27 开启同步字 2+1 字节
	  SX1278_WriteReg(REG_SYNCVALUE1,0xC1);    //0x28
	  SX1278_WriteReg(REG_SYNCVALUE2,0x94);    //0x29
	  SX1278_WriteReg(REG_SYNCVALUE3,0xC1);    //0x2A
		SX1278_WriteReg(REG_PACKETCONFIG1,0xD8); //0x30
		SX1278_WriteReg(REG_FIFOTHRESH,0x8F);    //0x35
		
		 // 服务寄存器
		SX1278_WriteReg(REG_IMAGECAL,0x02);
	
	  SX1278_SetMode( MODE_LORA );				    //进入LOAR 模式 访问LOAR寄存器
		//下面这两行设置扩频因子和带宽  不适用于 扩频因子=6
	  
	  //0x1D 信号带宽 + 纠错编码率 + 显示报头     4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1278_WriteReg(REG_LR_MODEMCONFIG1,(SX1278_LoRaBwTbl[SX1278_BandWide_Sel]<<4) | (0x01<<1) | 0x00 );
	  //0x1E 扩频因子 + 发送模式 + 发送端CRC + RX超时
		SX1278_WriteReg(REG_LR_MODEMCONFIG2,(SX1278_SpreadFactorTbl[SX1278_Lora_Rate_Sel]<<4) | (1<<2) | 0x03 ); 

		SX1278_WriteReg(REG_LR_SYMBTIMEOUTLSB,0xFF);//0x1F 接收机超时 = 0x3FF(Max) 
		SX1278_WriteReg(REG_LR_PREAMBLEMSB,0x00);   //0x20 前导码 高字节
		SX1278_WriteReg(REG_LR_PREAMBLELSB,12);     //0x21 前导码 低字节 前导码=8+4=12字节 
		
		SX1278_WriteReg(REG_LR_DIOMAPPING1,0x00);   //0x40 引脚映射
		SX1278_WriteReg(REG_LR_DIOMAPPING2,0x01);   //0x41 引脚映射   
		
    SX1278_LoRa_SetMode( LORA_MODE_STDBY );//进入待机模式
}

/**********************************************************
**名字:     SX1278_7_8_LoRaEntryTx
**功能:     Entry Tx mode
**输入:     None
**输出:     0-超时错误
**********************************************************/
unsigned char SX1278_LoRaEntryTx(void)
{
  static unsigned char Vesion,addr,tmp;

  SX1278_Config();               //配置基本参数
  SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //进入待机模式
	
	//访问LORA寄存器  
	SX1276_SetMode( MODE_LORA );				          //进入LOAR 模式 访问LOAR寄存器
	SX1278_WriteReg(REG_LR_HOPPERIOD,0x00);       //0x24 无跳频周期  关闭
  SX1278_WriteReg(REG_LR_PADAC,0x87);           //0x4D 高功率设置 
 
  SX1278_WriteReg(REG_LR_IRQFLAGSMASK,0xF7);    //0x11 中断屏蔽寄存器  开启 【 TXDone 】 中断
	SX1278_WriteReg(REG_LR_IRQFLAGS,0xFF);        //0x12 清除中断标识
	
  SX1278_WriteReg(REG_LR_PAYLOADLENGTH,1);      //0x22 负载长度 不可写0 (在扩频因子为6时数据大于一字节此寄存器必须配置) 
  
  addr = SX1278_ReadReg( REG_LR_FIFOTXBASEADDR );//FiFo数据缓冲区中发送调制器的写入基地址
  SX1278_WriteReg(REG_LR_FIFOADDRPTR,addr);      //Fifo数据缓冲区中SPI接口地址指针

	tmp = SX1278_ReadReg(REG_LR_OPMODE);         //0x01 读取通用寄存器
	tmp |= 0x08;                                 //清除高频访问模式位
	SX1278_WriteReg(REG_LR_OPMODE,tmp);          //0x01 433模块 配置高频模式
	
	SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //进入待机模式
	vTaskDelay(configTICK_RATE_HZ/100);     //等待完成

	Vesion=SX1278_ReadReg(REG_LR_VERSION); //0x42 读取版本
	if(Vesion==0x12)
			return 1;
    else
			return 0;
}

/**********************************************************
**名字:     SX1278_7_8_LoRaEntryRx
**功能:     进入LORa RX模式
**输入:     None
**输出:     0-超时错误
**********************************************************/
unsigned char SX1278_LoRaEntryRx(void)
{
	return 0;
}

unsigned char SX1278_LoRaRxPacket(unsigned char *buffer)
{
  return 0;
}

void SX1278_LoRaTxPacket(unsigned char *buffer,unsigned char length)
{
	SX1278_WriteReg( REG_LR_PAYLOADLENGTH, length ); //写包长
	SX1278_WriteReg( REG_LR_FIFOTXBASEADDR, 0x00 ); // Full buffer used for Tx
  SX1278_WriteReg( REG_LR_FIFOADDRPTR, 0x00 );
	// FIFO operations can not take place in Sleep mode
	if( ( SX1278_ReadReg( REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
	{
			SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //进入待机模式
	}
	SX1278_WriteFifo( buffer, length );
	SX1278_WriteReg(REG_LR_OPMODE,0x8b);  //Tx Mode  433低频模式
  while(!(SX1278_ReadReg( REG_LR_IRQFLAGS )&0x08)); //TxDone
	SX1278_WriteReg( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );//清除中断
	SX1278_LoRa_SetMode( LORA_MODE_STDBY );
}

/*!
 * Performs the Rx chain calibration for LF and HF bands
 * \remark Must be called just after the reset so all registers are at their
 *         default values
 *  官方代码中的校准程序
 */
void SX1278_RxChainCalibration( void )
{
    uint8_t regPaConfigInitVal;
    uint32_t initialFreq;
    // Save context 
    regPaConfigInitVal = SX1278_ReadReg( REG_PACONFIG );
    initialFreq = ( double )( ( ( uint32_t )SX1278_ReadReg( REG_FRFMSB ) << 16 ) |
                              ( ( uint32_t )SX1278_ReadReg( REG_FRFMID ) << 8 ) |
                              ( ( uint32_t )SX1278_ReadReg( REG_FRFLSB ) ) ) * ( double )FREQ_STEP;

    // Cut the PA just in case, RFO output, power = -1 dBm
    SX1278_WriteReg( REG_PACONFIG, 0x00 );

    // Launch Rx chain calibration for LF band
    SX1278_WriteReg( REG_IMAGECAL, ( SX1278_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1278_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Sets a Frequency in HF band
    SX1278_SetChannel( 434000000 );

    // Launch Rx chain calibration for HF band
    SX1278_WriteReg( REG_IMAGECAL, ( SX1278_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1278_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Restore context
    SX1278_WriteReg( REG_PACONFIG, regPaConfigInitVal );
    SX1278_SetChannel( initialFreq );
}

//PB0 1 2 10 TX:CSN TXE RXE RST
void RF433_Init(unsigned char mode)
{
	unsigned char temp;
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	   //使能PA,PB端口时钟
  
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0
																 |GPIO_Pin_1
																 |GPIO_Pin_2
																 |GPIO_Pin_10;            //端口配置
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    		//IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI1_Init();         //TX for 433Mhz
	SX1278_SleepMode();  //接收和发送使能关闭
	SX1278_Reset();      //硬件复位
	SX1278_RxChainCalibration();//接收通道校准
	temp=SX1278_LoRaEntryTx();  //LOAR发送配置
	SX1278_SetSwitchTx();       //硬件使能发送
	if(temp==0){R_LED=0;}       //初始化失败点亮红灯
}

