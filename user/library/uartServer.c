#include <ts7200.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uart.h>
#include <interrupt.h>
#include <nameServer.h>
#include <transmitBuffer.h>
#include <uartServer.h>

#define ASYNC_POOL_SIZE 32

typedef struct async_UartRequestTable{
    int cursor;
    int length;
    uartRequest requests[ASYNC_POOL_SIZE];
} AsyncUARTRequests;

int getPhysicalUartRequestIndex(int index){
    return index%ASYNC_POOL_SIZE;
}

void initializeAsyncUARTRequests(AsyncUARTRequests* requests){
    requests->cursor = 0;
    requests->length = 0;
}

uartRequest* peekAsyncUARTRequests(AsyncUARTRequests* requests){
    if(requests->length - requests->cursor > 0)
        return requests->requests + getPhysicalUartRequestIndex(requests->cursor);
    return NULL;
}

uartRequest* popAsyncUARTRequests(AsyncUARTRequests* requests){
    if(requests->length - requests->cursor > 0)
        return requests->requests + getPhysicalUartRequestIndex(requests->cursor++);
    return NULL;
}

int pushAsyncUARTRequests(AsyncUARTRequests* requests, uartRequest* request){
    int index = getPhysicalUartRequestIndex(requests->length++);
    requests->requests[index].endpoint = request->endpoint;
    requests->requests[index].length = request->length;
    requests->requests[index].method = request->method;    
    requests->requests[index].opt = request->opt;
    requests->requests[index].payload = request->payload;
}

void uartRequestLogger(uartRequest request){	
    if(request.method==POST){
	    bwprintf(COM2, "POST ->");
    } else if(request.method==GET){
	    bwprintf(COM2, "GET ->");
    } else if(request.method==OPT){
	    bwprintf(COM2, "OPT ->");
    } else if(request.method==NOTIFY){
	    bwprintf(COM2, "NOTIFY ->");
    }	
    bwprintf(COM2, "chos://%d:%d\r\n", request.endpoint, request.length);
    if(request.method==POST){
	    bwprintf(COM2, "\tWith attached payload:\r\n\t\t");
	    for(int i=0;i<request.length;i++){
	        bwprintf(COM2, "%c", request.payload[i]);
	    }
	    bwprintf(COM2, "\r\n");
    } 
}

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
    
    Receive(&uartServer, &reply, sizeof(reply));
    Reply(uartServer, &reply, sizeof(reply));

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

    request.method = NOTIFY;
    request.endpoint = reply;

    while(1){
        deviceId = AwaitMultipleEvent(&val, 3, rx, tx, in);
        if(deviceId == rx){
            request.length = 1;
        } else if(deviceId == tx){
            request.length = 2;
        } else if(deviceId == in) {
            if(val && 0x8){
                request.length = 3;
            } else if(val && 0x4){
                request.length = 2;
            } else if(val && 0x2){
                request.length = 1;
            } else if (val && 0x1){
                request.length = 0;
            }
        }
        Send(uartServer, (const char*)&request, sizeof(request), (char*)&reply, sizeof(reply));
    }
}

void ProcessAsyncRequest(){

}

void uartServer(){
    uartRequest request;

    //creates and configures the notifier for uart1 and uart2
    int config = 1;
    int uart1 = Create(-2, uartNotifier);
    int uart2 = Create(-2, uartNotifier);
    Send(uart1, &config, sizeof(config), request, 1);
    config++;
    Send(uart2, &config, sizeof(config), request, 1);

    //buffer configuration
    bool receiveReady1 = true;
    bool transmitReady1 = true;
    bool receiveReady2 = true;
    bool transmitReady2 = true;
    int status;

    TransmitBuffer rBuffer1;
    TransmitBuffer rBuffer2;
    TransmitBuffer tBuffer1;
    TransmitBuffer tBuffer2;

    initializeTransmitBuffer(&rBuffer1);
    initializeTransmitBuffer(&rBuffer2);
    initializeTransmitBuffer(&tBuffer1);
    initializeTransmitBuffer(&tBuffer2);

    TransmitBuffer *receive, *transmit;

    AsyncUARTRequests rRequest1;
    AsyncUARTRequests tRequest1;
    AsyncUARTRequests rRequest2;
    AsyncUARTRequests tRequest2;

    initializeAsyncUARTRequests(&rRequest1);
    initializeAsyncUARTRequests(&tRequest1);
    initializeAsyncUARTRequests(&rRequest2);
    initializeAsyncUARTRequests(&tRequest2);

    AsyncUARTRequests *receiveRequest, *transmitRequest;

    // Leaves registration for the last, once this is registered its hot and primed to go
    RegisterAs("UART");

    while(1){
        Receive(&config, &request, sizeof(request));

	// uartRequestLogger(request);
        if(request.endpoint == 1){
            receive = &rBuffer1;
            transmit = &tBuffer1;
        } else {
            receive = &rBuffer2;
            transmit = &tBuffer2;
        }
        if(request.method == POST){
            status = fillBuffer(transmit, request.payload, request.length);
            if(status<0 && request.opt){
                pushAsyncUARTRequests(transmitRequest, &request);
            }
        } else if(request.method == GET){
            if(peekAsyncUARTRequests(receiveRequest))
            status = fetchBuffer(receive, request.payload, request.length);
            if(status<0 && request.opt){
                pushAsyncUARTRequests(receiveRequest, &request);
            }
        } else if(request.method == OPT){
            status = glean(receive, request.payload, request.opt, request.length);
        } else if(request.method == NOTIFY) {
            if(request.endpoint == 1){
                if(request.length%2){
                    receiveReady1 = true;
                } else {
                    transmitReady1 = true;
                }
            } else if (request.endpoint == 2){
                if(request.length%2){
                    receiveReady2 = true;
                } else {
                    transmitReady2 = true;
                }
            }
        }
        Reply(config, &status, sizeof(status));
        
        //Some light processing
        status = 0;
        char retainer;
        if(receiveReady1){
            while(rBuffer1.cursor < rBuffer1.length){
                status = get(1, &retainer);
                if (status) {
                    receiveReady1 = false;
                    enableDeviceInterrupt(UART1RX_DEV_ID);
                    break;
                } else {
                    rBuffer1.buffer[rBuffer1.cursor++] = retainer;
                }
            }
        }
        if(transmitReady1){
            while(tBuffer1.cursor < tBuffer1.length){
                status = put(1, tBuffer1.buffer[tBuffer1.cursor]);
                if (status) {
                    transmitReady1 = false;
                    enableDeviceInterrupt(UART1TX_DEV_ID);
                    break;
                } else {
                    tBuffer1.cursor++;
                }
            }
        }
        if(receiveReady2){
            while(rBuffer2.cursor < rBuffer2.length){
                status = get(2, &retainer);
                if (status) {
                    receiveReady2 = false;
                    enableDeviceInterrupt(UART2RX_DEV_ID);
                    break;
                } else {
                    rBuffer2.buffer[rBuffer2.cursor++] = retainer;
                }
            }
        }
        if(transmitReady2){
            while(tBuffer2.cursor < tBuffer2.length){
                status = put(2, tBuffer2.buffer[tBuffer2.cursor]);
                if (status) {
                    transmitReady2 = false;
                    enableDeviceInterrupt(UART2TX_DEV_ID);
                    break;
                } else {
                    tBuffer2.cursor++;
                }
            }
        }
    }
}

int Getc(int tid, int channel){
    char locale;
    int status = GetCN(tid, channel, &locale, 1, false);
    if(status==1){
        return locale;
    }
    return status;
}

int Putc(int tid, int channel, char ch){
    int status = PutCN(tid, channel, &ch, 1, false);
    if(status == 1){
        return ch;
    }
    return status;
}

int GetCN(int tid, int channel, char* buffer, int length, bool async){
    uartRequest request;
    request.endpoint = channel;
    request.method = GET;
    request.length = length;
    request.payload = buffer;
    request.opt = async;
    int response;
    Send(tid, &request, sizeof(request), &response, sizeof(response));
    return response;
}

int PutCN(int tid, int channel, char* buffer, int length, bool async){
    uartRequest request;
    request.endpoint = channel;
    request.method = POST;
    request.length = length;
    request.payload = buffer;
    request.opt = async;
    int response;
    Send(tid, &request, sizeof(request), &response, sizeof(response));
    return response;
}

int GleanUART(int tid, int channel, int offset, char* buffer, int length){
    uartRequest request;
    request.endpoint = channel;
    request.method = OPT;
    request.length = length;
    request.opt = offset;
    request.payload = buffer;
    int response;
    Send(tid, &request, sizeof(request), &response, sizeof(response));
    return response;
}
