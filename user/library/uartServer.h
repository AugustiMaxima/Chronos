#ifndef UART_SERVER
#define UART_SERVER

typedef struct uartServerRequest{
    int endpoint;
    enum UARTMethod{
        POST,
        GET,
        NOTIFY
    } method;
    int length;
    char* payload;
} uartRequest;

void uartServer();

int Getc(int tid, int channel);

int Putc(int tid, int channel, char ch);

int GetCN(int tid, int channel, char* buffer, int length);

int PutCN(int tid, int channel, char* buffer, int length);

#endif