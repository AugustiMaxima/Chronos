#include <uartServer.h>
#include <clockServer.h>
#include <track.h>
#include <chlib.h>
#include <track_data.h>
#include <bwio.h>

#define MAXSPEED 15
#define SMOKELIGHTS 16
#define TURNOUT 32
#define STRAIGHT 33
#define CURVE 34
#define GO 96
#define STOP 97

void startTrack(int uart){
    char command[1];
    command[0] = GO;
    PutCN(uart, 1, command, 1, true);
}

void engineSpeedTrack(int uart, int clk, int train, int speed){
    char command[2];
    command[0] = speed;
    command[1] = train;
    PutCN(uart, 1, command, 2, true);
}

void reverseTrack(int uart, int clk, int train){
    engineSpeedTrack(uart, clk, train, 15);
}

void branchTrack(int uart, int clk, int location, char direction){
    char command[2];
    switch (direction){
        case 'S':
	case 's':
            command[0] = STRAIGHT;
            break;
        case 'C':
	case 'c':
            command[0] = CURVE;
            break;
        default:
            command[0] = 'X';
            break;
    }
    command[1] = location;
    PutCN(uart, 1, command, 2, true);
}

void turnOutTrack(int uart){
    char command[1];
    command[0] = TURNOUT;
    PutCN(uart, 1, command, 1, true);
}

void sendSensorRequestTrack(int uart){
    char command[1];
    command[0] = 0x85;
    PutCN(uart, 1, command, 1, true);
}

void getSensorReadingTrack(int uart, char* sensorBank){
    GetCN(uart, 1, sensorBank, 10, true);
}


void stoptrack(int uart){
    
}
