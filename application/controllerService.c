#include <stdlib.h>
#include <syslib.h>
#include <tui.h>
#include "sensorService.h"
#include "controllerService.h"
#include <bwio.h>

void controllerService(){
    Conductor* conductor;
    int TS1, TS2, caller;
    Receive(&caller, (char*)&conductor, sizeof(conductor));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&TS1, sizeof(TS1));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&TS2, sizeof(TS2));
    Reply(caller, NULL, 0);
    TUIRenderState* prop;
    Receive(&caller, (char*)&prop, sizeof(prop));
    Reply(caller, NULL, 0);
    
    createSensorService(conductor, prop);

    //blocks master thread until this resumes
    int dominus;
    Receive(&dominus, NULL, 0);

    //start
    Send(TS1, NULL, 0, NULL, 0);
    Send(TS2, NULL, 0, NULL, 0);


    int TS1Active = 1;
    int TS2Active = 1;

    while(TS1Active || TS2Active){
        Receive(&caller, NULL, 0);
	if(TS1Active)
            Send(TS1, NULL, 0, (char*)&TS1Active, sizeof(TS1Active));
        if(TS2Active)
            Send(TS2, NULL, 0, (char*)&TS2Active, sizeof(TS2Active));
	if(TS1Active || TS2Active)
            Reply(caller, NULL, 0);
        else
            //if neither are active any more, means task complete
            Reply(caller, (const char*)&TS1Active, sizeof(TS1Active));
 
    }

    Reply(dominus, NULL, 0);
    Destroy();
}

int createControllerService(Conductor* conductor, int trainServer1, int trainServer2, TUIRenderState* prop){
    int controller = Create(-2, controllerService);
    Send(controller, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(controller, (const char*)&trainServer1, sizeof(trainServer1), NULL, 0);
    Send(controller, (const char*)&trainServer2, sizeof(trainServer2), NULL, 0);
    Send(controller, (const char*)&prop, sizeof(prop), NULL, 0);
    return controller;
}
