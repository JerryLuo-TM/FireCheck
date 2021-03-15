#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

void UART1_Init(u32 bound);
void uart1_sendbyte(unsigned char dat);
void uart1_send_string(unsigned char *buf,unsigned short length);

void UART2_Init(u32 bound);
void uart2_sendbyte(unsigned char dat);
void uart2_send_string(unsigned char *buf,unsigned short length);

void debug_printf( const char * format, ... );
void debug_printf2( const char * format, ... );

#endif


