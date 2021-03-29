#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"
#include "stdbool.h"

extern uint8_t g_uart1_dma_tx_buffer[256 + 64];
extern uint8_t g_uart1_dma_rx_buffer[64];

extern volatile bool g_uart1_tx_is_transfer;

void UART1_Init(uint32_t bound);
void uart1_sendbyte(uint8_t dat);
void uart1_send_string(uint8_t *buf,unsigned short length);
void debug_printf( const char * format, ... );

void UART1_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx);
void UART1_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx);

void UART1_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len);

#endif


