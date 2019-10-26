#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#define TRAIN_COUNT 80
#define SENSOR_COUNT 80
#define SWITCH_COUNT 22

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

} Conductor;


void initializeConductor(Conductor* conductor, int RX, int TX, int CLK);

void setSpeedConductor(Conductor* conductor, int train, int speed);

void reverseConductor(Conductor* conductor, int train);

void switchConductor(Conductor* conductor, int direction, char state);

void getSensorData(Conductor* conductor);

#endif