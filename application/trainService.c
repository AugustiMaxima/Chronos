#include <stdlib.h>
#include <stdbool.h>
#include <pathFinder.h>
#include "trainService.h"

//in mm
#define SAFE_DISTANCE 400


void generateGraphMask(bool* graphMask, bool* sensor, track_node* trackData){
    for(int i=0;i<TRACK_MAX;i++){
        if(trackData[i].type == SENSOR){
            //if the sensor is true, it means it is currently occupied
            graphMask[i] = !sensor[trackData[i].num];
        } else {
            //since we cannot detect occupation, we will default to optimistic case
            graphMask[i] = true;
        }
    }
}


int collisionFreePaths(Conductor* conductor, int position, int destination, PATH* path, TRACKEVENT* events, int eventLength){
    bool graphMask[TRACK_MAX];
    generateGraphMask(graphMask, conductor->sensor, conductor->trackNodes);
    graphMask[position] = true;

    computePath(conductor->trackNodes, path, graphMask, position, destination);
    
    if(path->cost[destination] == -1)
        return 1;

    parsePath(conductor->trackNodes, path, events, eventLength, destination);
    return 0;
}


//only cares about a certain window -> anything beyond that, we will check when we need to
//-1: collision
//0:nothing
//1:update
int collisionDetection(Conductor* conductor, int* position, TRACKEVENT* events, int eventIndex){
    int dist = 0;
    





}


void trainService(){
    Conductor* conductor;
    int train;
    int position;
    int eventIndex = 0;
    int displacement = -1;
    int destination;
    int caller;
    Receive(&caller, (char*)&conductor, sizeof(conductor));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&train, sizeof(train));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&position, sizeof(position));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&destination, sizeof(destination));
    Reply(caller, NULL, 0);

    bool graphMask[TRACK_MAX];

    PATH path;
    TRACKEVENT events[TRACK_MAX];

    int status;
    status = collisionFreePaths(conductor, position, destination, &path, events, TRACK_MAX);

    if(!status)
        setSpeedConductor(conductor, train, 14);

    while(1){
        Receive(&caller, NULL, 0);






    }

}

int createTrainService(Conductor* condcutor, int train, int position, int destination){
    





}

