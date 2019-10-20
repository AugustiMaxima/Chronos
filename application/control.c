#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>

#include <track.h>

void control(){
    int uart = Create(-2, uartServer);

    Conductor conductor;
    conductor.uartServer = uart;

    while(1){
    //     getSensorReading(uart, conductor.sensorStat);
    //     char* sensor = (char*)conductor.sensorStat;
    //     PutCN(uart, 2, sensor, 10, true);
    }

}