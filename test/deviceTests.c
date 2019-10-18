#include <syslib.h>
#include <interrupt.h>
#include <deviceTests.h>
#include <bwio.h>

void windows(){
    int val;
    int device;
    initializeTimer(2, 508000, 5080, 1); // 10ms
    bwprintf(COM2, "windows\r\n");
    while(1){
	device = AwaitMultipleEvent(&val, 2, TC1UI_DEV_ID, TC2UI_DEV_ID);		
	bwprintf(COM2, "Timer %d with value %d\r\n", device, val);
    }
}
