#include <ts7200.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslib.h>
#include <uart.h>
#include <interrupt.h>
#include <transmitBuffer.h>
#include <nameServer.h>
#include <uartServer.h>
#include <marklinServer.h>
#include <bwio.h>


void marklinRxNotifier(){
    int rxWorker;
    Receive(&rxWorker, NULL, 0);
    Reply(rxWorker, NULL, 0);
    //enableDeviceInterrupt(INT_UART1);
    enableDeviceInterrupt(UART1RX_DEV_ID);

    //int device, val;

    while(1){
        /*val = AwaitEvent(INT_UART1);
        if(val & 0x8){
            //Receive Timeout
            Send(rxWorker, NULL, 0, NULL, 0);
        } else if(val & 0x2){
            //RX
            Send(rxWorker, NULL, 0, NULL, 0);
        }*/
	AwaitEvent(UART1RX_DEV_ID);
	//bwprintf(COM2, "Print\r\n");
	Send(rxWorker, NULL, 0, NULL, 0);
    }
}

void marklinTxNotifier(){
    int txWorker;
    Receive(&txWorker, NULL, 0);
    Reply(txWorker, NULL, 0);
    enableDeviceInterrupt(INT_UART1);

    while(1){
        int val = AwaitEvent(INT_UART1);
        if(val & 0x4){
            //TX
            Send(txWorker, NULL, 0, NULL, 0);
        } else if(val & 0x1){
            //MODEM
	    if(getUartFlag(1) & CTS_MASK)
		Send(txWorker, NULL, 0, NULL, 0);
        } else if(!val){
	    if(getUartFlag(1) & CTS_MASK)
		Send(txWorker, NULL, 0, NULL, 0);
	}
    }
}

void marklinRxWorker(){
    TransmitBuffer* buffer;
    int server;
    int caller;
    int notifier = Create(-4, marklinRxNotifier); 

    Send(notifier, NULL, 0, NULL, 0);
    Receive(&server, (char*)&buffer, sizeof(buffer));
    Reply(server, NULL, 0);
    char retainer;
    int status = 0;
    bool serverBlocked = false;
    while(1){
        Receive(&caller, NULL, 0);
        if(caller == notifier){
            Reply(notifier, NULL, 0);
        } else {
            serverBlocked = true;
        }
        while(getBufferCapacity(buffer) > 0){
            status = getUart(1, &retainer);
            if(status){
		//bwprintf(COM2, "%d %d\r\n", getBufferFill(buffer), buffer->length);
                setReceiveInterrupt(1, true);
                setReceiveTimeout(1, true);
                break;
            } else {
                buffer->buffer[getPhysicalBufferIndex(buffer->length++)] = retainer;
            }
        }
        if(serverBlocked){
            serverBlocked = false;
            Reply(server, NULL, 0);
        }
    }
}

void marklinTxWorker(){
    TransmitBuffer* buffer;
    int server;
    int caller;
    int notifier = Create(-4, marklinTxNotifier);
    Send(notifier, NULL, 0, NULL, 0);
    Receive(&server, (char*)&buffer, sizeof(buffer));
    Reply(server, NULL, 0);
    bool serverBlocked = false;
    while(1){
        Receive(&caller, NULL, 0);
        if(caller==notifier){
            Reply(caller, NULL, 0);
        } else {
            serverBlocked = true;
        }
        while(getBufferFill(buffer) > 0){
            int status = putUart(1, buffer->buffer[getPhysicalBufferIndex(buffer->cursor)]);
            if(status & TXFF_MASK){
                setTransmitInterrupt(1, true);
            }
            if(status){
                break;
            } else {
                buffer->cursor++;

                Receive(&caller, NULL, 0);
                Reply(caller, NULL, 0);
                //bwprintf(COM2, "CTS Reup\r\n");
            }
        }
        if(serverBlocked){
            Reply(server, NULL, 0);
        }
    }
}

void marklinRxServer(){
    TransmitBuffer RX;

    RegisterAs("MR1");

    initializeTransmitBuffer(&RX);

    int RXWorker = Create(-4, marklinRxWorker);
    void* buff = &RX;
    Send(RXWorker, (const char*)&buff, sizeof(buff), NULL, 0);

    uartRequest request;
    int caller;
    int status;
    while(1){
        Receive(&caller, (char*)&request, sizeof(request));
        if(request.method == GET){
            do{
		//bwprintf(COM2, "B");
                Send(RXWorker, NULL, 0, NULL, 0);
		//bwprintf(COM2, "F");
                status = fetchBuffer(&RX, request.payload, request.length);
		//bwprintf(COM2, "%d\r\n", status);
            } while(status < 0);
            Reply(caller, (const char*)&status, sizeof(status));
        }
    }

}

void marklinTxServer(){
    TransmitBuffer TX;

    RegisterAs("MT1");

    initializeTransmitBuffer(&TX);

    int TXWorker = Create(-3, marklinTxWorker);
    void* buff = &TX;
    Send(TXWorker, (const char*)&buff, sizeof(buff), NULL, 0);

    uartRequest request;
    int caller;
    int status;
    while(1){
        Receive(&caller, (char*)&request, sizeof(request));
        if(request.method == POST){
	    //bwprintf(COM2, "POST\r\n");
            do{
                status = fillBuffer(&TX, request.payload, request.length);
                Send(TXWorker, NULL, 0, NULL, 0);
            } while(status < 0);
            Reply(caller, (const char*)&status, sizeof(status));
        }
    }

}
