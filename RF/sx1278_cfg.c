#include "struct.h"

unsigned char   SX1278_Lora_Rate_Sel = 0x02;  //��Ƶ����Ϊ7
unsigned char   SX1278_BandWide_Sel  = 0x08;  //500KHZ

//��Ƶ����Խ�󣬴���ʱ��Խ��

/* ��Ƶ���� */
const unsigned char SX1278_SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

/* ���� */
const unsigned char SX1278_LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

//�ر�CRC reg:0x30 bit4 [0����  1����]
//        reg:0x1E bit2 [0����  1����]
void SX1278_Config(void)
{
	  SX1278_LoRa_SetMode(LORA_MODE_SLEEP);//reg:0x01 ����˯��ģʽ
	  SX1278_SetChannel(SX1278_frequency); //�����ز�Ƶ��
		//���û������� 
	  SX1278_SetMode( MODE_LORA );		        //����LOAR ģʽ ����LOAR�Ĵ���
		SX1278_WriteReg(REG_LR_PACONFIG,0xFF);  //0x09 TX �����������   20dBm
	  SX1278_WriteReg(REG_LR_PARAMP,0x09);    //0x0A TX LORAģʽ�� б��/б��
		SX1278_WriteReg(REG_LR_OCP,0x0B);       //0x0B TX �����������ƣ��ر�PA��������
		//SX1278_WriteReg(REG_LR_LNA,0x38);     //0x0C RX LAN��������Ϊ��� ����������
    SX1278_WriteReg(REG_LR_PAYLOADMAXLENGTH,0xFF); //0x23 ���س������ֵ  �������ᱻ�˳�
	  //���ý��ջ�  FSK  ģʽ
	  SX1278_SetMode( MODE_FSK );              //0x01 ����FSK����
	  SX1278_WriteReg(REG_RXCONFIG,0x1E);      //0x0D
	  SX1278_WriteReg(REG_RSSICONFIG,0xD2);    //0x0E
	  SX1278_WriteReg(REG_AFCFEI,0x01);        //0x1A
	  SX1278_WriteReg(REG_PREAMBLEDETECT,0xAA);//0x1f
	  SX1278_WriteReg(REG_OSC,0x07);           //0x24 RC���� CLOCKOUT�ر�
	  SX1278_WriteReg(REG_SYNCCONFIG,0x12);    //0x27 ����ͬ���� 2+1 �ֽ�
	  SX1278_WriteReg(REG_SYNCVALUE1,0xC1);    //0x28
	  SX1278_WriteReg(REG_SYNCVALUE2,0x94);    //0x29
	  SX1278_WriteReg(REG_SYNCVALUE3,0xC1);    //0x2A
		SX1278_WriteReg(REG_PACKETCONFIG1,0xD8); //0x30
		SX1278_WriteReg(REG_FIFOTHRESH,0x8F);    //0x35
		
		 // ����Ĵ���
		SX1278_WriteReg(REG_IMAGECAL,0x02);
	
	  SX1278_SetMode( MODE_LORA );				    //����LOAR ģʽ ����LOAR�Ĵ���
		//����������������Ƶ���Ӻʹ���  �������� ��Ƶ����=6
	  
	  //0x1D �źŴ��� + ��������� + ��ʾ��ͷ     4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1278_WriteReg(REG_LR_MODEMCONFIG1,(SX1278_LoRaBwTbl[SX1278_BandWide_Sel]<<4) | (0x01<<1) | 0x00 );
	  //0x1E ��Ƶ���� + ����ģʽ + ���Ͷ�CRC + RX��ʱ
		SX1278_WriteReg(REG_LR_MODEMCONFIG2,(SX1278_SpreadFactorTbl[SX1278_Lora_Rate_Sel]<<4) | (1<<2) | 0x03 ); 

		SX1278_WriteReg(REG_LR_SYMBTIMEOUTLSB,0xFF);//0x1F ���ջ���ʱ = 0x3FF(Max) 
		SX1278_WriteReg(REG_LR_PREAMBLEMSB,0x00);   //0x20 ǰ���� ���ֽ�
		SX1278_WriteReg(REG_LR_PREAMBLELSB,12);     //0x21 ǰ���� ���ֽ� ǰ����=8+4=12�ֽ� 
		
		SX1278_WriteReg(REG_LR_DIOMAPPING1,0x00);   //0x40 ����ӳ��
		SX1278_WriteReg(REG_LR_DIOMAPPING2,0x01);   //0x41 ����ӳ��   
		
    SX1278_LoRa_SetMode( LORA_MODE_STDBY );//�������ģʽ
}

/**********************************************************
**����:     SX1278_7_8_LoRaEntryTx
**����:     Entry Tx mode
**����:     None
**���:     0-��ʱ����
**********************************************************/
unsigned char SX1278_LoRaEntryTx(void)
{
  static unsigned char Vesion,addr,tmp;

  SX1278_Config();               //���û�������
  SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //�������ģʽ
	
	//����LORA�Ĵ���  
	SX1276_SetMode( MODE_LORA );				          //����LOAR ģʽ ����LOAR�Ĵ���
	SX1278_WriteReg(REG_LR_HOPPERIOD,0x00);       //0x24 ����Ƶ����  �ر�
  SX1278_WriteReg(REG_LR_PADAC,0x87);           //0x4D �߹������� 
 
  SX1278_WriteReg(REG_LR_IRQFLAGSMASK,0xF7);    //0x11 �ж����μĴ���  ���� �� TXDone �� �ж�
	SX1278_WriteReg(REG_LR_IRQFLAGS,0xFF);        //0x12 ����жϱ�ʶ
	
  SX1278_WriteReg(REG_LR_PAYLOADLENGTH,1);      //0x22 ���س��� ����д0 (����Ƶ����Ϊ6ʱ���ݴ���һ�ֽڴ˼Ĵ�����������) 
  
  addr = SX1278_ReadReg( REG_LR_FIFOTXBASEADDR );//FiFo���ݻ������з��͵�������д�����ַ
  SX1278_WriteReg(REG_LR_FIFOADDRPTR,addr);      //Fifo���ݻ�������SPI�ӿڵ�ַָ��

	tmp = SX1278_ReadReg(REG_LR_OPMODE);         //0x01 ��ȡͨ�üĴ���
	tmp |= 0x08;                                 //�����Ƶ����ģʽλ
	SX1278_WriteReg(REG_LR_OPMODE,tmp);          //0x01 433ģ�� ���ø�Ƶģʽ
	
	SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //�������ģʽ
	vTaskDelay(configTICK_RATE_HZ/100);     //�ȴ����

	Vesion=SX1278_ReadReg(REG_LR_VERSION); //0x42 ��ȡ�汾
	if(Vesion==0x12)
			return 1;
    else
			return 0;
}

/**********************************************************
**����:     SX1278_7_8_LoRaEntryRx
**����:     ����LORa RXģʽ
**����:     None
**���:     0-��ʱ����
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
	SX1278_WriteReg( REG_LR_PAYLOADLENGTH, length ); //д����
	SX1278_WriteReg( REG_LR_FIFOTXBASEADDR, 0x00 ); // Full buffer used for Tx
  SX1278_WriteReg( REG_LR_FIFOADDRPTR, 0x00 );
	// FIFO operations can not take place in Sleep mode
	if( ( SX1278_ReadReg( REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
	{
			SX1278_LoRa_SetMode( LORA_MODE_STDBY ); //�������ģʽ
	}
	SX1278_WriteFifo( buffer, length );
	SX1278_WriteReg(REG_LR_OPMODE,0x8b);  //Tx Mode  433��Ƶģʽ
  while(!(SX1278_ReadReg( REG_LR_IRQFLAGS )&0x08)); //TxDone
	SX1278_WriteReg( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );//����ж�
	SX1278_LoRa_SetMode( LORA_MODE_STDBY );
}

/*!
 * Performs the Rx chain calibration for LF and HF bands
 * \remark Must be called just after the reset so all registers are at their
 *         default values
 *  �ٷ������е�У׼����
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	   //ʹ��PA,PB�˿�ʱ��
  
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0
																 |GPIO_Pin_1
																 |GPIO_Pin_2
																 |GPIO_Pin_10;            //�˿�����
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI1_Init();         //TX for 433Mhz
	SX1278_SleepMode();  //���պͷ���ʹ�ܹر�
	SX1278_Reset();      //Ӳ����λ
	SX1278_RxChainCalibration();//����ͨ��У׼
	temp=SX1278_LoRaEntryTx();  //LOAR��������
	SX1278_SetSwitchTx();       //Ӳ��ʹ�ܷ���
	if(temp==0){R_LED=0;}       //��ʼ��ʧ�ܵ������
}

