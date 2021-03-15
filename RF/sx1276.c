#include "struct.h"

unsigned char SPI_SX1276_ExchangeByte(unsigned char uchar)
{
	return SPI2_ReadWriteByte(uchar);
}

void SX1276_WriteReg( unsigned char addr, unsigned char value )
{
    RX_CSN=0;
    SPI_SX1276_ExchangeByte( addr | 0x80 );
    SPI_SX1276_ExchangeByte( value );
    RX_CSN=1;
}

void SX1276_WriteBuffer( unsigned char addr, unsigned char *buffer, unsigned char size )
{
	unsigned char i;
	RX_CSN=0;
	SPI_SX1276_ExchangeByte( addr | 0x80 );
	for( i=0; i<size; i++ )
	{
		SPI_SX1276_ExchangeByte( buffer[i] );
	}
	RX_CSN=1;
}

void SX1276_WriteFifo( unsigned char *buffer, unsigned char size )
{
	unsigned char i;
	RX_CSN=0;
  SPI_SX1276_ExchangeByte( 0x00 | 0x80 );
	for( i=0; i<size; i++ )
	{
		SPI_SX1276_ExchangeByte( buffer[i] );
	}
	RX_CSN=1;
}

unsigned char SX1276_ReadReg( unsigned char addr )
{
    unsigned char btmp;
    RX_CSN=0;
    SPI_SX1276_ExchangeByte( addr & 0x7F );
	  btmp = SPI_SX1276_ExchangeByte( 0 );
    RX_CSN=1;
    return btmp;
}

unsigned char SX1276_ReadFifo( void )
{
	  unsigned char btmp;
    RX_CSN=0;
    SPI_SX1276_ExchangeByte( 0x00 & 0x7F );
    btmp = SPI_SX1276_ExchangeByte( 0 );
    RX_CSN=1;
    return btmp;
}

void SX1276_Reset( void )
{
	RX_RST=0;
	delay_ms(3);
	RX_RST=1;
	delay_ms(10);
}

void SX1276_LoRa_SetMode( LORA_CHIP_MODE lora_chip_mode )
{
	unsigned char tmp;
	tmp = SX1276_ReadReg( REG_LR_OPMODE );//读取原有状态
	tmp &= ~0x07; //保留高字节
	tmp |= lora_chip_mode;
	SX1276_WriteReg( REG_LR_OPMODE, tmp );
}

void SX1276_SetMode( RANGE_MODE range_mode )
{
	unsigned char tmp;
	SX1276_LoRa_SetMode( LORA_MODE_SLEEP );
	tmp = SX1276_ReadReg( 0x01 );
  tmp &= ~( 1<<7 );
	tmp |= ( range_mode << 7 );
	SX1276_WriteReg( 0x01, tmp );
}

void SX1276_SleepMode( void )
{
  unsigned char tmp;
	tmp = SX1276_ReadReg( REG_OPMODE );
	tmp &= ~( ( 1<<2 ) | ( 1<<1 ) | ( 1<<0 ) );
	SX1276_WriteReg( REG_OPMODE, tmp );
	RX_TXE=0;
	RX_RXE=0;
}

void SX1276_SetSwitchTx( void )
{
	RX_TXE=1;
	RX_RXE=0;
	delay_ms(5);
}

void SX1276_SetSwitchRx( void )
{
	RX_TXE=0;
	RX_RXE=1;
	delay_ms(5);
}

void SX1276_SetChannel( uint32_t freq )
{
    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );//61.03515625
	  SX1276_WriteReg( REG_FRFMSB,  ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
    SX1276_WriteReg( REG_FRFMID,  ( uint8_t )( ( freq >> 8  ) & 0xFF ) );
    SX1276_WriteReg( REG_FRFLSB,  (              freq & 0xFF         ) );
}

