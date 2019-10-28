#ifndef UART_SERVER
#define UART_SERVER

#include <stdbool.h>

#define MAX_UART_QUEUE_DEPTH 32

typedef struct uartServerRequest{
    int endpoint;
    enum UARTMethod{
        POST,
        GET,
        OPT,
        GETLN,
        CLEAR,
        FLUSH,
        NOTIFY
    } method;
    int length;
    int opt;
    char* payload;
} uartRequest;

typedef struct uart_asyncDescriptor{
    uartRequest request;
    int requester;
} AsyncRequestDescriptor;

typedef struct asyncQueue{
    AsyncRequestDescriptor taskQueue[MAX_UART_QUEUE_DEPTH];
    int cursor;
    int length;
} AsyncTaskQueue;

typedef struct delimiterTracker{
    volatile bool enabled;
    volatile bool found;
    volatile char delimiter;
    volatile int maxSize;
} DelimiterTracker;

//TODO: remove this from public reference as everyone should be using the cleaner interface
void rxServer();
void txServer();

int createRxServer(int config);

int createTxServer(int config);

// void uartServer();

//Return types:
// >=0: Normal operation, # of chars fetched
// -1: Insufficient resources / failed
// -2: No delimiter found within max size
// -3: Current blocked by async request

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
