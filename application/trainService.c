#include <stdlib.h>
#include <stdbool.h>
#include <syslib.h>
#include <math.h>
#include <clockServer.h>
#include "trainService.h"

//in mm
#define SAFE_DISTANCE 400

//more accurately, 5.92 mm / tick
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
        distance += pos->edge[DIR_STRAIGHT].dist;
    }

    if(pos->type == BRANCH){
        pruned = pruneDistinctPathSensors(data, sensorArray, distArray, pruned, distance, pos->edge[0].dest - data);
        pruned = pruneDistinctPathSensors(data, sensorArray, pruned, distance, pos->edge[1].dest - data);
    } else if(pos->type == SENSOR){
        //if this was a parallel algorithm, I'd have to use a global lock
        sensorArray[pruned] = pos - data;
        sensorArray[pruned++] = distance;
    }

    return pruned;
}


//only cares about a certain window -> anything beyond that, we will check when we need to
//-1*: impending doom
//0:nothing
//1:update position
//2:you better update now
CollisionData collisionDetection(Conductor* conductor, int position, TRACKEVENT* events, int eventIndex, int timeDisplacement){
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
    int size = pruneDistinctPathSensors(conductor->trackNodes, pruned, pruned_dist, 0, 0, position);
    int time = Time(conductor->CLK);
    int displacement = (time - timeDisplacement)*AVG_TRAIN_MAX_SPEED;
    CollisionData stat = {.impendingCollison = false, .updateStat = 0, .updateTarget = 0};

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
            stat.updateStat = 3;
            return stat;
        } else if(events[current].type == SENSOR){
            if(!graphMask[events[current].nodeId]){
                //blocked
                if(abs(dist - displacement) < MARGIN_OF_ERROR){
                    //activated trigger is within margin of error, likely just a figure of self
                    stat.updateStat = 1;
                    stat.updateTarget = current;
                } else {
                    //this is probably an impending collision
                    stat.impendingCollision = true;
                }
            }
        }
        if(current>=1)
            dist+=events[current - 1].distance;
        current++;
    }

    if(stat.updateTarget){
        return stat;
    }

    for(int i=0; i<size; i++){
        int node = pruned[i];
        if(!graphMask[node]){
            //block
            //distance
            if(abs(pruned_dist[i] - displacement) < MARGIN_OF_ERROR){
                //we assume that this means we might be here
                stat.updateStat = 2;
                stat.updateTarget = pruned[i];
                return stat;
            }
        }
    }
    return stat;
}

void dynamicPaths(TrainData* TrainData){
    int status = collisionFreePaths(trainData->conductor, trainData->position, trainData->destination, &trainData->path, trainData->events, TRACK_MAX);
    if(!status){
        parsePath(trainData->conductor->trackNodes, &trainData->path, trainData->events, TRACK_MAX, trainData->destination);
        trainData->eventIndex = 0;
        trainData->timeDisplacement = Time(trainData->conductor->CLK);
        setSpeedConductor(trainData->conductor, trainData->id, 14);
        trainData->stalled = false;
    }
}

int processIncomingEvents(TrainData* T){
    for(int i=T->eventIndex; i<MAX_TRACK){
        if(T->events[i].type == SENSOR)
            //only process up to the last sensor, everything else should be executed then
            break;
        else if(T->events[i].type == BRANCH){
            int swid = T->events[i].id;
            char direction = T->events[i].auxiliary == 0 ? 'S' : 'C';
            switchConductor(T->conductor, swid, direction);
        } else if(T->events[i].type == END){
            return 1;
        }
    }
    return 0;
}

void terminateTrain(TrainData* T){
    //termination
    setSpeedConductor(T->conductor, T->id, 0);
    //informs the controller of the heartbeat status
    Receive(&caller, NULL, 0);
    int status = 0;
    Reply(caller, (const char*)&status, sizeof(status));
    Destroy();
}

void trainService(){
    TrainData T;
    int caller;
    Receive(&caller, (char*)&T.conductor, sizeof(T.conductor));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&T.id, sizeof(T.id));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&T.position, sizeof(T.position));
    Reply(caller, NULL, 0);
    Receive(&caller, (char*)&T.destination, sizeof(T.destination));
    Reply(caller, NULL, 0);

    //blocker, prevents the trains from starting until a explicit go signal
    Receive(&caller, NULL, 0);
    Reply(caller, NULL, 0);

    int status;


    while(1){
        Receive(&caller, NULL, 0);
        Reply(caller, NULL, 0);
        if(stalled){//exclusive branch, should not affected or be part of the loop to be followed
            dynamicPaths(&T);
            continue;
        }

        CollisionData stat = collisionDetection(T.conductor, T.position, T.events, T.eventIndex, T.timeDisplacement);
        
        bool recomputePath = false;

        if(stat.updateStat == 1){
            //skips to node already reached
            T.eventIndex = stat.updateTarget;
            T.position = T.events[T.eventIndex].nodeId;
            T.timeDisplacement = Time(T.conductor->CLK);
            status = processIncomingEvents(&T);
            if(status){
                terminateTrain(&T);
            }
        } else if(stat.updateStat == 2){
            T.position = stat.updateTarget;
            T.timeDisplacement = Time(T.conductor->CLK);
            recomputePath = true;
        } else if(stat.updateStat == 3){
            terminateTrain(&T);
        }
        if(stat.impendingCollision){
            setSpeedConductor(T.conductor, T.id, 0);
            recomputePath = true;
        }

        if(recomputePath){
            dynamicPaths(&T);
        }
    }
}

int createTrainService(Conductor* conductor, int train, int position, int destination){
    int ts = Create(-1, trainService);
    Send(ts, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(ts, (const char*)&train, sizeof(train), NULL, 0);
    Send(ts, (const char*)&position, sizeof(position), NULL, 0);
    Send(ts, (const char*)&destination, sizeof(destination), NULL, 0);
    return ts;
}

