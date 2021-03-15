#include "ws2812.h"

/*WS2812B Timing sequence
	    ________
	   |		|	   T0L	     |
0 code |<------>|<-------------->|
	   |  T0H   |________________|


	    ___________
	   |		   |	   T1L	  |
1 code |<--------->|<--------- -->|
	   |  T1H      |______________|


RET code
	   |	     Treset		 	   |
	   |<------------------------->|
	   |___________________________|

Data transefer time:
T0H 		0 code ,high voltage time 		0.4us ¡À150ns
T1H 		1 code ,high voltage time 		0.8us ¡À150ns
T0L 		0 code , low voltage time 		0.85us ¡À150ns
T1L 		1 code ,low voltage time 		0.45us ¡À150ns
RES 		low voltage time 				Above 50us

*/


int8_t WS2812_Init(void)
{
	return 0;
}

static int8_t WS2812_BitCode_Packet(uint8_t bit_val,uint8_t *spi_packet_buf)
{
	if (bit_val == 0) {
		spi_packet_buf[0] = 0xFF;
		spi_packet_buf[1] = 0x00;
		spi_packet_buf[2] = 0x00;
	} else if(bit_val == 1) {
		spi_packet_buf[0] = 0xFF;
		spi_packet_buf[1] = 0xFF;
		spi_packet_buf[2] = 0x00;
	} else {
		return -1;
	}

	return 0;
}

static int8_t WS2812_OnePixel_Packet(T_RGB_CTRL sRGBVal,uint8_t *packet_buf)
{
	uint8_t bit_cnt;
	uint8_t color_cnt;
	uint8_t bit_pos;
	uint8_t singlecolor_val;

	for (color_cnt = 0; color_cnt < 3; color_cnt++) {
		if (color_cnt == 0)
			singlecolor_val = sRGBVal.G_Val;
		else if(color_cnt == 1)
			singlecolor_val = sRGBVal.R_Val;
		else if(color_cnt == 2)
			singlecolor_val = sRGBVal.B_Val;

		for (bit_cnt = 0; bit_cnt < 8; bit_cnt++) {
			bit_pos = (7 - bit_cnt);
			if(singlecolor_val & (1 << bit_pos)) {
				WS2812_BitCode_Packet(1,packet_buf);
			} else {
				WS2812_BitCode_Packet(0,packet_buf);
			}
			packet_buf += SPI_BYTE_LEN_PER_PIXEL;
		}
	}
	return 0;
}

uint32_t WS2812_Pixels_Packet(uint32_t pixel_num,uint32_t *rgb_buf,uint8_t* spi_packet_buf)
{
	uint32_t pixel_index;
	T_RGB_CTRL rgb_ctrl_val;
	uint32_t spi_packet_buf_index = 0;

	for (pixel_index = 0; pixel_index < pixel_num; pixel_index++) {
		rgb_ctrl_val.R_Val = (uint8_t)(rgb_buf[pixel_index] >> 16);
		rgb_ctrl_val.G_Val = (uint8_t)(rgb_buf[pixel_index] >> 8);
		rgb_ctrl_val.B_Val = (uint8_t)(rgb_buf[pixel_index] >> 0);
		WS2812_OnePixel_Packet(rgb_ctrl_val,spi_packet_buf + spi_packet_buf_index);
		spi_packet_buf_index += SPI_BYTE_LEN_PER_PIXEL * 24;
	}
	return spi_packet_buf_index;
}

