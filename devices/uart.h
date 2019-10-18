#ifndef UART_H
#define UART_H

#include <stdbool.h>

#define UART1RX_DEV_ID 23
#define UART1TX_DEV_ID 24
#define UART2RX_DEV_ID 25
#define UART2TX_DEV_ID 26
#define INT_UART1 52
#define INT_UART2 54

#define UARTCLK 7372800

void initializeUART(int channel, int bitRate, bool fifo, bool RTIE, bool TIE, bool RIE, bool MSIE);

int put(int channel, char Byte);

int get(int channel, char* Byte);

#endif
