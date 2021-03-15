#ifndef _WS2812_H_
#define _WS2812_H_

#include "stm32f10x.h"
#include "sys.h"

#define SPI_BYTE_LEN_PER_PIXEL			3

#define WS2812_IN_PIN	PAout(7)

typedef struct{
	uint8_t R_Val;
	uint8_t G_Val;
	uint8_t B_Val;
}T_RGB_CTRL;

int8_t WS2812_Init(void);
int8_t WS2812_Send24Bits(T_RGB_CTRL *sRGBVal);
uint32_t WS2812_Pixels_Packet(uint32_t pixel_num,uint32_t *rgb_buf,uint8_t* spi_packet_buf);

#endif

