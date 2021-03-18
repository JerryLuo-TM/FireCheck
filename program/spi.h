#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define SPI_TX_DMA_CHANNEL		DMA1_Channel3
#define SPI_TX_DMA_IRQn				DMA1_Channel3_IRQn

void SPI1_Init(void);               //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet);    //����SPI�ٶ�
u8 SPI1_ReadWriteByte(u8 TxData);   //SPI���߶�дһ���ֽ�

void SPI1_DMA_IT_Config(void);
void SPI_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void SPI_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint16_t len);


void SPI2_Init(void);               //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet);    //����SPI�ٶ�
u8 SPI2_ReadWriteByte(u8 TxData);   //SPI���߶�дһ���ֽ�

#endif
