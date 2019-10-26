#ifndef UART_SERVER
#define UART_SERVER

#include <stdbool.h>

typedef struct uartServerRequest{
    int endpoint;
    enum UARTMethod{
        POST,
        GET,
        OPT,
        GETLN,
        CLEAR,
        FLUSH
    } method;
    int length;
    int opt;
    char* payload;
} uartRequest;

void rxServer();
void txServer();

// void uartServer();

int Getc(int tid, int channel);

int Putc(int tid, int channel, char ch);

int GetCN(int tid, int channel, char* buffer, int length, bool async);

int PutCN(int tid, int channel, char* buffer, int length, bool async);

int GleanUART(int tid, int channel, int offset, char* buffer, int length);

//Make sure your buffer has at least one character
int GetLN(int tid, int channel, char* buffer, int length, char delimiter, bool async);

void clearRXBuffer(int tid, int channel);

void flushTXBuffer(int tid, int channel);

#endif
