#ifndef TRACK_H
#define TRACK_H

#define TRAIN_COUNT 80
#define SENSOR_COUNT 80
#define SWITCH_COUNT 24

#include <stdbool.h>
/*
    Handles UART based operations for the track
    Dumb, i.e, not latency aware and does not perform scheduling
    That is up to the resource level abstraction
*/

typedef struct track_conductor{
    int trainSpeedStat[TRAIN_COUNT];
    int trackStat[SWITCH_COUNT];
    bool sensorStat[SENSOR_COUNT];
    int uartServer;
} Conductor;


//Not only responsible for sending the sole GO
//Should set all train speeds to 0 and set all switches one way
void start(int uart);

void engineSpeed(int uart, int train, int speed);

void reverse(int uart, int train);

void branch(int uart, int location, int direction);

//Solenoid
void turnOut(int uart);

void getSensorReading(int uart, bool* sensorBank);

#endif