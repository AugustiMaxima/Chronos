#include <track.h>
#include <clockServer.h>
#include <uartServer.h>
#include <conductor.h>
#include <bwio.h>

//should take in RX1 and TX1
void initializeConductor(Conductor* conductor, int RX, int TX, int CLK, int track){

    if(track == 1){
        init_tracka(conductor->trackNodes);
    } else if(track == 2){
        init_trackb(conductor->trackNodes);
    }

    int branchIndex = 0;
    int sensorIndex = 0;

    //160 bytes is actually cheaper than any competing abstract data structure for storage
    //also faster
    for(int i=0; i<160; i++){
        conductor->index.switchToIndex[i] = (char)255; //indicates invalid
    }

    for(int i=0; i<TRACK_MAX; i++){
        if(conductor->trackNodes[i].type == NODE_BRANCH){
	    // bwprintf(COM2, "Branch Index: %d, Label: %d\r\n", branchIndex, conductor->trackNodes[i].num);
            conductor->index.indexToSwitch[branchIndex] = conductor->trackNodes[i].num;
            conductor->index.switchToIndex[conductor->trackNodes[i].num] = branchIndex;
            conductor->index.indexToNode[branchIndex++] = i;
        }
        else if(conductor->trackNodes[i].type == NODE_SENSOR){
	    // bwprintf(COM2, "Sensor Index: %d\r\n", sensorIndex);
            conductor->index.sensorToNode[sensorIndex++] = i;
        }
    }

    // bwprintf(COM2, "%d, %d\r\n", branchIndex, sensorIndex);


    conductor->RX = RX;
    conductor->TX = TX;
    conductor->CLK = CLK;
    startTrack(TX);
    Delay(CLK, 100);
    
    //get the switches set
    for(int i=0;i<SWITCH_COUNT;i++){
        conductor->switches[i] = 'S';
        branchTrack(TX, CLK, conductor->index.indexToSwitch[i], 'S');
        Delay(CLK, 30);
    }
    turnOutTrack(TX);
    
    Delay(CLK, 40);

    //get the switches set
    for(int i=0;i<SWITCH_COUNT;i++){
        conductor->switches[i] = 'C';
        branchTrack(TX, CLK, conductor->index.indexToSwitch[i], 'C');
        Delay(CLK, 30);
    }
    turnOutTrack(TX);

    Delay(CLK, 40);

    //get the trains set
    /*for(int i=1;i<TRAIN_COUNT;i++){
        setSpeedConductor(conductor, i, 0);
        Delay(CLK, 40);
    }*/

    // Consider doing something here
    // get the sensor stat
    getSensorData(conductor);
}

void setSpeedConductor(Conductor* conductor, int train, int speed){
    conductor->trainSpeed[train] = speed;
    engineSpeedTrack(conductor->TX, conductor->CLK, train, speed);
}


void reverseConductor(Conductor* conductor, int train){
    int speed = conductor->trainSpeed[train];
    engineSpeedTrack(conductor->TX, conductor->CLK, train, 0);
    Delay(conductor->CLK, 2*speed*speed);
    engineSpeedTrack(conductor->TX, conductor->CLK, train, 15);
    Delay(conductor->CLK, 10);
    engineSpeedTrack(conductor->TX, conductor->CLK, train, speed);
}

void switchConductor(Conductor* conductor, int location, char state){
    int index = conductor->index.switchToIndex[location];
    if(state == 'C' || state == 'c')
        conductor->switches[index] = 'C';
    else if(state == 'S' || state == 's')
        conductor->switches[index] = 'S';
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
