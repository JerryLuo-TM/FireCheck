#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define SPI_TX_DMA_CHANNEL      DMA1_Channel3
#define SPI_TX_DMA_IRQn         DMA1_Channel3_IRQn

void SPI1_Init(void);
void SPI1_SetSpeed(uint8_t SpeedSet);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);

void SPI1_DMA_IT_Config(void);
void SPI_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr);
void SPI_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint16_t len);


void SPI2_Init(void);
void SPI2_SetSpeed(uint8_t SpeedSet);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);

#endif
