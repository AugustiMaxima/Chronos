#ifndef UART_SERVER
#define UART_SERVER

#include <stdbool.h>

typedef struct uartServerRequest{
    int endpoint;
    enum UARTMethod{
        POST,
        GET,
        OPT,
        NOTIFY
    } method;
    int length;
    int opt;
    char* payload;
} uartRequest;

void uartServer();

int Getc(int tid, int channel);

int Putc(int tid, int channel, char ch);

int GetCN(int tid, int channel, char* buffer, int length, bool async);

int PutCN(int tid, int channel, char* buffer, int length, bool async);

#endif
