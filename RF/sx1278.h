#ifndef __SX1278_H__
#define __SX1278_H__

#include "sys.h"
#include "sx1276-LoRa.h"

#define TX_CSN                     PBout(0)
#define TX_RST                     PBout(10)
#define TX_TXE                     PBout(1)
#define TX_RXE                     PBout(2)

#define TX_SCK                     PAout(5)
#define TX_MOSI                    PAout(7)
#define TX_MISO                    PAin(6)

//底层函数
unsigned char SPI_SX1278_ExchangeByte(unsigned char uchar);
void SX1278_WriteReg( unsigned char addr, unsigned char value );
void SX1278_WriteBuffer( unsigned char addr, unsigned char *buffer, unsigned char size );
void SX1278_WriteFifo( unsigned char *buffer, unsigned char size );
unsigned char SX1278_ReadReg( unsigned char addr );
unsigned char SX1278_ReadFifo( void );
void SX1278_LoRa_SetMode( LORA_CHIP_MODE lora_chip_mode );
void SX1278_SetMode( RANGE_MODE range_mode );
void SX1278_Reset( void );
void SX1278_SetSwitchTx( void );
void SX1278_SetSwitchRx( void );
void SX1278_SetChannel( uint32_t freq );
void SX1278_SleepMode( void );

//应用函数
void RF433_Init(unsigned char mode);

void SX1278_Config(void);
void SX1278_ConfigRX(void);
void SX1278_ConfigTX(void);

unsigned char SX1278_LoRaEntryTx(void);
unsigned char SX1278_LoRaEntryRx(void);
unsigned char SX1278_LoRaRxPacket(unsigned char *buffer);
void SX1278_LoRaTxPacket(unsigned char *buffer,unsigned char length);

#endif
