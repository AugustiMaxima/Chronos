#include <syslib.h>
#include <timer.h>
#include <string.h>
#include <interrupt.h>
#include <nameServer.h>
#include <uartServer.h>
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
    bwprintf(COM2, "So long, fuckers\r\n");
    char buffer[32];
    Create(-1, nameServer);
    int server = Create(-1, uartServer);
    bwprintf(COM2, "Both creates were successfull\r\n");
    //PutCN(server, 2, "Basic test!\r\n", strlen("Basic test!\r\n"), true);
    GetCN(server, 2, buffer, 8, true);
    buffer[9] = 0;
    bwprintf(COM2, "Received buffer: %s\r\n", buffer);
    //PutCN(server, 2, buffer, 9, true);
}
