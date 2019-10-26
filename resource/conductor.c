#include <track.h>
#include <clockServer.h>
#include <conductor.h>
#include <bwio.h>

//should take in RX1 and TX1
void initializeConductor(Conductor* conductor, int RX, int TX, int CLK){
    conductor->RX = RX;
    conductor->TX = TX;
    conductor->CLK = CLK;
    startTrack(TX);
    Delay(CLK, 40);

    //get the switches set
    for(int i=1;i<=SWITCH_COUNT;i++){
        conductor->switches[i] = 'S';
        branchTrack(TX, i, 'S');
        Delay(CLK, 40);
    }
    turnOutTrack(TX);
    bwprintf(COM2, "Switch set once\r\n");
    
    Delay(CLK, 40);

    //get the switches set
    for(int i=1;i<=SWITCH_COUNT;i++){
        conductor->switches[i] = 'C';
        branchTrack(TX, i, 'C');
        Delay(CLK, 40);
    }
    turnOutTrack(TX);
    bwprintf(COM2, "Switch set twice\r\n");

    Delay(CLK, 40);

    //get the trains set
    for(int i=1;i<TRAIN_COUNT;i++){
        conductor->trainSpeed[i] = 0;
        engineSpeedTrack(TX, i, 0);
        Delay(CLK, 40);
    }

    bwprintf(COM2, "Trains\r\n");

    //Consider doing something here
    //get the sensor stat
    getSensorData(conductor);
    bwprintf(COM2, "Done\r\n");
}

void setSpeedConductor(Conductor* conductor, int train, int speed){
    conductor->trainSpeed[train] = speed;
    engineSpeedTrack(conductor->TX, train, speed);
}


void reverseConductor(Conductor* conductor, int train){
    int speed = conductor->trainSpeed[train];
    engineSpeedTrack(conductor->TX, train, 0);
    Delay(conductor->CLK, 18*speed*speed);
    engineSpeedTrack(conductor->TX, train, 18);
    Delay(conductor->CLK, 18);
    engineSpeedTrack(conductor->TX, train, speed);
}

void switchConductor(Conductor* conductor, int location, char state){
    conductor->switches[location] = state;
    branchTrack(conductor->TX, location, state);
    Delay(conductor->CLK, 18);
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
