#include <stdlib.h>
#include <stdbool.h>
#include <syslib.h>
#include <math.h>
#include <clockServer.h>
#include <conductor.h>
#include "trainService.h"
#include <bwio.h>

//in mm
#define SAFE_DISTANCE 800
#define RESERVE_DISTANCE 400

//more accurately, 5.92 mm / tick
//we are really only using this for low precision apporximation with high factors of safety so it should be ok
#define AVG_TRAIN_MAX_SPEED 600

//Estimated difference and observed difference within 100mm will be merged
#define MARGIN_OF_ERROR 100

#define MAX_DIVERGENCE 4

void TrainLogger(TrainData* T, const char* message){
   bwprintf(COM2, "Train: %d at %s:\t%s\r\n", T->id, T->conductor->trackNodes[T->position].name, message); 
}

void generateGraphMask(bool* graphMask, Conductor* conductor, int id){
    struct track_node* trackData = conductor->trackNodes;
    char* sensor = conductor->sensor;
    for(int i=0;i<TRACK_MAX;i++){
        if(trackData[i].type == NODE_SENSOR){
            //if the sensor is true, it means it is currently occupied
            //mirrored and reflected for accurate operation
            graphMask[i] = !(sensor[trackData[i].num] || sensor[trackData[i].reverse->num] || (conductor->reserved[i] != id && conductor->reserved[i]));
        } else {
            //since we cannot detect occupation, we will default to optimistic case
            graphMask[i] = true;
        }
    }
}


int collisionFreePaths(Conductor* conductor, int position, int destination, PATH* path, TRACKEVENT* events, int eventLength, int id){
    bool graphMask[TRACK_MAX];
    generateGraphMask(graphMask, conductor, id);
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

        if(pos->type == NODE_SENSOR || pos->type == NODE_BRANCH)
            break;
        
        pos = pos->edge[DIR_STRAIGHT].dest;
        distance += pos->edge[DIR_STRAIGHT].dist;
    }

    if(pos->type == NODE_BRANCH){
        pruned = pruneDistinctPathSensors(data, sensorArray, distArray, pruned, distance, pos->edge[0].dest - data);
	pruned = pruneDistinctPathSensors(data, sensorArray, distArray, pruned, distance, pos->edge[1].dest - data);
    } else if(pos->type == NODE_SENSOR){
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
CollisionData collisionDetection(Conductor* conductor, int position, TRACKEVENT* events, int eventIndex, int timeDisplacement, int id){
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
    CollisionData stat = {.impendingCollision = false, .updateStat = 0, .updateTarget = 0};

    bool graphMask[TRACK_MAX];
    generateGraphMask(graphMask, conductor, id);

    //consume events between this box
    //anything beyond will not be looked at until the next iteration

    int current = eventIndex + 1;

    bwprintf(COM2, "Expected position: %s + %d\r\n", conductor->trackNodes[position].name, displacement);

    displacement = 0;

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
		bwprintf(COM2, "New position:%s\r\n", conductor->trackNodes[stat.updateTarget].name);
                return stat;
            }
        }
    }
    return stat;
}

CollisionData collisionDetectionV2(Conductor* conductor, int position, TRACKEVENT* events, int eventIndex, int timeDisplacement, int id){
    int dist = 0;
    int pruned[MAX_DIVERGENCE];
    int pruned_dist[MAX_DIVERGENCE];
    int size = pruneDistinctPathSensors(conductor->trackNodes, pruned, pruned_dist, 0, 0, position);
    CollisionData stat = {.impendingCollision = false, .updateStat = 0, .updateTarget = 0};

    bool graphMask[TRACK_MAX];
    generateGraphMask(graphMask, conductor, id);

    //consume events between this box
    //anything beyond will not be looked at until the next iteration

    int current = eventIndex + 1;

    int first = 0;

    while(dist < SAFE_DISTANCE){
        if(events[current].type == REVERSE){
        //we don't really, or shouldn't really look ahead
        //might be subject to change
            break;
        } else if(events[current].type == END){
        //what can you do?
	    if(first==0){
		stat.updateStat = 3;
	    }
	    break;
        } else if(events[current].type == SENSOR){
	    if(first++==0){
		//bwprintf(COM2, "Expecting %s\r\n", conductor->trackNodes[events[current].nodeId].name);
	    }
	    //else
		//bwprintf(COM2, "Also %s\r\n", conductor->trackNodes[events[current].nodeId].name);
	    
            if(!graphMask[events[current].nodeId]){
		//bwprintf(COM2, "%s %d blocked\r\n",conductor->trackNodes[events[current].nodeId].name, events[current].nodeId);
                //blocked
                if(first==1){
                    //activated trigger is within margin of error, likely just a figure of self
                    stat.updateStat = 1;
                    stat.updateTarget = current;
		    //bwprintf(COM2, "Hit@\r\n\r\n\r\n");
                } else {
                    //this is probably an impending collision
                    stat.impendingCollision = true;
                }
            }
        }
        if(current>=1)
            dist+=events[current - 1].distance;
	//hack job for hard coded values
        if(current++ == 144){
	    current--;
	    break;
	}
    }

    stat.frontierIndex = current;

    if(!stat.updateStat){
	for(int i=0; i<size; i++){
	    int node = pruned[i];
	    if(!graphMask[node]){
		//block
		//distance
		if(abs(pruned_dist[i]) < MARGIN_OF_ERROR){
		    //we assume that this means we might be here
		    stat.updateStat = 2;
		    stat.updateTarget = pruned[i];
		    //bwprintf(COM2, "New position:%s\r\n", conductor->trackNodes[stat.updateTarget].name);
		    break;
		}
	    }
	}
    }
    return stat;
}

void acquire_lock(Conductor* conductor, int order){

    //acquire lock
    conductor->wantToWrite[order] = 1;
    int inverse = order ? 0 : 1;
    while(conductor->wantToWrite[inverse]){
	bwprintf(COM2, "Lock acquire blocked, retrying...");
	if(conductor->lastUser == order){
	    conductor->wantToWrite[order] = 0;
	    while(conductor->lastUser==order)
		Yield();
	    conductor->wantToWrite[order] = 1;
	}
    }

}

int try_lock(Conductor* conductor, int order){
    
    conductor->wantToWrite[order] = 1;
    int inverse = order ? 0 : 1;
    while(conductor->wantToWrite[inverse]){
	if(conductor->lastUser == order){
	    conductor->wantToWrite[order] = 0;
	    return 1;
	}
    }

    return 0;
}

void release_lock(Conductor* conductor, int order){

    //release lock
    conductor->lastUser = order;
    conductor->wantToWrite[order] = 0;

}


//once we are certain of "in zone" collisions, we will worry about "outer collision" with transactional reservation
int reservation(Conductor* conductor, TRACKEVENT* events, int frontierIndex, int currentIndex, int id, int order){

    int status = try_lock(conductor, order);;

    if(status){
	return 1;
    }

    int* reservation = conductor->reserved;

    status = 0;
    
    int nid = 0;
    //it's techinically possible to create race conditions with memory sharing, so here is a dekker's alg for protection
     for(int i = frontierIndex; i > currentIndex; i--){
	int nid = events[i].nodeId;
	bwprintf(COM2, "%d %s\r\n By %d", nid, conductor->trackNodes[nid].name, id);
	if(!reservation[nid]){
	    reservation[nid] = id;
	    bwprintf(COM2, "%s reserved by %d\r\n", conductor->trackNodes[nid].name, id);
	}
	else if (reservation[nid] == id){
	    bwprintf(COM2, "Already owned by self %d\r\n", id);
	    break;
	} else {
	    bwprintf(COM2, "Can't reserve %s for %d\r\n", conductor->trackNodes[nid].name, id);
	    status = 1;
	    break;
	}

    }


    int i = 0;
    if(status){
	while(i++ < frontierIndex)
	nid = events[i].nodeId;
	reservation[nid] = 0;
    }

    release_lock(conductor, order);

    return status;

}

int freeReservation(Conductor* conductor, TRACKEVENT* events, int discardIndex, int currentIndex, int id, int order){
    acquire_lock(conductor, order);

    int* reservation = conductor->reserved;

    int status = 0;
    for(int i=discardIndex; i <= currentIndex; i++){
	int nid = events[i].nodeId;
	if(reservation[nid] == id){
	    reservation[nid] = 0;
	} else if(reservation[nid]!=0){
	    status = 1;
	}

    }

    release_lock(conductor, order);
    return status;
}



void dynamicPaths(TrainData* trainData){
    setSpeedConductor(trainData->conductor, trainData->id, 0);
    bwprintf(COM2, "Uart request succeeded %d\r\n", trainData->id);
    freeReservation(trainData->conductor, trainData->events, trainData->eventIndex, trainData->frontierIndex, trainData->id, trainData->order);
    bwprintf(COM2, "Reservation freed %d\r\n", trainData->id);
    int status = collisionFreePaths(trainData->conductor, trainData->position, trainData->destination, &trainData->path, trainData->events, TRACK_MAX, trainData->id);
    if(!status){
	TrainLogger(trainData, "New path!");
	char superBuffer[256];
	generatePath(trainData->conductor->trackNodes, &trainData->path, superBuffer, trainData->destination);
	if(trainData->order)
	bwprintf(COM2, "\337\33[8;0H%s\r\n\338", superBuffer);
	else
	bwprintf(COM2, "\337\33[7;0H%s\r\n\338", superBuffer);
        parsePath(trainData->conductor->trackNodes, &trainData->path, trainData->events, TRACK_MAX, trainData->destination);
        trainData->eventIndex = -1;
        trainData->timeDisplacement = Time(trainData->conductor->CLK);
        setSpeedConductor(trainData->conductor, trainData->id, 10);
        trainData->stalled = false;
	processIncomingEvents(trainData);
    } else {
	trainData->stalled = true;
	bwprintf(COM2, "Dead! %d\r\n", trainData->id);
    }
}

int processIncomingEvents(TrainData* T){
    int dist = 0;
    int sensor = 0;
    for(int i=T->eventIndex + 1;; i++){
        if(T->events[i].type == SENSOR)
            //only process up to the last sensor, everything else should be executed then
	    sensor = 1;
        else if(T->events[i].type == BRANCH){
            int swid = T->events[i].id;
            char direction = T->events[i].auxiliary == 0 ? 'S' : 'C';
            switchConductor(T->conductor, swid, direction);
        } else if(T->events[i].type == REVERSE){
	    if(!sensor){
		reverseConductor(T->conductor, T->id);
	    }
	} else if(T->events[i].type == END){
	    if(!sensor)
		return 1;
	    else
		break;
        }
	dist += T->events[i].distance;
	if(dist > SAFE_DISTANCE){
	    break;
	}
    }
    return 0;
}

void terminateTrain(TrainData* T, int caller){
    bwprintf(COM2, "Suicide alert\r\n");
    //termination
    setSpeedConductor(T->conductor, T->id, 0);
    //informs the controller of the heartbeat status
    int status = 0;
    Reply(caller, (const char*)&status, sizeof(status));
    Destroy();
}


void trainService(){
    TrainData T;
    T.stalled = true;
    T.frontierIndex = 0;
    T.eventIndex = -1;
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
    Receive(&caller, (char*)&T.order, sizeof(T.order));
    Reply(caller, NULL, 0);

    bwprintf(COM2, "%d, %d\r\n", T.order, T.id);

    int status;

    TrainLogger(&T, "Initializing");

    while(1){
        Receive(&caller, NULL, 0);
        bwprintf(COM2, "%d Received controller udpate\r\n", T.id);

	if(T.stalled){//exclusive branch, should not affected or be part of the loop to be followed
            dynamicPaths(&T);
	    Reply(caller, NULL, 0);
	    bwprintf(COM2, "Replied %d\r\n", T.id);
            continue;
        }

        CollisionData stat = collisionDetectionV2(T.conductor, T.position, T.events, T.eventIndex, T.timeDisplacement, T.id);
       
	bwprintf(COM2, "Block 1 \r\n");
 
        bool recomputePath = false;

        if(stat.updateStat == 1){
            //skips to node already reached
	    int baseIndex = T.eventIndex;
            T.eventIndex = stat.updateTarget;
            T.position = T.events[T.eventIndex].nodeId;
            T.timeDisplacement = Time(T.conductor->CLK);
            status = processIncomingEvents(&T);
            if(status){
		bwprintf(COM2, "Reaching end\r\n");
                terminateTrain(&T, caller);
            }
	    bwprintf(COM2, "Position: %d %s\r\n", T.id, T.conductor->trackNodes[T.position].name);
	    status = reservation(T.conductor, T.events, T.frontierIndex, T.eventIndex, T.id, T.order);
	    freeReservation(T.conductor, T.events, baseIndex, T.eventIndex - 1, T.id, T.order);
	    if(status){
		recomputePath = true;
	    }
        } else if(stat.updateStat == 2){
            T.position = stat.updateTarget;
            T.timeDisplacement = Time(T.conductor->CLK);
	    bwprintf(COM2, "Position: %d %s\r\n", T.id, T.conductor->trackNodes[T.position].name);
            recomputePath = true;
        } else if(stat.updateStat == 3){
	    bwprintf(COM2, "Last sensor within reach\r\n");
            terminateTrain(&T, caller);
        }

	bwprintf(COM2, "Block 2\r\n");
        if(stat.impendingCollision){
	    TrainLogger(&T, "Pausing");
            setSpeedConductor(T.conductor, T.id, 0);
            recomputePath = true;
        }
        if(recomputePath){
            dynamicPaths(&T);
        }
        Reply(caller, NULL, 0);
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

