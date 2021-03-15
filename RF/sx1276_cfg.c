#include "struct.h"

unsigned char   SX1276_Lora_Rate_Sel = 0x01;  //��Ƶ����Ϊ7
unsigned char   SX1276_BandWide_Sel  = 0x09;  //500KHZ

//��Ƶ����Խ�󣬴���ʱ��Խ��

/* ��Ƶ���� */
const unsigned char SX1276_SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

/* ���� */
const unsigned char SX1276_LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

void SX1276_Config(void)
{
	  SX1276_LoRa_SetMode( LORA_MODE_SLEEP );//�ı䵱ǰģʽ�������˯��ģʽ
	  SX1276_SetChannel(SX1276_frequency);   //�����ز�Ƶ��
		//���û������� 
		SX1276_SetMode( MODE_LORA );				 //����LOAR ģʽ ����LOAR�Ĵ���
		SX1276_WriteReg(REG_LR_PACONFIG,0xFF);//0x09 TX �����������   20dBm
	  SX1276_WriteReg(REG_LR_PARAMP,0x09);  //0x0A TX LORAģʽ�� б��/б��
		SX1276_WriteReg(REG_LR_OCP,0x0B);     //0x0B TX �����������ƣ��ر�PA��������
		SX1276_WriteReg(REG_LR_LNA,0x23);     //0x0C RX LAN��������Ϊ��� ����������
		SX1276_WriteReg(REG_LR_PAYLOADMAXLENGTH,0xFF); //0x23 ���س������ֵ  �������ᱻ�˳�
		  //���ý��ջ�  FSK  ģʽ
	  SX1276_SetMode( MODE_FSK );              //0x01 ����FSK����
	  SX1276_WriteReg(REG_RXCONFIG,0x1E);      //0x0D
	  SX1276_WriteReg(REG_RSSICONFIG,0xD2);    //0x0E
	  SX1276_WriteReg(REG_AFCFEI,0x01);        //0x1A
	  SX1276_WriteReg(REG_PREAMBLEDETECT,0xAA);//0x1f
	  SX1276_WriteReg(REG_OSC,0x07);           //0x24 RC���� CLOCKOUT�ر�
	  SX1276_WriteReg(REG_SYNCCONFIG,0x12);    //0x27 ����ͬ���� 2+1 �ֽ�
	  SX1276_WriteReg(REG_SYNCVALUE1,0xC1);    //0x28
	  SX1276_WriteReg(REG_SYNCVALUE2,0x94);    //0x29
	  SX1276_WriteReg(REG_SYNCVALUE3,0xC1);    //0x2A
		SX1276_WriteReg(REG_PACKETCONFIG1,0xD8|(2<<1)); //0x30 ������  �Ƿ����ڵ��ַɸѡ
		
//		SX1276_WriteReg(REG_NODEADRS,0xAA);      //0x33    �ڵ��ַ
//		SX1276_WriteReg(REG_BROADCASTADRS,0xAA); //0x34    �㲥��ַ
		
		SX1276_WriteReg(REG_FIFOTHRESH,0x8F);    //0x35
		 // ����Ĵ���
		SX1276_WriteReg(REG_IMAGECAL,0x02);
	
	  SX1276_SetMode( MODE_LORA );				     //����LOAR ģʽ ����LOAR�Ĵ���
		//����������������Ƶ���Ӻʹ���  �������� ��Ƶ����=6
	  
	  //0x1D �źŴ��� + ��������� + ��ʾ��ͷ     4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SX1276_WriteReg(REG_LR_MODEMCONFIG1,(SX1276_LoRaBwTbl[SX1276_BandWide_Sel]<<4) | (0x01<<1) | 0x00 );
	  //0x1E ��Ƶ���� + ����ģʽ + ���Ͷ�CRC + RX��ʱ
		SX1276_WriteReg(REG_LR_MODEMCONFIG2,(SX1276_SpreadFactorTbl[SX1276_Lora_Rate_Sel]<<4) | (1<<2) | 0x03 ); 

		SX1276_WriteReg(REG_LR_SYMBTIMEOUTLSB,0xFF);//0x1F ���ջ���ʱ = 0x3FF(Max) 
		SX1276_WriteReg(REG_LR_PREAMBLEMSB,0x00);   //0x20 ǰ���� ���ֽ�
		SX1276_WriteReg(REG_LR_PREAMBLELSB,12);     //0x21 ǰ���� ���ֽ� ǰ����=8+4=12�ֽ� 
		
		SX1276_WriteReg(REG_LR_DIOMAPPING1,0x00);   //0x40 ����ӳ��
																								//bit[7:6]DIO0 
																								//bit[5:4]DIO1
																								//bit[3:2]DIO2
																								//bit[1:0]DIO3
		SX1276_WriteReg(REG_LR_DIOMAPPING2,0x01);   //0x41 ����ӳ��   
																								//bit[7:6]DIO2
																								//bit[5:4]DIO2
																								//bit[0]  1��RSSI�ж�  0��PreambleDetect �ж�
		
    SX1276_LoRa_SetMode( LORA_MODE_STDBY );//�������ģʽ
}

/**********************************************************
**����:     SX1276_7_8_LoRaEntryTx
**����:     Entry Tx mode
**����:     None
**���:     0-��ʱ����
**********************************************************/
unsigned char SX1276_LoRaEntryTx(void)
{
	return 0;
}

/**********************************************************
**����:     SX1276_7_8_LoRaEntryRx
**����:     ����LORa RXģʽ
**����:     None
**���:     0-��ʱ����
**********************************************************/
unsigned char SX1276_LoRaEntryRx(void)
{
  static unsigned char addr,temp;

  SX1276_Config();               //������������

  SX1276_WriteReg(REG_LR_PADAC,0x84);         //������Rx
  SX1276_WriteReg(REG_LR_HOPPERIOD,0xFF);     //����Ƶ
	
	//DIO0 ����Ϊ��������ź� 
	//DIO1 ���ճ�ʱ
	//DIO2 FhssChange Channel
	//DIO3 CADDone
  SX1276_WriteReg(REG_LR_DIOMAPPING1,0x01);   //DIO0=00, DIO1=00, DIO2=00, DIO3=01

  SX1276_WriteReg(REG_LR_IRQFLAGSMASK,0x3F);  //��RxDone�ж�&��ʱ0011 1111
	SX1276_WriteReg(REG_LR_IRQFLAGS,0xFF);      //����жϱ�ʶ
	
  SX1276_WriteReg(REG_LR_PAYLOADLENGTH,1);  //21�ֽ�(����Ƶ����Ϊ6ʱ���ݴ���һ�ֽڴ˼Ĵ�����������) 
	
	addr = SX1276_ReadReg(REG_LR_FIFORXBASEADDR);//Read RxBaseAddr
  SX1276_WriteReg(REG_LR_FIFOADDRPTR,addr);    //RxBaseAddr -> FiFoAddrPtr��
  
	//SX1276_WriteReg(REG_LR_OPMODE,0x8d);         //����Rxģʽ//��Ƶģʽ10001101
	SX1276_WriteReg(REG_LR_OPMODE,0x85);           //����Rxģʽ//��Ƶģʽ00000101

	vTaskDelay(configTICK_RATE_HZ/100);    //�ȴ����

	temp=SX1276_ReadReg(REG_LR_MODEMSTAT)&0x04;//Rx���м��

	if(temp==0x04)//Rx-on going RegModemStat//RX������
		return 1;
	else
		return 0;
}

unsigned char SX1276_LoRaRxPacket(unsigned char *buffer)
{
  unsigned char i; 
  unsigned char packet_size=0;
	unsigned char IRQ_val;
	//��ȡ�жϼĴ���
	IRQ_val=SX1276_ReadReg(REG_LR_IRQFLAGS);
  if((IRQ_val&0x40)!=0)  //�����ж�
  {
		if((IRQ_val&0x20)==0) //CRC ��ȷ
		{
			//���յ����һ�����ݰ�����ʼ��ַ�����ݻ������У�
			//RxBaseAddr -> FiFoAddrPtr    
			SX1276_WriteReg(REG_LR_FIFOADDRPTR,SX1276_ReadReg(REG_LR_FIFORXCURRENTADDR));

			packet_size = SX1276_ReadReg(REG_LR_NBRXBYTES);      //���յ��ֽ���
			for( i=0; i<packet_size; i++ )
			{
				*buffer ++ = SX1276_ReadFifo( );
			}

			SX1276_WriteReg(REG_LR_IRQFLAGS,0xFF);      //����жϱ�ʶ
			
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
	SX1276_WriteReg( REG_LR_PAYLOADLENGTH, length ); //д����
	SX1276_WriteReg( REG_LR_FIFOTXBASEADDR, 0x00 ); // Full buffer used for Tx
  SX1276_WriteReg( REG_LR_FIFOADDRPTR, 0x00 );
	SX1276_WriteFifo( buffer, length );
	SX1276_WriteReg(REG_LR_OPMODE,0x8b);  //Tx Mode
  while(!(SX1276_ReadReg( REG_LR_IRQFLAGS )&0x08) ); //TxDone //TxDone
	SX1276_WriteReg( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );//����ж�
	SX1276_LoRa_SetMode( LORA_MODE_STDBY );
}

/*!
 * Performs the Rx chain calibration for LF and HF bands
 * \remark Must be called just after the reset so all registers are at their
 *         default values
 *  �ٷ������е�У׼����
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);	 //ʹ��PA,PB�˿�ʱ��

  //PA8 9  RX:TXE CSN
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9;  //�˿�����
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PB11 12  RX:RST RXE
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11|GPIO_Pin_12;  //�˿�����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  SPI2_Init();         //RX for 915MHz
	SX1276_SleepMode();  //���պͷ���ʹ�ܹر�
	SX1276_Reset();      //Ӳ����λ
	SX1276_RxChainCalibration();//����ͨ��У׼
  SX1276_LoRaEntryRx();//LOAR��������
  SX1276_SetSwitchRx();//Ӳ��ʹ�ܽ��մ�
}

