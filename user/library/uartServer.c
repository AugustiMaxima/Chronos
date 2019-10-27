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

int getPhysicalAsyncTaskQueueIndex(int index){
    return index % MAX_UART_QUEUE_DEPTH;
}

void initializeAsyncTaskQueue(AsyncTaskQueue* queue){
    queue->cursor = 0;
    queue->length = 0;
}

AsyncRequestDescriptor* peekAsyncTaskQueue(AsyncTaskQueue* queue){
    if(queue->length - queue->cursor > 0)
        return queue->taskQueue + getPhysicalAsyncTaskQueueIndex(queue->cursor);
    return NULL;
}

AsyncRequestDescriptor* popAsyncTaskQueue(AsyncTaskQueue* queue){
    if(queue->length - queue->cursor > 0)
        return queue->taskQueue + getPhysicalAsyncTaskQueueIndex(queue->cursor++);
    return NULL;
}

int pushAsyncTaskQueue(AsyncTaskQueue* queue, uartRequest* request, int requester){
    if(MAX_UART_QUEUE_DEPTH - queue->length + queue->cursor <= 0)
        return 0;
    int index = getPhysicalAsyncTaskQueueIndex(queue->length++);
    queue->taskQueue[index].request.endpoint = request->endpoint;
    queue->taskQueue[index].request.length = request->length;
    queue->taskQueue[index].request.method = request->method;    
    queue->taskQueue[index].request.opt = request->opt;
    queue->taskQueue[index].request.payload = request->payload;
    queue->taskQueue[index].requester = requester;
    return index;
}


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
        if(device == GeneralInterrupt){
            if(val & 0x8){
                //Receive Timeout
                Send(tId, NULL, 0 ,NULL, 0);
            } else if(val & 0x2){
                //RX
                Send(tId, NULL, 0 ,NULL, 0);
            }
        } else {
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
    DelimiterTracker* delimit;

    int caller;
    Receive(&tId, (char*)&config, sizeof(config));
    Reply(tId, NULL, 0);
    Receive(&tId, (char*)&buffer, sizeof(buffer));
    Reply(tId, NULL, 0);
    Receive(&tId, (char*)&delimit, sizeof(delimit));
    Reply(tId, NULL, 0);

    int notifier = Create(-3, rxNotifier);
    Send(notifier, (const char*)&config, sizeof(config), NULL, 0);
    bool serverWaiting = false;
    while(1){
        Receive(&caller, NULL, 0);
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
                setReceiveInterrupt(config, true);
                setReceiveTimeout(config, true);
                break;
            } else {
                buffer->buffer[getPhysicalBufferIndex(buffer->length++)] = retainer;
                if(delimit->enabled && delimit->delimiter == retainer){
                    delimit->found = true;
                }
            }
        }
        if (!serverWaiting) {
	    } else if (delimit->enabled && !delimit->found && delimit->maxSize < getBufferFill(buffer)) {
        } else if(!delimit->enabled && delimit->maxSize > getBufferFill(buffer)){
	    } else {
	        serverWaiting = false;
            Reply(caller, NULL, 0);
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

void uxMessenger(){
    //Arguments
    int worker;
    int server;

    Receive(&server, (char*)&worker, sizeof(worker));
    Reply(server, NULL, 0);

    uartRequest request;
    request.method = NOTIFY;

    while(1){
        Send(server, (const char*)&request, sizeof(request), NULL, 0);
        Send(worker, NULL, 0, NULL, 0);
    }
}

bool processAsyncRxRequests(TransmitBuffer* buffer, AsyncTaskQueue* queue, DelimiterTracker* delimit){
    AsyncRequestDescriptor* task;
    while((task = popAsyncTaskQueue(queue))){
        int status;
        delimit->maxSize = task->request.length;
        delimit->found = false;
        if(task->request.method == GET){
            delimit->enabled = false;
            status = fetchBuffer(buffer, task->request.payload, task->request.length);
            if(status>=0){
                popAsyncTaskQueue(queue);
            } else {
                //implies we need to make another request again
                return false;
            }
        } else if(task->request.method == GETLN){
            delimit->enabled = true;
            delimit->delimiter = *(task->request.payload);
            status = readUntilDelimiter(buffer, task->request.payload, task->request.length, delimit->delimiter);
            if(status >= 0 || status == -2){
                popAsyncTaskQueue(queue);
            } else {
                *(task->request.payload) = delimit->delimiter;
                return false;
            }
        }
        Reply(task->requester, (const char*)&status, sizeof(status));
    }
    return true;
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
    uartRequest request;
    DelimiterTracker delimit;
    delimit.enabled = false;
    delimit.maxSize = 0;
    AsyncTaskQueue queue;
    initializeAsyncTaskQueue(&queue);

    int worker = Create(-2, rxWorker);
    void* ptr = &buffer;
    Send(worker, (const char*)&(config), sizeof(config), NULL, 0);
    Send(worker, (const char*)&(ptr), sizeof(ptr), NULL, 0);
    ptr = &delimit;
    Send(worker, (const char*)&(ptr), sizeof(ptr), NULL, 0);

    int messenger = Create(-1, uxMessenger);
    Send(messenger, (const char*)&worker, sizeof(worker), NULL, 0);

    bool workerReady = false;

    int status;

    while(1){
        Receive(&tId, (char*)&request, sizeof(request));
        if(request.method == GET){
            status = -3;
            if(peekAsyncTaskQueue(&queue))
		        status = fetchBuffer(&buffer, request.payload, request.length);
            if(status >= 0 || !request.opt){
                Reply(tId, (const char*)&status, sizeof(status));
            } else {                    
                pushAsyncTaskQueue(&queue, &request, tId);
                if(status == -1){
                    //File off request, if that is possible
                    delimit.maxSize = request.length;
                    delimit.enabled = false;
                    delimit.found = false;
                    if(workerReady){
                        Reply(messenger, NULL, 0);
                    }
                }
            }
        } else if(request.method == OPT){
            status = glean(&buffer, request.payload, request.opt, request.length);
            Reply(tId, (const char*)&status, sizeof(status));
        } else if(request.method == GETLN){
            char delimiter = *request.payload;
            if(peekAsyncTaskQueue(&queue))
                status = readUntilDelimiter(&buffer, request.payload, request.length, delimiter);
            if(status>=0 || !request.opt || status == -2){
                Reply(tId, (const char*)&status, sizeof(status));
            } else {                    
                pushAsyncTaskQueue(&queue, &request, tId);
                *request.payload = delimiter;
                if(status == -1){
                    //File off request, if that is possible
                    delimit.maxSize = request.length;
                    delimit.enabled = true;
                    delimit.found = false;
                    delimit.delimiter = delimiter;
                    if(workerReady){
                        Reply(messenger, NULL, 0);
                    }
                }
            }
        } else if(request.method == CLEAR){
            buffer.length = buffer.cursor;
            Reply(tId, NULL, 0);
        } else if(request.method == NOTIFY){
            workerReady = processAsyncRxRequests(&buffer, &queue, &delimit);
            if(!workerReady){
                Reply(messenger, NULL, 0);
            }
        }
    }
}

bool processAsyncTxRequests(TransmitBuffer* buffer, AsyncTaskQueue* queue){
    AsyncRequestDescriptor* task;
    int status;
    while((task = peekAsyncTaskQueue(queue))){
        if((task->request.method == POST)){
            status = fillBuffer(buffer, task->request.payload, task->request.length);
            if(status >= 0){
                popAsyncTaskQueue(queue);
            } else {
                return false;
            }
        }
        Reply(task->requester, (const char*)&status, sizeof(status));
    }
    return true;
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

    AsyncTaskQueue queue;
    initializeAsyncTaskQueue(&queue);
    
    int messenger = Create(-1, uxMessenger);
    Send(messenger, (const char*)&worker, sizeof(worker), NULL, 0);
    bool workerReady = false;

    int status;
    while(1){
        Receive(&tId, (char*)&request, sizeof(request));
        if(request.method == POST){
            status = -3;
            if(peekAsyncTaskQueue(&queue))
                status = fillBuffer(&buffer, request.payload, request.length);
            if(status >= 0 || !request.opt){
                Reply(tId, (const char*)&status, sizeof(status));
            } else {
                pushAsyncTaskQueue(&queue, &request, tId);
            }
            if(workerReady){
                Reply(messenger, NULL, 0);
            }
        } else if(request.method == FLUSH){
            buffer.length = buffer.cursor;
            Reply(tId, NULL, 0);
        } else if(request.method == NOTIFY){
            workerReady = processAsyncTxRequests(&buffer , &queue);
            if(!workerReady){
                Reply(messenger, NULL, 0);
            }
        }
    }

}

int createRxServer(int config){
    int rx = Create(-1, rxServer);
    Send(rx, (const char*)&config, sizeof(config), NULL, 0);
    return rx;
}

int createTxServer(int config){
    int tx = Create(-1, txServer);
    Send(tx, (const char*)&config, sizeof(config), NULL, 0);
    return tx;
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

void clearRXBuffer(int tid, int channel){
    uartRequest request;
    request.endpoint = channel;
    request.method = CLEAR;
    Send(tid, (const char*)&request, sizeof(request), NULL, 0);
}

void flushTXBuffer(int tid, int channel){
    uartRequest request;
    request.endpoint = channel;
    request.method = FLUSH;
    Send(tid, (const char*)&request, sizeof(request), NULL, 0);
}
