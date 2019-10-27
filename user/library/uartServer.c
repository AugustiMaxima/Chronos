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

typedef struct delimiterTracker{
    bool enabled;
    bool found;
    char delimiter;
    int maxSize;
} DelimiterTracker;

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
                setReceiveInterrupt(config, true);
                setReceiveTimeout(config, true);
                break;
            } else {
                buffer->buffer[getPhysicalBufferIndex(buffer->length++)] = retainer;
                if(delimit.enabled && delimit.delimiter == retainer){
                    delimit.found = true;
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
        } else if(request.method == CLEAR){
            buffer.length = buffer.cursor;
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
        } else if(request.method == FLUSH){
            buffer.length = buffer.cursor;
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
