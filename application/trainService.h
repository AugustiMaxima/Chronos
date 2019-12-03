#ifndef TRAIN_SERVICE 
#define TRAIN_SERVICE

#include <stdbool.h>
#include <conductor.h>
#include <pathFinder.h>

typedef struct metaCollsion{
    bool impendingCollision;
    int updateStat;
    int updateTarget;
    int frontierIndex;
} CollisionData;


typedef struct metaTrain{
    Conductor* conductor;
    int id;
    int order;
    int position;
    int eventIndex;
    int frontierIndex;
    int timeDisplacement;
    int destination;
    bool stalled;
    PATH path;
    TRACKEVENT events[TRACK_MAX];
} TrainData;

int createTrainService(Conductor* conductor, int train, int position, int destination);

#endif
