#include <syslib.h>
#include <timer.h>
#include <string.h>
#include <interrupt.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <deviceTests.h>
#include <bwio.h>

void windows(){
    int val;
    int device;
    initializeTimer(2, 508000, 5080, 1); // 10ms
    enableDeviceInterrupt(TC2UI_DEV_ID);
    while(1){
	    device = AwaitMultipleEvent(&val, 2, TC1UI_DEV_ID, TC2UI_DEV_ID);		
	    bwprintf(COM2, "Timer %d with value %d\r\n", device, val);
    }
}


//let's try to root out all of the issues potentailly in the uart server
void uartServerTest(){
    char buffer[32];
    Create(-1, nameServer);
    int server = Create(-1, uartServer);
    PutCN(server, 2, "Basic test!\r\n", strlen("Basic test!\r\n"), true);
    GetCN(server, 2, buffer, 5, true);
    buffer[5] = 0;
    bwprintf(COM2, "GetCN succeeded\r\n");
    PutCN(server, 2, buffer, 6, true);
    bwprintf(COM2, "Did it print?\r\n");
}

void control(){
    Create(-1, nameServer);
    int uart = Create(-1, uartServer);
    int clock = Create(-1, clockServer);
    Conductor conductor;
    conductor.uartServer = uart;

    while(1){
        getSensorReading(uart, conductor.sensorStat);
        char sensorCount[82];
        for(int i=0;i<80;i++){
            if(conductor.sensorStat[i]){
                sensorCount[i] = 'X';
            }else{
                sensorCount[i] = '_';
            }
        }
        sensorCount[80] = '\r';
        sensorCount[81] = '\n';
        PutCN(uart, 2, "Printing sensors:\r\n", strlen("Printing sensors:\r\n"), true);
        PutCN(uart, 2, sensorCount, 82, true);
        Delay(clock, 100);
    }
}
