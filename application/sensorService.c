#include <stdlib.h>
#include "sensorService.h"


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
    while(1){
        //debatable whether this is needed, will be a subject of change later
        Delay(clock, 1);
        getSensorData(conductor);
        //notify the controller
        Send(controller, NULL, 0, NULL, 0);
    }
}

int createSensorService(Conductor* conductor, TUIRenderState* prop){
    int sensor = Create(-2, sensorService);
    Send(sensor, (const char*)conductor, sizeof(conductor), NULL, 0);
    Send(sensor, (const char*)prop, sizeof(prop), NULL, 0);
    return sensor;
}
