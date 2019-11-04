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
#include <stdlib.h>
#include <terminal.h>
#include <conductor.h>

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
    
    // char sensorBanks[10];
    // char sensorCount[83];
    
    // while(1){
    //     sendSensorRequest(TX1);
    //     Delay(clock, 100);
    //     getSensorReading(RX1, sensorBanks);
    //     //PutCN(TX2, 2, "Printing sensors:\r\n", strlen("Printing sensors:\r\n"), true);
    //     // PutCN(TX2, 2, sensorCount, 82, true);
    //     //bwprintf(COM2, "Printing sensors:\r\n%s\r\n", sensorCount);
    //     Delay(clock, 100);
    // }

    // startTrack(TX1);
    // Delay(clock, 40);
    // branchTrack(TX1, 4, 'S');
    // Delay(clock, 40);
    // branchTrack(TX1, 4, 'C');
    // Delay(clock, 40);
    // turnOutTrack(TX1);
    // engineSpeedTrack(TX1, 1, 12);


    char sensorBanks[10];
    while(1){        
        sendSensorRequestTrack(TX1);
        getSensorReadingTrack(RX1, sensorBanks);
        for(int i=0;i<10;i++)
            bwprintf(COM2, "%d ", sensorBanks[i]);
        bwprintf(COM2, "\r\n");
        for(int i=0; i<10; i++){
            for(int j=0;j<8;j++){
                if(sensorBanks[i] & (1<<j)){
                    bwprintf(COM2, "X");                    
                } else {
                    bwprintf(COM2, "_");
                }
            }
        }
        bwprintf(COM2, "\r\n");
    }

}

void controlServer(){
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

    Conductor conductor;
    initializeConductor(&conductor, RX1, TX1, clock, 2);
}

void switchTest(){
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

    char buffer[10];

    startTrack(TX1);

    Delay(clock, 100);
    
    sendSensorRequestTrack(TX1);
    getSensorReadingTrack(RX1, buffer);
 
    Delay(clock, 100);
  
    branchTrack(TX1, clock, 1, 'S');

    Delay(clock, 40);

    turnOutTrack(TX1);
    
    Delay(clock, 40);
    
    branchTrack(TX1, clock, 1, 'C');

    Delay(clock, 40);

    turnOutTrack(TX1);
}

void rawSwitch(){
    Create(-1, nameServer);
    int clock = Create(-1, clockServer); 
    bwputc(COM1, 96);
    Delay(clock, 40);
    Delay(clock, 500);
    bwputc(COM1, 33);
    Delay(clock, 10);
    bwputc(COM1, 1);
    Delay(clock, 40);
    bwputc(COM1, 32);
    Delay(clock, 40);
    bwputc(COM1, 34);
    Delay(clock, 10);
    bwputc(COM1, 1);
    Delay(clock, 40);
    bwputc(COM1, 32);
}
