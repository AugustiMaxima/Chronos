#include <stdlib.h>
#include <syslib.h>
#include <tui.h>
#include <conductor.h>
#include <clockServer.h>
#include "sensorService.h"
#include <bwio.h>

void sensorService(){
    Conductor* conductor;
    TUIRenderState* prop;
    int controller;
    int clock;
    Receive(&controller, (char*)&conductor, sizeof(conductor));
    Reply(controller, NULL, 0);
    clock = conductor->CLK;
    Receive(&controller, (char*)&prop, sizeof(prop));
    Reply(controller, NULL, 0);
    int status = 1;
    while(status){
        //debatable whether this is needed, will be a subject of change later
        Delay(clock, 1);
        getSensorData(conductor);
        //notify the controller
        Send(controller, NULL, 0, (char*)&status, sizeof(status));
    }
    Destroy();
}

int createSensorService(Conductor* conductor, TUIRenderState* prop){
    int sensor = Create(-2, sensorService);
    Send(sensor, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(sensor, (const char*)&prop, sizeof(prop), NULL, 0);
    return sensor;
}
