#include "struct.h"

unsigned char   SX1276_Lora_Rate_Sel = 0x01;  //扩频因子为7
unsigned char   SX1276_BandWide_Sel  = 0x09;  //500KHZ

//扩频因子越大，传播时间越长

/* 扩频因子 */
const unsigned char SX1276_SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

/* 带宽 */
const unsigned char SX1276_LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

void SX1276_Config(void)
{
	  SX1276_LoRa_SetMode( LORA_MODE_SLEEP );//改变当前模式必须进入睡眠模式
	  SX1276_SetChannel(SX1276_frequency);   //设置载波频率
		//设置基本参数 
		SX1276_SetMode( MODE_LORA );				 //进入LOAR 模式 访问LOAR寄存器
		SX1276_WriteReg(REG_LR_PACONFIG,0xFF);//0x09 TX 设置输出增益   20dBm
	  SX1276_WriteReg(REG_LR_PARAMP,0x09);  //0x0A TX LORA模式下 斜升/斜降
		SX1276_WriteReg(REG_LR_OCP,0x0B);     //0x0B TX 过流保护控制，关闭PA过流保护
		SX1276_WriteReg(REG_LR_LNA,0x23);     //0x0C RX LAN增益设置为最大 提升灵敏度
		SX1276_WriteReg(REG_LR_PAYLOADMAXLENGTH,0xFF); //0x23 负载长度最大值  超出将会被滤除
		  //设置接收机  FSK  模式
	  SX1276_SetMode( MODE_FSK );              //0x01 进入FSK调试
	  SX1276_WriteReg(REG_RXCONFIG,0x1E);      //0x0D
	  SX1276_WriteReg(REG_RSSICONFIG,0xD2);    //0x0E
	  SX1276_WriteReg(REG_AFCFEI,0x01);        //0x1A
	  SX1276_WriteReg(REG_PREAMBLEDETECT,0xAA);//0x1f
	  SX1276_WriteReg(REG_OSC,0x07);           //0x24 RC震荡器 CLOCKOUT关闭
	  SX1276_WriteReg(REG_SYNCCONFIG,0x12);    //0x27 开启同步字 2+1 字节
	  SX1276_WriteReg(REG_SYNCVALUE1,0xC1);    //0x28
	  SX1276_WriteReg(REG_SYNCVALUE2,0x94);    //0x29
	  SX1276_WriteReg(REG_SYNCVALUE3,0xC1);    //0x2A
		SX1276_WriteReg(REG_PACKETCONFIG1,0xD8|(2<<1)); //0x30 包控制  是否开启节点地址筛选
		
//		SX1276_WriteReg(REG_NODEADRS,0xAA);      //0x33    节点地址
//		SX1276_WriteReg(REG_BROADCASTADRS,0xAA); //0x34    广播地址
		
		SX1276_WriteReg(REG_FIFOTHRESH,0x8F);    //0x35
		 // 服务寄存器
		SX1276_WriteReg(REG_IMAGECAL,0x02);
	
	  SX1276_SetMode( MODE_LORA );				     //进入LOAR 模式 访问LOAR寄存器
		//下面这两行设置扩频因子和带宽  不适用于 扩频因子=6
	  
	  //0x1D 信号带宽 + 纠错编码率 + 显示报头     4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1276_WriteReg(REG_LR_MODEMCONFIG1,(SX1276_LoRaBwTbl[SX1276_BandWide_Sel]<<4) | (0x01<<1) | 0x00 );
	  //0x1E 扩频因子 + 发送模式 + 发送端CRC + RX超时
		SX1276_WriteReg(REG_LR_MODEMCONFIG2,(SX1276_SpreadFactorTbl[SX1276_Lora_Rate_Sel]<<4) | (1<<2) | 0x03 ); 

		SX1276_WriteReg(REG_LR_SYMBTIMEOUTLSB,0xFF);//0x1F 接收机超时 = 0x3FF(Max) 
		SX1276_WriteReg(REG_LR_PREAMBLEMSB,0x00);   //0x20 前导码 高字节
		SX1276_WriteReg(REG_LR_PREAMBLELSB,12);     //0x21 前导码 低字节 前导码=8+4=12字节 
		
		SX1276_WriteReg(REG_LR_DIOMAPPING1,0x00);   //0x40 引脚映射
																								//bit[7:6]DIO0 
																								//bit[5:4]DIO1
																								//bit[3:2]DIO2
																								//bit[1:0]DIO3
		SX1276_WriteReg(REG_LR_DIOMAPPING2,0x01);   //0x41 引脚映射   
																								//bit[7:6]DIO2
																								//bit[5:4]DIO2
																								//bit[0]  1：RSSI中断  0：PreambleDetect 中断
		
    SX1276_LoRa_SetMode( LORA_MODE_STDBY );//进入待机模式
}

/**********************************************************
**名字:     SX1276_7_8_LoRaEntryTx
**功能:     Entry Tx mode
**输入:     None
**输出:     0-超时错误
**********************************************************/
unsigned char SX1276_LoRaEntryTx(void)
{
	return 0;
}

/**********************************************************
**名字:     SX1276_7_8_LoRaEntryRx
**功能:     进入LORa RX模式
**输入:     None
**输出:     0-超时错误
**********************************************************/
unsigned char SX1276_LoRaEntryRx(void)
{
  static unsigned char addr,temp;

  SX1276_Config();               //基本参数配置

  SX1276_WriteReg(REG_LR_PADAC,0x84);         //正常的Rx
  SX1276_WriteReg(REG_LR_HOPPERIOD,0xFF);     //无跳频
	
	//DIO0 设置为接收完成信号 
	//DIO1 接收超时
	//DIO2 FhssChange Channel
	//DIO3 CADDone
  SX1276_WriteReg(REG_LR_DIOMAPPING1,0x01);   //DIO0=00, DIO1=00, DIO2=00, DIO3=01

  SX1276_WriteReg(REG_LR_IRQFLAGSMASK,0x3F);  //打开RxDone中断&超时0011 1111
	SX1276_WriteReg(REG_LR_IRQFLAGS,0xFF);      //清除中断标识
	
  SX1276_WriteReg(REG_LR_PAYLOADLENGTH,1);  //21字节(在扩频因子为6时数据大于一字节此寄存器必须配置) 
	
	addr = SX1276_ReadReg(REG_LR_FIFORXBASEADDR);//Read RxBaseAddr
  SX1276_WriteReg(REG_LR_FIFOADDRPTR,addr);    //RxBaseAddr -> FiFoAddrPtr　
  
	//SX1276_WriteReg(REG_LR_OPMODE,0x8d);         //连续Rx模式//低频模式10001101
	SX1276_WriteReg(REG_LR_OPMODE,0x85);           //连续Rx模式//高频模式00000101

	vTaskDelay(configTICK_RATE_HZ/100);    //等待完成

	temp=SX1276_ReadReg(REG_LR_MODEMSTAT)&0x04;//Rx进行检测

	if(temp==0x04)//Rx-on going RegModemStat//RX进行中
		return 1;
	else
		return 0;
}

unsigned char SX1276_LoRaRxPacket(unsigned char *buffer)
{
  unsigned char i; 
  unsigned char packet_size=0;
	unsigned char IRQ_val;
	//读取中断寄存器
	IRQ_val=SX1276_ReadReg(REG_LR_IRQFLAGS);
  if((IRQ_val&0x40)!=0)  //接收中断
  {
		if((IRQ_val&0x20)==0) //CRC 正确
		{
			//接收到最后一个数据包的起始地址（数据缓冲区中）
			//RxBaseAddr -> FiFoAddrPtr    
			SX1276_WriteReg(REG_LR_FIFOADDRPTR,SX1276_ReadReg(REG_LR_FIFORXCURRENTADDR));

			packet_size = SX1276_ReadReg(REG_LR_NBRXBYTES);      //接收的字节数
			for( i=0; i<packet_size; i++ )
			{
				*buffer ++ = SX1276_ReadFifo( );
			}

			SX1276_WriteReg(REG_LR_IRQFLAGS,0xFF);      //清除中断标识
			
			return packet_size;
	  }
		else
		{
			SX1276_WriteReg(REG_LR_IRQFLAGS,0x60);
		}
  }
  return 0;
}

void SX1276_LoRaTxPacket(unsigned char *buffer,unsigned char length)
{
	SX1276_WriteReg( REG_LR_PAYLOADLENGTH, length ); //写包长
	SX1276_WriteReg( REG_LR_FIFOTXBASEADDR, 0x00 ); // Full buffer used for Tx
  SX1276_WriteReg( REG_LR_FIFOADDRPTR, 0x00 );
	SX1276_WriteFifo( buffer, length );
	SX1276_WriteReg(REG_LR_OPMODE,0x8b);  //Tx Mode
  while(!(SX1276_ReadReg( REG_LR_IRQFLAGS )&0x08) ); //TxDone //TxDone
	SX1276_WriteReg( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );//清除中断
	SX1276_LoRa_SetMode( LORA_MODE_STDBY );
}

/*!
 * Performs the Rx chain calibration for LF and HF bands
 * \remark Must be called just after the reset so all registers are at their
 *         default values
 *  官方代码中的校准程序
 */
void SX1276_RxChainCalibration( void )
{
    uint8_t regPaConfigInitVal;
    uint32_t initialFreq;
    // Save context 
    regPaConfigInitVal = SX1276_ReadReg( REG_PACONFIG );
    initialFreq = ( double )( ( ( uint32_t )SX1276_ReadReg( REG_FRFMSB ) << 16 ) |
                              ( ( uint32_t )SX1276_ReadReg( REG_FRFMID ) << 8 ) |
                              ( ( uint32_t )SX1276_ReadReg( REG_FRFLSB ) ) ) * ( double )FREQ_STEP;

    // Cut the PA just in case, RFO output, power = -1 dBm
    SX1276_WriteReg( REG_PACONFIG, 0x00 );

    // Launch Rx chain calibration for LF band
    SX1276_WriteReg( REG_IMAGECAL, ( SX1276_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1276_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Sets a Frequency in HF band
    SX1276_SetChannel( 868000000 );

    // Launch Rx chain calibration for HF band
    SX1276_WriteReg( REG_IMAGECAL, ( SX1276_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1276_ReadReg( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Restore context
    SX1276_WriteReg( REG_PACONFIG, regPaConfigInitVal );
    SX1276_SetChannel( initialFreq );
}

void RF915_Init(unsigned char mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);	 //使能PA,PB端口时钟

  //PA8 9  RX:TXE CSN
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9;  //端口配置
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    		//IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PB11 12  RX:RST RXE
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11|GPIO_Pin_12;  //端口配置
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  SPI2_Init();         //RX for 915MHz
	SX1276_SleepMode();  //接收和发送使能关闭
	SX1276_Reset();      //硬件复位
	SX1276_RxChainCalibration();//接收通道校准
  SX1276_LoRaEntryRx();//LOAR接收配置
  SX1276_SetSwitchRx();//硬件使能接收打开
}

