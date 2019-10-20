#include <ts7200.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslib.h>
#include <uart.h>
#include <interrupt.h>
#include <nameServer.h>
#include <transmitBuffer.h>
#include <uartServer.h>
#include <bwio.h>

#define ASYNC_POOL_SIZE 32

typedef struct async_UartRequest{
    uartRequest request;
    int requester;
}asyncUartRequest;

typedef struct async_UartRequestQueue{
    int cursor;
    int length;
    asyncUartRequest requests[ASYNC_POOL_SIZE];
} AsyncUartRequests;

int getPhysicalUartRequestIndex(int index){
    return index%ASYNC_POOL_SIZE;
}

void initializeAsyncUartRequests(AsyncUartRequests* requests){
    requests->cursor = 0;
    requests->length = 0;
}

asyncUartRequest* peekAsyncUartRequests(AsyncUartRequests* requests){
    if(requests->length - requests->cursor > 0)
        return requests->requests + getPhysicalUartRequestIndex(requests->cursor);
    return NULL;
}

asyncUartRequest* popAsyncUartRequests(AsyncUartRequests* requests){
    if(requests->length - requests->cursor > 0)
        return requests->requests + getPhysicalUartRequestIndex(requests->cursor++);
    return NULL;
}

int pushAsyncUartRequests(AsyncUartRequests* requests, uartRequest* request, int requester){
    if(ASYNC_POOL_SIZE - requests->length + requests->cursor <= 0)
        return 0;
    int index = getPhysicalUartRequestIndex(requests->length++);
    requests->requests[index].request.endpoint = request->endpoint;
    requests->requests[index].request.length = request->length;
    requests->requests[index].request.method = request->method;    
    requests->requests[index].request.opt = request->opt;
    requests->requests[index].request.payload = request->payload;
    requests->requests[index].requester = requester;
    return index;
}

typedef struct delimiterTracker{
    bool enabled;
    bool found;
    bool unknown;
    char delimiter;
    int maxSize;
} DelimiterTracker;

typedef struct uartStatus{
    bool rx;
    bool tx;
    bool cts;
} UartStatus;

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
    // int uartbase;
    int tx;
    int rx;
    int in;
    
    Receive(&uartServer, (char*)&reply, sizeof(reply));
    Reply(uartServer, (char*)&reply, sizeof(reply));

    if(reply == 1){
        // uartbase = UART1_BASE;
        tx = UART1TX_DEV_ID;
        rx = UART1RX_DEV_ID;
        in = INT_UART1;
    } else {
        // uartbase = UART2_BASE;
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

int ProcessAsyncUartRequest(AsyncUartRequests* requests, TransmitBuffer* receive, TransmitBuffer* transmit){
    asyncUartRequest* asyncReq = peekAsyncUartRequests(requests);
    if(!asyncReq)
	    return 0;
    uartRequest request = asyncReq->request;
    int status;
    if(request.method == POST){
        status = fillBuffer(transmit, request.payload, request.length);
    } else if(request.method == GET){
	    status = fetchBuffer(receive, request.payload, request.length);
    } else if (request.method == GETLN){
        char delimiter = *request.payload;
        status = readUntilDelimiter(receive, request.payload, request.length, delimiter);
        if(status<0){
            *request.payload = delimiter;
        }
    }
    if(status>=0){
	    popAsyncUartRequests(requests);
        Reply(asyncReq->requester, (char*)&status, sizeof(status));
    }
    return status;
}

void uartServer(){
    uartRequest request;

    //creates and configures the notifier for uart1 and uart2
    int config = 1;
    int uart1 = Create(-2, uartNotifier);
    int uart2 = Create(-2, uartNotifier);
    Send(uart1, (const char*)&config, sizeof(config), (char*)&request, 1);
    config++;
    Send(uart2, (const char*)&config, sizeof(config), (char*)&request, 1);

    //buffer configuration
    UartStatus uState1, uState2, *uState;
    uState1.cts = true;
    uState1.rx = true;
    uState1.tx = true;
    uState2.cts = true;
    uState2.rx = true;
    uState2.tx = true;

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

    AsyncUartRequests rRequest1;
    AsyncUartRequests tRequest1;
    AsyncUartRequests rRequest2;
    AsyncUartRequests tRequest2;

    initializeAsyncUartRequests(&rRequest1);
    initializeAsyncUartRequests(&tRequest1);
    initializeAsyncUartRequests(&rRequest2);
    initializeAsyncUartRequests(&tRequest2);

    AsyncUartRequests *receiveRequest, *transmitRequest;

    // Leaves registration for the last, once this is registered its hotdelimiter and primed to go
    RegisterAs("UART");

    DelimiterTracker delimit1, delimit2, *delimit;

    while(1){
        Receive(&config, (char*)&request, sizeof(request));
	    bool deferred = false;
	    // uartRequestLogger(request);
        //Configuration
        if(request.endpoint == 1){
            receive = &rBuffer1;
            transmit = &tBuffer1;
	        receiveRequest = &rRequest1;
	        transmitRequest = &tRequest1;
            delimit = &delimit1;
            uState = &uState1;
        } else {
            receive = &rBuffer2;
            transmit = &tBuffer2;
	        receiveRequest = &rRequest2;
	        transmitRequest = &tRequest2;
            delimit = &delimit2;
            uState = &uState2;
        }
        if(request.method == POST){
            status = fillBuffer(transmit, request.payload, request.length);
            if(status<0 && request.opt){
		        deferred = true;
                pushAsyncUartRequests(transmitRequest, &request, config);
            }
        } else if(request.method == GET){
	        status = -3;
            if(!peekAsyncUartRequests(receiveRequest))
		        status = fetchBuffer(receive, request.payload, request.length);
            if(status==-3 || (status==-1&&request.opt)){
		        deferred = true;
		        pushAsyncUartRequests(receiveRequest, &request, config);
            }
        } else if(request.method == OPT){
            status = glean(receive, request.payload, request.opt, request.length);
        } else if(request.method == GETLN){
            status = -3;
            char delimiter = *request.payload;
            if(!peekAsyncUartRequests(receiveRequest))
                status = readUntilDelimiter(receive, request.payload, request.length, delimiter);
            if(status==-3){
                deferred = true;
                pushAsyncUartRequests(receiveRequest, &request, config);
            } else if (status==-1 && request.opt){
                deferred = true;
                pushAsyncUartRequests(receiveRequest, &request, config);
                delimit->enabled = true;
                delimit->found = false;
                delimit->unknown = false;
                delimit->delimiter = delimiter;
                delimit->maxSize = request.length;
                *request.payload = delimiter;
            }
        } else if(request.method == NOTIFY) {
            if(request.length%2){
                uState->rx = true;
            } else if (request.length == 3){
                uState->tx = true;
            } else {
                uState->cts = true;
            }
        }
	    if(!deferred)
	        Reply(config, (char*)&status, sizeof(status));
        
        //Some light processing
        status = 0;
        char retainer;

        int rxInterrupt;
        int txInterrupt;

        asyncUartRequest* asyncRequest;

        for(int j=1;j<3;j++){
            //Configuration
            if(j==1){
                receive = &rBuffer1;
                transmit = &tBuffer1;
                uState = &uState1;
                delimit = &delimit1;
                receiveRequest = &rRequest1;
                transmitRequest = &tRequest1;
                rxInterrupt = UART1RX_DEV_ID;
                txInterrupt = UART1TX_DEV_ID;
            } else {
                receive = &rBuffer2;
                transmit = &tBuffer2;
                uState = &uState2;
                delimit = &delimit2;
                receiveRequest = &rRequest2;
                transmitRequest = &tRequest2;
                rxInterrupt = UART2RX_DEV_ID;
                txInterrupt = UART2TX_DEV_ID;
            }

            //Processing loop
            if(uState->rx){
                while(receive->cursor < receive->length){
                    status = get(j, &retainer);
                    if(status) {
                        uState->rx = false;
                        enableDeviceInterrupt(rxInterrupt);
                        break;
                    } else {
                        receive->buffer[receive->cursor++] = retainer;
                        if(delimit->enabled && delimit->delimiter == retainer){
                            delimit->found = true;
                        }
                    }
                }

                // Logic to facilitate efficient look up of getByDelimiter operation
                // may seem excessive, but the majority of read request to uart server can be done this way efficiently
                status = -2;
                if(delimit->enabled && !delimit->found){
                    if(delimit->unknown){
                        status = ProcessAsyncUartRequest(receiveRequest, receive, NULL);
                        delimit->unknown = false;
                    } else if(delimit->maxSize <= getBufferFill(receive)){
                        asyncRequest = popAsyncUartRequests(receiveRequest);
                        Reply(asyncRequest->requester, (char*)&status, sizeof(status));
                        delimit->enabled = false;
                    }
                } else {
                    status = ProcessAsyncUartRequest(receiveRequest, receive, NULL);
                }   
                if(status >= 0){
                    delimit->enabled = false;
                    asyncRequest = peekAsyncUartRequests(receiveRequest);
                    if(asyncRequest && asyncRequest->request.method == GETLN){
                        //prepare the delimit check for the next gen
                        delimit->enabled = true;
                        delimit->delimiter = *asyncRequest->request.payload;
                        delimit->maxSize = asyncRequest->request.length;
                        delimit->found = false;
                        delimit->unknown = true;
                    }
                }
            }
            if(uState->tx){
                while(transmit->cursor < transmit->length){
                    status = put(j, transmit->buffer[transmit->cursor]);
                    if(status){
                        //this needs better refactoring
                        uState->tx = false;
                        enableDeviceInterrupt(txInterrupt);
                        break;
                    } else {
                        transmit->cursor++;
                    }
                }
                ProcessAsyncUartRequest(transmitRequest, NULL, transmit);
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
    Send(tid, (const char*)&request, sizeof(request), (char*)&response, sizeof(response));
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
    Send(tid, (const char*)&request, sizeof(request), (char*)&response, sizeof(response));
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
    Send(tid, (const char*)&request, sizeof(request), (char*)&response, sizeof(response));
    return response;
}

int GetLN(int tid, int channel, char* buffer, int length, char delimiter, bool async){
    uartRequest request;
    request.endpoint = channel;
    request.method = GETLN;
    request.length = length;
    request.opt = async;
    request.payload = buffer;
    *request.payload = delimiter;
    int response;
    Send(tid, (const char*)&request, sizeof(request), (char*)&response, sizeof(response));
    return response;
}

