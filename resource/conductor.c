#include <track.h>
#include <clockServer.h>
#include <uartServer.h>
#include <conductor.h>
#include <bwio.h>

//should take in RX1 and TX1
void initializeConductor(Conductor* conductor, int RX, int TX, int CLK){
    conductor->RX = RX;
    conductor->TX = TX;
    conductor->CLK = CLK;
    startTrack(TX);
    Delay(CLK, 100);

    //get the switches set
    for(int i=1;i<=SWITCH_COUNT;i++){
        conductor->switches[i] = 'S';
        branchTrack(TX, CLK, i, 'S');
        Delay(CLK, 30);
    }
    turnOutTrack(TX);
    
    Delay(CLK, 40);

    //get the switches set
    for(int i=1;i<=SWITCH_COUNT;i++){
        conductor->switches[i] = 'C';
        branchTrack(TX, CLK, i, 'C');
        Delay(CLK, 30);
    }
    turnOutTrack(TX);

    Delay(CLK, 40);

    //get the trains set
    for(int i=1;i<TRAIN_COUNT;i++){
        conductor->trainSpeed[i] = 0;
        engineSpeedTrack(TX, i, 0);
        Delay(CLK, 40);
    }

    //Consider doing something here
    //get the sensor stat
    getSensorData(conductor);
}

void setSpeedConductor(Conductor* conductor, int train, int speed){
    conductor->trainSpeed[train] = speed;
    engineSpeedTrack(conductor->TX, train, speed);
}


void reverseConductor(Conductor* conductor, int train){
    int speed = conductor->trainSpeed[train];
    engineSpeedTrack(conductor->TX, train, 0);
    Delay(conductor->CLK, 20*speed*speed);
    engineSpeedTrack(conductor->TX, train, 15);
    Delay(conductor->CLK, 10);
    engineSpeedTrack(conductor->TX, train, speed);
}

void switchConductor(Conductor* conductor, int location, char state){
    if(state == 'C' || state == 'c')
        conductor->switches[location] = 'C';
    else if(state == 'S' || state == 's')
        conductor->switches[location] = 'S';
    else
        return;
    branchTrack(conductor->TX, conductor->CLK, location, state);
    Delay(conductor->CLK, 20);
    turnOutTrack(conductor->TX);
}

void getSensorData(Conductor* conductor){
    char sensorBank[10];
    sendSensorRequestTrack(conductor->TX);
    getSensorReadingTrack(conductor->RX, sensorBank);
    for(int i=0; i<10; i++){
        for(int j=0;j<8;j++){
            conductor->sensor[i*8 + j] = (sensorBank[i] & (1<<j) )>>j;
        }
    }
}
