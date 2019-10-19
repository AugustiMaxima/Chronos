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


void uartServerTest(){
    Create(-1, nameServer);
    int server = Create(-1, uartServer);
    PutCN(server, 2, "Basic test!\r\n", strlen("Basic test!\r\n"), true);
}
