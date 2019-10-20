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

int GleanCN(int tid, int channel, int offset, char* buffer, int length);

//Make sure your buffer has at least one character
int GetLN(int tid, int channel, char* buffer, int length, char delimiter, bool async);

#endif
