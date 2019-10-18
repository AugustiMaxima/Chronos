#include <ts7200.h>
#include <uart.h>
#include <nameServer.h>
#include <uartServer.h>


void uartNotifier(){
    int uartServer = -1;
    
    //Await device
    int deviceId;
    int val;
    
    //UartServer requests
    uartRequest request;
    int reply;
    
    //UART configs
    int uartbase;
    int tx;
    int rx;
    int in;
    
    while(uartServer == -1)
        uartServer = WhoIs("UART");
    
    request.endpoint = 0;
    request.method = NOTIFY;
    request.length = 0;

    Send(uartServer, (const char*)&request, sizeof(request), (char*)&reply, sizeof(reply));

    if(reply == 1){
        uartbase = UART1_BASE;
        tx = UART1TX_DEV_ID;
        rx = UART1RX_DEV_ID;
        in = INT_UART1;
    } else if (reply == 2) {
        uartbase = UART2_BASE;
        tx = UART2TX_DEV_ID;
        rx = UART2RX_DEV_ID;
        in = INT_UART2;
    }

    request.endpoint = reply;

    while(1){
        deviceId = AwaitMultipleEvent(&val, 3, tx, rx, in);
        if(deviceId == tx){
            request.length = 1;
        } else if(deviceId == rx){
            request.length = 2;
        } else if(deviceId == in) {
            if(val && 0x8){
                request.length = 3;
            }
        }
        Send(uartServer, (const char*)&request, sizeof(request), (char*)&reply, sizeof(reply));
    }
}

void uartServer(){

}

int Getc(int tid, int channel);

int Putc(int tid, int channel, char ch);

int GetCN(int tid, int channel, char* buffer, int length);

int PutCN(int tid, int channel, char* buffer, int length);