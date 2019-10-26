#include <stdlib.h>
#include <syslib.h>
#include <uart.h>
#include <uartServer.h>
#include <terminal.h>
#include <tui.h>


void drawInput(int TX2, char* buffer, int length){
    PutCN(TX2, 2, buffer, length, true);
}


//Needs the value of TX2 and RX2
//Low prioirity should be fine for this task
//Needs more handle on the track state, working on getting this now
void tuiThread(){
    //initial configuration
    int RX2;
    int TX2;
    // int CLK;
    int caller;
    Receive(&caller, (char*)(&RX2), sizeof(RX2));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)(&TX2), sizeof(TX2));
    Reply(caller, NULL, 0);

    //In case we are interested in implementing exponential backoff
    // Receive(&caller, (char*)(&CLK), sizeof(CLK));
    // Reply(caller, NULL, 0);

    char buffer[256];
    int index = 0;

    while(1){
        //Practically any event happening related to the user ui or train track should result in this call
        AwaitMultipleEvent(&caller, 2, INT_UART1, INT_UART2);
        int status = GleanUART(RX2, 2, index, buffer, 256);
        if(status > 0){
            index+=status;
            drawInput(TX2, buffer, status);
        }
    }
}
