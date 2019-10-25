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

/*
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
*/
typedef struct delimiterTracker{
    bool enabled;
    bool found;
    char delimiter;
    int maxSize;
} DelimiterTracker;
/*
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
    } else if(request.method==GETLN){
	    bwprintf(COM2, "GETLN ->");
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

	enableDeviceInterrupt(tx);
	enableDeviceInterrupt(rx);
	enableDeviceInterrupt(in);

    request.method = NOTIFY;
    request.endpoint = reply;

    while(1){
        deviceId = AwaitMultipleEvent(&val, 3, rx, tx, in);
        //NOTIFY payloads uses length as a status flag
        //Resembles the general interrupt flag
        if(deviceId == rx){
            request.length = 2;
            request.opt = val;
        } else if(deviceId == tx){
            request.length = 4;
        } else if(deviceId == in) {
            request.length = val;
            if(request.length & 0x2){
                char bite;
                getUart(request.endpoint, &bite);
                request.opt = bite;
                // bwprintf(COM2, "Get char: %c\r\n", bite);
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
    // bwprintf(COM2, "Retrying async request: Relogging \r\n");
    // uartRequestLogger(request);
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
}*/


void rxNotifier(){
    int config;
    int RXInterrupt;
    int GeneralInterrupt;
    int tId;
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);

    if(config==1){
        RXInterrupt = UART1RX_DEV_ID;
        GeneralInterrupt = INT_UART1;
    } else {
        RXInterrupt = UART2RX_DEV_ID;
        GeneralInterrupt = INT_UART2;
    }

    enableDeviceInterrupt(RXInterrupt);
    enableDeviceInterrupt(GeneralInterrupt);

    int val;
    int device;

    while(1){
        device = AwaitMultipleEvent(&val, 2, RXInterrupt, GeneralInterrupt);
        // bwprintf(COM2, "Triggered RX notif\r\n");
        if(device == GeneralInterrupt){
            // bwprintf(COM2, "General flag: %x\r\n", val);
            if(val & 0x8){
                //Receive Timeout
		// bwprintf(COM1, "RX event received\r\n");
                Send(tId, NULL, 0 ,NULL, 0);
            } else if(val & 0x2){
                //RX
		// bwprintf(COM1, "RX event received\r\n");
                Send(tId, NULL, 0 ,NULL, 0);
            }
        } else {
	    // bwprintf(COM1, "RX event received\r\n");
            Send(tId, NULL, 0 ,NULL, 0);
        }
    }
}

void txNotifier(){
    int config;
    int TXInterrupt;
    int GeneralInterrupt;
    int tId;
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);

    if(config==1){
        TXInterrupt = UART1TX_DEV_ID;
        GeneralInterrupt = INT_UART1;
    } else {
        TXInterrupt = UART2TX_DEV_ID;
        GeneralInterrupt = INT_UART2;
    }

    enableDeviceInterrupt(TXInterrupt);
    enableDeviceInterrupt(GeneralInterrupt);

    int val;
    int device;

    while(1){
        device = AwaitMultipleEvent(&val, 2, TXInterrupt, GeneralInterrupt);
        if(device == GeneralInterrupt){
            if(val & 0x4){
                //TX
                Send(tId, NULL, 0, NULL, 0);
            } else if(val & 0x1){
                //MODEM
                Send(tId, NULL, 0, NULL, 0);
            }
        } else {
            Send(tId, NULL, 0, NULL, 0);
        }
    }
}


void rxWorker(){
    TransmitBuffer* buffer;
    int config;
    int tId;
    int caller;
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);
    Receive(&tId, (char*)&buffer, sizeof(buffer));
    Reply(tId, NULL, 0);
    int notifier = Create(-3, rxNotifier);
    Send(notifier, (const char*)&config, sizeof(config), NULL, 0);
    DelimiterTracker delimit;
    delimit.enabled = false;
    delimit.maxSize = 0;
    bool serverWaiting = false;
    while(1){
        Receive(&caller, (char*)&delimit, sizeof(delimit));
	if(caller == tId){
	    serverWaiting = true;
	} else {
	    Reply(caller, NULL, 0);
	}
        char retainer;
        int status;
        //Processing
        while(getBufferCapacity(buffer) > 0){
            status = getUart(config, &retainer);
            if(status){
		// bwprintf(COM1, "Buffer empty\r\n");
                setReceiveInterrupt(config, true);
                setReceiveTimeout(config, true);
                break;
            } else {
		// bwprintf(COM1, "Got value: %d\r\n", retainer);
                buffer->buffer[getPhysicalBufferIndex(buffer->length++)] = retainer;
		if(delimit.enabled){
		    // bwprintf(COM1, "FYI, delimiter is %d\r\n", delimit.delimiter);
		}
                if(delimit.enabled && delimit.delimiter == retainer){
                    delimit.found = true;
		    // bwprintf(COM1, "Matched\r\n");
                }
            }
        }
        if (!serverWaiting) {
	} else if (delimit.enabled && !delimit.found && delimit.maxSize < getBufferFill(buffer)) {
        } else if(!delimit.enabled && delimit.maxSize > getBufferFill(buffer)){
	} else {
	    serverWaiting = false;
            Reply(tId, NULL, 0);
        }
    }
}

void txWorker(){
    TransmitBuffer* buffer;
    int config;
    int tId;
    int caller;
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);
    Receive(&tId, (char*)&buffer, sizeof(buffer));
    Reply(tId, NULL, 0);
    int notifier = Create(-3, txNotifier);
    Send(notifier, (char*)&config, sizeof(config), NULL, 0);
    while(1){
        Receive(&caller, NULL, 0);
	if(caller == notifier)
	    Reply(notifier, NULL, 0);
        while(buffer->cursor < buffer->length){
            int status = putUart(config, buffer->buffer[getPhysicalBufferIndex(buffer->cursor)]);
            if(status & TXFF_MASK){
                setTransmitInterrupt(config, true);
            }
            if(status){
                break;
            } else {
                buffer->cursor++;
            }
        }
	if(caller == tId)
	    Reply(caller, NULL, 0);
    }
}

void rxServer(){
    int tId;
    int config;
    TransmitBuffer buffer;
    initializeTransmitBuffer(&buffer);
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);
    if(config == 1){
        RegisterAs("RX1");
    } else {
        RegisterAs("RX2");
    }
    int worker = Create(-2, rxWorker);
    TransmitBuffer* ptr = &buffer;
    Send(worker, (const char*)&(config), sizeof(config), NULL, 0);
    Send(worker, (const char*)&(ptr), sizeof(ptr), NULL, 0);
    uartRequest request;
    DelimiterTracker delimit;
    int status;
    while(1){
        Receive(&tId, (char*)&request, sizeof(request));
        if(request.method == GET){
		    status = fetchBuffer(&buffer, request.payload, request.length);
            while(status==-1 && request.opt){
                delimit.enabled = false;
                delimit.maxSize = request.length;
                Send(worker, (const char*)&delimit, sizeof(delimit), NULL, 0);
                status = fetchBuffer(&buffer, request.payload, request.length);
            }
        } else if(request.method == OPT){
            status = glean(&buffer, request.payload, request.opt, request.length);
        } else if(request.method == GETLN){
            char delimiter = *request.payload;
            status = readUntilDelimiter(&buffer, request.payload, request.length, delimiter);
            while (status==-1 && request.opt){
                delimit.enabled = true;
                delimit.found = false;
                delimit.delimiter = delimiter;
                delimit.maxSize = request.length;
                *request.payload = delimiter;
                Send(worker, (const char*)&delimit, sizeof(delimit), NULL, 0);
                status = readUntilDelimiter(&buffer, request.payload, request.length, delimiter);
            }
	    delimit.enabled = false;
	    delimit.found = false;
	    //bwprintf(COM1, "Success read! Index now at %d, with %d\r\n", buffer.cursor, buffer.buffer[buffer.cursor]);
        }
        Reply(tId, (const char*)&status, sizeof(status));
    }
}

void txServer(){
    int tId;
    int config;
    TransmitBuffer buffer;
    initializeTransmitBuffer(&buffer);
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);
    if(config == 1){
        RegisterAs("TX1");
    } else {
        RegisterAs("TX2");
    }
    int worker = Create(-2, txWorker);
    TransmitBuffer* ptr = &buffer;
    Send(worker, (const char*)&(config), sizeof(config), NULL, 0);
    Send(worker, (const char*)&(ptr), sizeof(ptr), NULL, 0);
    uartRequest request;
    int status;
    while(1){
        Receive(&tId, (char*)&request, sizeof(request));
        if(request.method == POST){
            status = fillBuffer(&buffer, request.payload, request.length);
	    Send(worker, NULL, 0, NULL, 0);
            while (status<0 && request.opt){
                status = fillBuffer(&buffer, request.payload, request.length);
                Send(worker, NULL, 0, NULL, 0);
            }
            Reply(tId, (const char*)&status, sizeof(status));
        }
    }

}

/*
* Something something from monolithic to micro services
* Separate the worker from the user request to improve latency
*
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

    delimit1.enabled = false;
    delimit2.enabled = false;

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
        } else 
	if(!deferred)
	    Reply(config, (char*)&status, sizeof(status));
        
        //Some light processing
        status = 0;
        char retainer;

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
            } else {
                receive = &rBuffer2;
                transmit = &tBuffer2;
                uState = &uState2;
                delimit = &delimit2;
                receiveRequest = &rRequest2;
                transmitRequest = &tRequest2;
            }

            //Processing loop
            if(uState->rx){
                while(getBufferCapacity(receive) > 0){
                    status = getUart(j, &retainer);
                    if(status) {
                        uState->rx = false;
                        setReceiveInterrupt(j, true);
                        setReceiveTimeout(j, true);
                        break;
                    } else {
                        receive->buffer[receive->length++] = retainer;
                        if(delimit->enabled && delimit->delimiter == retainer){
                            delimit->found = true;
                        }
                    }
                }

                bwprintf(COM2, "%d\n", getBufferFill(receive));

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

            if(uState->tx && uState->cts){
                while(transmit->cursor < transmit->length){
                    status = putUart(j, transmit->buffer[transmit->cursor]);
                    if(status){
                        if(status & CTS_MASK){
                            uState->cts = false;
                        }
                        if(status & TXFF_MASK){
                            uState->tx = false;
                            setTransmitInterrupt(j, true);
                        }
                        break;
                    } else {
                        transmit->cursor++;
                    }
                }
                ProcessAsyncUartRequest(transmitRequest, NULL, transmit);
            }
        }
    }
}*/

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

