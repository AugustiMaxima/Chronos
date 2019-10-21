#include <uartServer.h>
#include <track.h>
#include <chlib.h>
#include <bwio.h>

#define MAXSPEED 15
#define SMOKELIGHTS 16
#define TURNOUT 32
#define STRAIGHT 33
#define CURVE 34
#define GO 96

void start(int uart){
    char command[1];
    command[0] = GO;
    PutCN(uart, 1, command, 1, true);
}

void engineSpeed(int uart, int train, int speed){
    char command[2];
    command[0] = speed;
    command[1] = train;
    PutCN(uart, 1, command, 2, true);
}

void reverse(int uart, int train){
    char command[2];
    command[0] = 15;
    command[1] = train;
    PutCN(uart, 1, command, 2, true);
}

void branch(int uart, int location, int direction){
    char command[2];
    switch (direction){
        case 'S':
            command[0] = STRAIGHT;
            break;
        case 'C':
            command[0] = CURVE;
            break;
        default:
            command[0] = 'X';
            break;
    }
    command[1] = location;
    PutCN(uart, 1, command, 2, true);
}

void turnOut(int uart){
    char command[1];
    command[0] = TURNOUT;
    PutCN(uart, 1, command, 1, true);
}

void sendSensorRequest(int uart){
    char command[1];
    command[0] = 0x85;
    PutCN(uart, 1, command, 1, true);
}

void getSensorReading(int uart, bool* sensorBank){
    char buffer[10];
    int status = GetCN(uart, 1, buffer, 10, false);
    bwprintf(COM2, "%d chars actually returned\r\n", status);
    char* sensorCast = (char*)sensorBank;
    for(int i=0;i<10;i++){
        sensorCast[i] = buffer[i];
    }
}
