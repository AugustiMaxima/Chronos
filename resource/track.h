#ifndef TRACK_H
#define TRACK_H

#include <stdbool.h>
/*
    Handles UART based operations for the track
    Dumb, i.e, not latency aware and does not perform scheduling
    That is up to the resource level abstraction
*/

//Not only responsible for sending the sole GO
//Should set all train speeds to 0 and set all switches one way
void startTrack(int uart);

void engineSpeedTrack(int uart, int clk, int train, int speed);

void reverseTrack(int uart, int clk, int train);

void branchTrack(int uart, int clk, int location, char direction);

//Solenoid
void turnOutTrack(int uart);

void sendSensorRequestTrack(int uart);

void getSensorReadingTrack(int uart, char* sensorBank);

#endif
