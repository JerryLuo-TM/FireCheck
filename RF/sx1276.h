#ifndef __SX1276_H__
#define __SX1276_H__

#include "sys.h"
#include "sx1276-LoRa.h"

#define XTAL_FREQ     32000000
#define FREQ_STEP     61.03515625

//RF171028
//#define RX_CSN                     PAout(9)
//#define RX_RST                     PBout(11)
//#define RX_TXE                     PAout(8)
//#define RX_RXE                     PBout(12)

//#define RX_SCK                     PBout(13)
//#define RX_MOSI                    PBout(15)
//#define RX_MISO                    PBin(14)

//RF171202
#define RX_CSN                     PAout(8)
#define RX_RST                     PBout(11)
#define RX_TXE                     PAout(9)
#define RX_RXE                     PBout(12)

#define RX_SCK                     PBout(13)
#define RX_MOSI                    PBout(15)
#define RX_MISO                    PBin(14)

//底层函数
unsigned char SPI_SX1276_ExchangeByte(unsigned char uchar);
void SX1276_WriteReg( unsigned char addr, unsigned char value );
void SX1276_WriteBuffer( unsigned char addr, unsigned char *buffer, unsigned char size );
void SX1276_WriteFifo( unsigned char *buffer, unsigned char size );
unsigned char SX1276_ReadReg( unsigned char addr );
unsigned char SX1276_ReadFifo( void );
void SX1276_LoRa_SetMode( LORA_CHIP_MODE lora_chip_mode );
void SX1276_SetMode( RANGE_MODE range_mode );
void SX1276_Reset( void );
void SX1276_SetSwitchTx( void );
void SX1276_SetSwitchRx( void );
void SX1276_SetChannel( uint32_t freq );
void SX1276_SleepMode( void );

//应用函数
void RF915_Init(unsigned char mode);
	
void SX1276_Config(void);
void SX1276_ConfigRX(void);
void SX1276_ConfigTX(void);

unsigned char SX1276_LoRaEntryTx(void);
unsigned char SX1276_LoRaEntryRx(void);
unsigned char SX1276_LoRaRxPacket(unsigned char *buffer);
void SX1276_LoRaTxPacket(unsigned char *buffer,unsigned char length);


#endif 
