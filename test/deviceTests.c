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


// //let's try to root out all of the issues potentailly in the uart server
// void uartServerTest(){
//     char buffer[32];
//     Create(-1, nameServer);
//     int server = Create(-1, uartServer);
//     PutCN(server, 2, "Basic test!\r\n", strlen("Basic test!\r\n"), true);
//     GetCN(server, 2, buffer, 5, true);
//     buffer[5] = 0;
//     bwprintf(COM2, "GetCN succeeded\r\n");
//     PutCN(server, 2, buffer, 6, true);
//     bwprintf(COM2, "Did it print?\r\n");
// }

void uartServerTest(){
    int buf = 1;
    Create(-1, nameServer);
    int RX1 = Create(-1, rxServer);
    Send(RX1, (const char*)&buf, sizeof(buf), NULL, 0);
    int TX1 = Create(-1, txServer);
    char buf2[5];
    buf2[4] = 0;
    while(1){
	GetCN(RX1, 1, buf2, 10, true);
	bwprintf(COM2, "%s\r\n", buf2);
    }
    
}
void control(){
    Create(-1, nameServer);
    int RX1 = Create(-1, rxServer);
    int RX2 = Create(-1, rxServer);
    int TX1 = Create(-1, txServer);
    int TX2 = Create(-1, txServer);
    int config = 1;
    Send(RX1, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX1, (const char*)&config, sizeof(config), NULL, 0);
    config++;
    Send(RX2, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX2, (const char*)&config, sizeof(config), NULL, 0);
    int clock = Create(-1, clockServer);
    
    bool sensorBanks[80];
    char sensorCount[83];
    
    while(1){
        sendSensorRequest(TX1);
        Delay(clock, 100);
        getSensorReading(RX1, sensorBanks);
        for(int i=0;i<80;i++){
            if(sensorBanks[i]){
                sensorCount[i] = 'X';
            }else{
                sensorCount[i] = '_';
            }
        }
        sensorCount[80] = '\r';
        sensorCount[81] = '\n';
        sensorCount[82] = 0;
        // PutCN(uart, 2, "Printing sensors:\r\n", strlen("Printing sensors:\r\n"), true);
        // PutCN(uart, 2, sensorCount, 82, true);
        bwprintf(COM2, "Printing sensors:\r\n%s\r\n", sensorCount);
        Delay(clock, 100);
    }
}
