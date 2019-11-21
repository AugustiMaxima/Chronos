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

#define UART_Modem_Stat 0x0104

void initializeUART(int channel, int bitRate, bool fifo, bool RTIE, bool TIE, bool RIE, bool MSIE);

int putUart(int channel, char Byte);

int getUart(int channel, char* Byte);

void setTransmitInterrupt(int channel, bool enable);

void setReceiveInterrupt(int channel, bool enable);

void setReceiveTimeout(int channel, bool enable);

unsigned processUartInterrupt(int channel);

unsigned getUartFlag(int channel);

#endif
