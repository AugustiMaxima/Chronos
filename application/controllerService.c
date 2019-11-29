#include <stdlib.h>
#include "controllerService.h"


void controllerService(){
    Conductor* conductor;
    int TS1, TS2, caller;
    Receive(&caller, (const char*)conductor, sizeof(conductor), NULL, 0);
    Reply(caller, NULL, 0);
    Receive(&caller, (const char*)TS1, sizeof(TS1), NULL, 0);
    Reply(caller, NULL, 0);
    Receive(&caller, (const char*)TS2, sizeof(TS2), NULL, 0);
    Reply(caller, NULL, 0);
    
    while(1){
        Receive(&caller, NULL, 0);
        Send(TS1, NULL, 0, NULL, 0);
        Send(TS2, NULL, 0, NULL, 0);
    }
}

int createControllerService(Conductor* conductor, int trainServer1, int trainServer2){
    int controller = Create(-2, controllerService);
    Send(controller, (const char*)conductor, sizeof(conductor), NULL, 0);
    Send(controller, (const char*)trainServer1, sizeof(trainServer1), NULL, 0);
    Send(controller, (const char*)trainServer2, sizeof(trainServer2), NULL, 0);
    return controller;
}