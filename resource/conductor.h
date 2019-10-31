#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include <track_data.h>
#include <map.h>

#define TRAIN_COUNT 80
#define SENSOR_COUNT 80
#define SWITCH_COUNT 22

//here is me hard coding some shitty values so the train can keep going
typedef struct lens_Track{
    //all the shit you need to resolve switch numbers 
    char indexToSwitch[SWITCH_COUNT];
    char switchToIndex[160];
    char indexToNode[SWITCH_COUNT];

    //sensor number to the node id
    char sensorToNode[SENSOR_COUNT];
} lEvUlUtIoN;


typedef struct conductor{
    //configuration stuff
    int RX;
    int TX;
    int CLK;

    bool sensor[SENSOR_COUNT];
    //records last speed command sent
    int trainSpeed[TRAIN_COUNT];
    //Switches are named weirdly, beware
    char switches[SWITCH_COUNT];

    track_node trackNodes[TRACK_MAX];

    lEvUlUtIoN index;

} Conductor;


void initializeConductor(Conductor* conductor, int RX, int TX, int CLK, int track);

void setSpeedConductor(Conductor* conductor, int train, int speed);

void reverseConductor(Conductor* conductor, int train);

void switchConductor(Conductor* conductor, int location, char state);

void getSensorData(Conductor* conductor);

#endif