
#include "sx1276-Fsk.h"
//extern INT8U result;

/*
===============================================================================
*Function:  SX1276_SetFskOok( ) => Put the SX1276 into another type
*Input   :  type,
        TYPE_FSK                     =   0x00,       //Fsk type
    	TYPE_OOK                     =   0x01,       //Ook type
*Output  :  None
===============================================================================
*/
void SX1276_SetFskOok( TYPE type )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_OPMODE );
	tmp &= ~( ( 1<<6 ) | ( 1<<5 ) );
	tmp |= ( type << 5 );
	SX1276_WriteReg( REG_OPMODE, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetMode( ) => Put the SX1276 into another work mode
*Input   :  fsk_chip_mode,
        FSK_MODE_SLEEP               =   0x00,       //sleep mode
	    FSK_MODE_STDBY               =   0x01,       //standby mode
	    FSK_MODE_FSTX                =   0x02,       //frequency synthesis tx mode
	    FSK_MODE_TX                  =   0x03,       //transmit mode
	    FSK_MODE_FSRX                =   0x04,       //frequency synthesis rx mode
	    FSK_MODE_RX                 =   0x05,       //receive continuous mode
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetMode( FSK_CHIP_MODE fsk_chip_mode )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_OPMODE );
	tmp &= ~( ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
	tmp |= ( fsk_chip_mode << 0 );
	SX1276_WriteReg( REG_OPMODE, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetDataRate( ) => Set the Datarate of SX1276
*Input   :  index, 0-3
*Output  :  None
===============================================================================
*/
static const INT8U Fsk_DataRate_Table[4][2] =
{
    { RF_BITRATEMSB_1000_BPS, RF_BITRATELSB_1000_BPS },
    { RF_BITRATEMSB_10000_BPS, RF_BITRATELSB_10000_BPS },
    { RF_BITRATEMSB_20000_BPS, RF_BITRATELSB_20000_BPS },
    { RF_BITRATEMSB_25000_BPS, RF_BITRATELSB_25000_BPS }
};
void SX1276_Fsk_SetDataRate( INT8U index )
{
    index = index <= 3 ? index : 3;

    SX1276_WriteReg( 0x02, Fsk_DataRate_Table[index][0] );
    SX1276_WriteReg( 0x03, Fsk_DataRate_Table[index][1] );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetRxBwMant( ) => Set the RxBwMant of SX1276 in Fsk model
*Input   :  rxbwmant
				  0x00,       //RxBwMant = 16
				  0x01,       //RxBwMant = 20
				  0x02,       //RxBwMant = 24
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetRxBwMant( INT8U rxbwmant )
{
	INT8U tmp;
	if( rxbwmant <= 2 )
	{
		tmp = SX1276_ReadReg( REG_RXBW );
		tmp &= ~( ( 1<<4 ) | ( 1<<3 ) );
		tmp |= ( rxbwmant << 3 );
		SX1276_WriteReg( REG_RXBW, tmp );
	}
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetRxBwExp( ) => Set the RxBwExp of SX1276 in Fsk model
*Input   :  rxbwexp, 1-7
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetRxBwExp( INT8U rxbwexp )
{
	INT8U tmp;
	if( ( rxbwexp >= 1 ) && ( rxbwexp <= 7 ) )
	{
		tmp = SX1276_ReadReg( REG_RXBW );
		tmp &= ~( ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
		tmp |= ( rxbwexp << 0 );
		SX1276_WriteReg( REG_RXBW, tmp );
	}
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetRxBw( ) => Set the RxBw of SX1276 in Fsk model
*Input   :  rxbw, 1-7
*Output  :  None
===============================================================================
*/
static const INT8U RxBw_Table[21][2] =
{
	{ 0, 1 },  //250.0KHz,  0
	{ 1, 1 },  //200.0KHz,  1
	{ 2, 1 },  //166.7KHz,  2
	{ 0, 2 },  //125.0KHz,  3
	{ 1, 2 },  //100.0KHz,  4
	{ 2, 2 },  //83.3KHz,   5
	{ 0, 3 },  //62.5KHz,   6
	{ 1, 3 },  //50.0KHz,	7
	{ 2, 3 },  //41.7KHz,	8
	{ 0, 4 },  //31.3KHz,	9
	{ 1, 4 },  //25.0KHz,	10
	{ 2, 4 },  //20.8KHz,   11
	{ 0, 5 },  //15.6KHz,	12
	{ 1, 5 },  //12.5KHz,	13
	{ 2, 5 },  //10.4KHz,	14
	{ 0, 6 },  //7.8KHz,	15
	{ 1, 6 },  //6.3KHz,	16
	{ 2, 6 },  //5.2KHz,	17
	{ 0, 7 },  //3.9KHz,	18
	{ 1, 7 },  //3.1KHz,	19
	{ 2, 7 }   //2.6KHz,	20
};
void SX1276_Fsk_SetRxBw( INT8U rxbw )
{
	if( rxbw <= 20 )
	{
		SX1276_Fsk_SetRxBwMant( RxBw_Table[rxbw][0] );
		SX1276_Fsk_SetRxBwExp( RxBw_Table[rxbw][1] );
	}
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetAfcEn( ) => Enables/Disables AFC
*Input   :  afc_en
				AFC_ON                       =   0x01,       //Enables Afc
				AFC_OFF                      =   0x00,       //Disables Afc
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetAfcEn( AFC_EN afc_en )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_RXCONFIG );
	tmp &= ~ ( 1<<4 );
	tmp |= ( afc_en << 4 );
	SX1276_WriteReg( REG_RXCONFIG, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetCrcEn( ) => Enables/Disables CRC
*Input   :  fsk_crc_en
				FSK_CRC_ON                       =   0x01,       //Enables Crc
    			FSK_CRC_OFF                      =   0x00,       //Disables Crc
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetCrcEn( FSK_CRC_EN fsk_crc_en )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_PACKETCONFIG1 );
	tmp &= ~ ( 1<<4 );
	tmp |= ( fsk_crc_en << 4 );
	SX1276_WriteReg( REG_PACKETCONFIG1, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetPacketFormat( ) => by the fixed packet or the variable packet
*Input   :  pkt_cormat
        PKT_FORMAT_FIXED        =   0x00,       //fixed packet length
        PKT_FORMAT_VARIABLE     =   0x01,       //variable packet length
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetPacketFormat( PKT_FORMAT pkt_cormat )
{
    INT8U tmp;
    tmp = SX1276_ReadReg( REG_PACKETCONFIG1 );
    tmp &= ~( 1<<7 );
    tmp |= ( pkt_cormat<<7 );
    SX1276_WriteReg( REG_PACKETCONFIG1, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetPayloadLength( ) => Set payload length or the max length in rx
*Input   :  fsk_payload_length, 0x00 - 0xff
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetPayloadLength( INT8U fsk_payload_tength )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_PAYLOADLENGTH );
	tmp &= 0x00;
	tmp |= ( fsk_payload_tength << 0 );
	SX1276_WriteReg( REG_PAYLOADLENGTH, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetPreambleLength( ) => Set the preamble length
*Input   :  fsk_preamble_length,
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetPreambleLength( INT16U fsk_preamble_length )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_PREAMBLEMSB );
	tmp &= 0x00;
	tmp |= ( ( fsk_preamble_length >> 8 ) & 0xFF );
	SX1276_WriteReg( REG_PREAMBLEMSB, tmp );

	tmp = SX1276_ReadReg( REG_PREAMBLELSB );
	tmp &= 0x00;
	tmp |= ( fsk_preamble_length & 0xFF );
	SX1276_WriteReg( REG_PREAMBLELSB, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetSyncEn( ) => Enables/Disables the Sync word generation and detection
*Input   :  SYNC_EN
		        SYNC_ON                      =   0x01,       //Enables the Sync word generation and detection
		    	SYNC_OFF                     =   0x00,       //Disables the Sync word generation and detection
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetSyncEn( SYNC_EN sync_en )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_SYNCCONFIG );
	tmp &= ~( 1<<4 );
	tmp |= ( sync_en << 4 );
	SX1276_WriteReg( REG_SYNCCONFIG, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetSyncSize( ) => Set size of the sync bytes
*Input   :  SYNC_SIZE
		        SYNC_SIZE_1                 =   0x00,       //1 sync byte
				SYNC_SIZE_2                 =   0x01,       //2 sync byte
				SYNC_SIZE_3                 =   0x02,       //3 sync byte
				SYNC_SIZE_4                 =   0x03,       //4 sync byte
				SYNC_SIZE_5                 =   0x04,       //5 sync byte
				SYNC_SIZE_6                 =   0x05,       //6 sync byte
				SYNC_SIZE_7                 =   0x06,       //7 sync byte
				SYNC_SIZE_8                 =   0x07,       //8 sync byte
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetSyncSize( SYNC_SIZE sync_size )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_SYNCCONFIG );
	tmp &= ~( ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
	tmp |= ( sync_size << 0 );
	SX1276_WriteReg( REG_SYNCCONFIG, tmp );
}
/*
===============================================================================
*Function:  SX1276_SetSyncValue( ) => Set the SYNC word of SX1276
*Input   :  sync_value1, the SYNC word 1 - 4
			sync_value2, the SYNC word 5 - 8
*Output  :  None
===============================================================================
*/
void SX1276_SetSyncValue( INT32U sync_value2, INT32U sync_value1 )
{
	SX1276_WriteReg( REG_SYNCVALUE1, sync_value1 >> 0 );
	SX1276_WriteReg( REG_SYNCVALUE2, sync_value1 >> 8 );
	SX1276_WriteReg( REG_SYNCVALUE3, sync_value1 >> 16 );
	SX1276_WriteReg( REG_SYNCVALUE4, sync_value1 >> 24 );

	SX1276_WriteReg( REG_SYNCVALUE5, sync_value2 >> 0 );
	SX1276_WriteReg( REG_SYNCVALUE6, sync_value2 >> 8 );
	SX1276_WriteReg( REG_SYNCVALUE7, sync_value2 >> 16 );
	SX1276_WriteReg( REG_SYNCVALUE8, sync_value2 >> 24 );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetDataMode( ) => Set the data mode of the SX1276 in Fsk model
*Input   :  data_mode,
				DATA_MODE_PACKET            =   0x01,       //packet handling mode
				DATA_MODE_CONTINUOUS        =   0x00,       //continuous mode
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetDataMode( DATA_MODE data_mode )
{
	INT8U tmp;
	tmp = SX1276_ReadReg( REG_PACKETCONFIG2 );
	tmp &= ~( 1<<6 ) ;
	tmp |= ( data_mode << 6 );
	SX1276_WriteReg( REG_PACKETCONFIG2, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_Init( ) => Initializes the SX1276
*Input   :  None
*Output  :  None
===============================================================================
*/
INT8U Fsk_RegsTable1[] =    //0x02 - 0x10
{
	( DEF_BITRATEMSB & 0x00 ) | RF_BITRATEMSB_1000_BPS,
	( DEF_BITRATELSB & 0x00 ) | RF_BITRATELSB_1000_BPS,  //bit rate 1kbps
	( DEF_FDEVMSB & RF_FDEVMSB_FDEV_MASK ) | RF_FDEVMSB_100000_HZ,
	( DEF_FDEVLSB & 0x00 ) | RF_FDEVLSB_100000_HZ,                 //fdev 100KHz
	DEF_FRFMSB,
	DEF_FRFMID,
	DEF_FRFLSB,  //0x6C, 0x80, 0x00 , freq 434MHz
	( DEF_PACONFIG & RF_PACONFIG_PASELECT_MASK ) | RF_PACONFIG_PASELECT_PABOOST,  //choose PA_BOOST
	( DEF_PARAMP & RF_PARAMP_MODULATIONSHAPING_MASK ) | RF_PARAMP_MODULATIONSHAPING_01,  //the PA ramp time of SX1276 40us
	( DEF_OCP & RF_OCP_TRIM_MASK ) | RF_OCP_TRIM_150_MA,  //OCP for PA 150mA
	DEF_LNA,  //LNA gain G1
	( DEF_RXCONFIG & RF_RXCONFIG_AFCAUTO_MASK ) | RF_RXCONFIG_AFCAUTO_ON, //Enables AFC
	DEF_RSSICONFIG,  //8 samples taken to average the RSSI result
	DEF_RSSICOLLISION,
	DEF_RSSITHRESH
};
INT8U Fsk_RegsTable2[] =    //0x12 - 0x1A
{
	( DEF_RXBW & RF_RXBW_MANT_MASK & RF_RXBW_EXP_MASK ) | RF_RXBW_MANT_16 | RF_RXBW_EXP_1,  //BW 250KHz
	DEF_AFCBW,  //
	DEF_OOKPEAK,  //bit sync enabled
	DEF_OOKFIX,
	DEF_OOKAVG,
	DEF_RES17,
	DEF_RES18,
	DEF_RES19,
	DEF_AFCFEI
};
INT8U Fsk_RegsTable3[] =    //0x1F - 0x3B
{
	( DEF_PREAMBLEDETECT & 0x00 ) | RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 | RF_PREAMBLEDETECT_DETECTORTOL_10,
	DEF_RXTIMEOUT1,
	DEF_RXTIMEOUT2,
	DEF_RXTIMEOUT3,
	DEF_RXDELAY,
	DEF_OSC,  //CLKOUT frequency:  FXOSC
	DEF_PREAMBLEMSB,
	DEF_PREAMBLELSB,  //Size of the preamble : 3 bytes
	( DEF_SYNCCONFIG & RF_SYNCCONFIG_SYNCSIZE_MASK ) | RF_SYNCCONFIG_SYNCSIZE_2,  //Sync size: 8 bytes
	DEF_SYNCVALUE1 | 0x69,
	DEF_SYNCVALUE2 | 0x81,
	DEF_SYNCVALUE3 | 0x7E,
	DEF_SYNCVALUE4 | 0x96,
	DEF_SYNCVALUE5 | 0x68,
	DEF_SYNCVALUE6 | 0x80,
	DEF_SYNCVALUE7 | 0x7D,
	DEF_SYNCVALUE8 | 0x95,
	DEF_PACKETCONFIG1,  //Variable packet, enables CRC,
	DEF_PACKETCONFIG2,  // Packet model
	( DEF_PAYLOADLENGTH & 0x00 ) | 0xFF,
	DEF_NODEADRS,
	DEF_BROADCASTADRS,
	( DEF_FIFOTHRESH & RF_FIFOTHRESH_TXSTARTCONDITION_MASK ) | RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY,  //Tx start fifo not empty
	DEF_SEQCONFIG1,
	DEF_SEQCONFIG2,
	DEF_TIMERRESOL,
	DEF_TIMER1COEF,
	DEF_TIMER2COEF,
	DEF_IMAGECAL,
};
void SX1276_Fsk_Init( void )
{
	SX1276_SetMode( MODE_FSK );
	SX1276_Fsk_SetMode( FSK_MODE_STDBY );
	SX1276_WriteBuffer( REG_BITRATEMSB, Fsk_RegsTable1, 0x10 - 0x02 + 1 );  //Regs 0x02 - 0x10
	SX1276_WriteBuffer( REG_RXBW, Fsk_RegsTable2, 0x1A - 0x12 + 1 );  //Regs 0x12 - 0x1A
	SX1276_WriteBuffer( REG_PREAMBLEDETECT, Fsk_RegsTable3, 0x3B - 0x1F + 1 );  //Regs 0x1F - 0x3B
	SX1276_WriteReg( REG_LOWBAT, DEF_LOWBAT ); //Regs 0x3D
	SX1276_WriteReg( REG_DIOMAPPING1, DEF_DIOMAPPING1 );  //Regs 0x40
	SX1276_WriteReg( REG_DIOMAPPING2, DEF_DIOMAPPING2 );  //Regs 0x41
	SX1276_WriteReg( REG_PADAC, DEF_PADAC | RF_PADAC_20DBM_ON );  //Regs 0x4D,  enables 20dbm,
	SX1276_WriteReg( REG_BITRATEFRAC, DEF_BITRATEFRAC );  //Regs 0x5D, BitRateFrac =0,
}
/*
===============================================================================
*Function:  SX1276_Fsk_SendPacket_Var( ) => Send packet via the SX1276
*Input   :  buffer, the buffer stores data
            size, how many bytes should be written
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SendPacket_Var( INT8U *buffer, INT8U size )
{
	if( size )
	{
		SX1276_Fsk_SetDataMode( DATA_MODE_PACKET );  //packet model
		SX1276_Fsk_SetPacketFormat( PKT_FORMAT_VARIABLE );  //variable packet length
		SX1276_WriteReg( REG_DIOMAPPING1, DEF_DIOMAPPING1 | RF_DIOMAPPING1_DIO0_00  //PacketSent
	                          							  | RF_DIOMAPPING1_DIO1_00  //FifoLevel
	                          							  | RF_DIOMAPPING1_DIO2_00  //FifoFull
	                          							  | RF_DIOMAPPING1_DIO3_01 );  //TxReady
		SX1276_WriteReg( REG_DIOMAPPING2, DEF_DIOMAPPING2 | RF_DIOMAPPING2_DIO4_00  //LowBat
		                                                  | RF_DIOMAPPING2_DIO5_10 );  //Data

		SX1276_SetSwitchTx( );
		SX1276_Fsk_SetMode( FSK_MODE_TX );  //Tx model

		//while(!SX1276_READ_DIO3( ) );  //Waits until tx ready
		SX1276_WriteFifo( ( INT8U* )&size, 1 );  //length byte
		SX1276_WriteFifo( buffer, size );
		//while(!SX1276_READ_DIO0( ) );  //Waits until tx done
	}
}
/*
===============================================================================
*Function:  SX1276_Fsk_StartRx( ) => Sets the radio in Rx mode
*Input   :  None
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_StartRx( void )
{
	SX1276_WriteReg( REG_DIOMAPPING1, DEF_DIOMAPPING1 | RF_DIOMAPPING1_DIO0_01  //CrcOk
                          							  | RF_DIOMAPPING1_DIO1_00  //FifoLevel
                          							  | RF_DIOMAPPING1_DIO2_11  //SyncAddr
                          							  | RF_DIOMAPPING1_DIO3_00 );  //FifoEmpty
	SX1276_WriteReg( REG_DIOMAPPING2, DEF_DIOMAPPING2 | RF_DIOMAPPING2_DIO4_11  //Preamble
	                                                  | RF_DIOMAPPING2_DIO5_10 );  //Data
	SX1276_SetSwitchRx( );
	SX1276_Fsk_SetMode( FSK_MODE_RX ); //Rx model
}
/*
===============================================================================
*Function:  SX1276_Fsk_RxProcess( ) => Process the Fsk modem Rx state machines depending on the SX1276 operating mode
*Input   :  None
*Output  :  value,
===============================================================================
*/
INT8U SX1276_Fsk_RxProcess( void )
{
	return 1;
//    if( SX1276_READ_DIO0( ) )   { return 1; }
//    else                        { return 0; }
// if( SX1276_READ_DIO0( ) )   { result = 1; }
// else                        { result = 0; }
// return result;
}
/*
===============================================================================
*Function:  SX1276_Fsk_GetRxPacket( ) => Gets a copy of the current received buffer
*Input   :  buffer, the buffer stores data
*Output  :  size, hao many bytes received
===============================================================================
*/
INT8U SX1276_Fsk_GetRxPacket( INT8U *buffer )
{
	INT8U size, i;
	size = SX1276_ReadFifo( );
	for( i = 0; i < size; i ++ )
    {
        *buffer ++ = SX1276_ReadFifo( );
    }
    if( size )   { SX1276_Fsk_FlushFifo( ); }
    return size;
}
/*
===============================================================================
*Function:  SX1276_Fsk_FlushFifo( ) =>Flush the FIFO
*Input   :  None
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_FlushFifo( void )
{
    INT8U tmp;
    tmp =  SX1276_ReadReg( 0x3F );
    tmp |= ( 1<<4 );
    SX1276_WriteReg( 0x3F, tmp );
}
/*
===============================================================================
*Function:  SX1276_Fsk_SetPreambleSize( ) =>Set the preamble count
*Input   :  cnt, count of the preamble size
*Output  :  None
===============================================================================
*/
void SX1276_Fsk_SetPreambleSize( INT16U cnt )
{
    SX1276_WriteReg( REG_PREAMBLEMSB, cnt >> 8 );
    SX1276_WriteReg( REG_PREAMBLELSB, cnt );
}






