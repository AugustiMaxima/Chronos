#ifndef UART_H
#define UART_H

#include <stdbool.h>

#define UARTCLK 7372800

void initializeUART(int channel, int bitRate, bool fifo, bool RTIE, bool TIE, bool RIE, bool MSIE);

int put(int channel, char Byte);

int get(int channel, char* Byte);

#endif
