#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pathFinder.h>
#include <clockServer.h>
#include "trainService.h"

//in mm
#define SAFE_DISTANCE 400

//more accurately, 4.92 mm / tick
//we are really only using this for low precision apporximation with high factors of safety so it should be ok
#define AVG_TRAIN_MAX_SPEED 600

//Estimated difference and observed difference within 100mm will be merged
#define MARGIN_OF_ERROR 100

#define MAX_DIVERGENCE 4

void generateGraphMask(bool* graphMask, bool* sensor, track_node* trackData){
    for(int i=0;i<TRACK_MAX;i++){
        if(trackData[i].type == SENSOR){
            //if the sensor is true, it means it is currently occupied
            //mirrored and reflected for accurate operation
            graphMask[i] = !(sensor[trackData[i].num] || sensor[trackData[i].reverse->num]);
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

int pruneDistinctPathSensors(track_node* data, int* sensorArray, int* distArray, int pruned, int distance, int position){
    track_node* pos = data + position;

    while(1){
        //stopping condition:
        //either we find the root
        //or we will have multiple root in which case we branch out
        if(pos->type == SENSOR || pos->type == BRANCH)
            break;
        
        pos = pos->edge[DIR_STRAIGHT].dest;
    }

    if(pos->type == BRANCH){
        pruned = pruneDistinctPathSensors(data, sensorArray, pruned, pos->edge[0].dest - data);
        pruned = pruneDistinctPathSensors(data, sensorArray, pruned, pos->edge[1].dest - data);
    } else if(pos->type == SENSOR){
        //if this was a parallel algorithm, I'd have to use a global lock
        sensorArray[pruned++] = pos - data;
    }

    return pruned;
}


//only cares about a certain window -> anything beyond that, we will check when we need to
//-1: impending doom
//0:nothing
//1:update position
//2:you better update now
int collisionDetection(Conductor* conductor, int position, TRACKEVENT* events, int eventIndex, int timeDisplacement){
    int dist = 0;
    //Two kinds of potential position updates:
    //expected: on the current path
    //unexpected: "plausible" locations, we define this as the immediate sensors reachable through distinct paths
    //technically, given an unbound number of branches, this could actually be infinite
    //since we don't have dynamic memory (lol) and people actually need sensors to coordinate positions (lol)
    //the actual tracks seems to have at most 4 distinct paths, during exit sections
    //guess how big the pruned array is...
    int pruned[MAX_DIVERGENCE];
    int pruned_dist[MAX_DIVERGENCE];
    int size = pruneDistinctPathSensors(conductor->trackNodes, pruned, pruned_dist, 0, position);
    int time = Time(conductor->CLK);
    int displacement = (time - timeDisplacement)*AVG_TRAIN_MAX_SPEED;

    bool graphMask[TRACK_MAX];
    generateGraphMask(graphMask, conductor->sensor, conductor->trackNodes);

    //consume events between this box
    //anything beyond will not be looked at until the next iteration

    int current = eventIndex;

    while(dist - displacement < SAFE_DISTANCE){
        if(events[current].type == REVERSE){
        //we don't really, or shouldn't really look ahead
        //might be subject to change
            break;
        } else if(events[current].type == END){
        //what can you do?
            break;
        } else if(events[current].type == SENSOR){
            if(!graphMask[events[current].nodeId]){
                //blocked
                if(abs(dist - displacement) < MARGIN_OF_ERROR){
                    //activated trigger is within margin of error, likely just a figure of self
                    return 1;
                } else {
                    //this is probably an impending collision
                    return -1;
                }
            }
        }
        if(current>=1)
            dist+=events[current - 1].distance;
        current++;
    }

    for(int i=0; i<size; i++){
        int node = pruned[i];
        if(!graphMask[node]){
            //block
            //distance




        }
    }

}


void trainService(){
    Conductor* conductor;
    int train;
    int position;
    int eventIndex = 0;
    //update this when you update position
    int timeDisplacement = 0;
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

