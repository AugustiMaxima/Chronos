#ifndef PATH_FINDER
#define PATH_FINDER

#include <track_data.h>

typedef struct pathSet{
    int source;
    int cost[TRACK_MAX];
    int intermediate[TRACK_MAX];
} PATH;

//all event processing is based on the most adjacent sensor
typedef struct trackEvents{
    int sensorID;
    enum trackEventType{
        REVERSE,
        BRANCH,
        END
    } type;
    int auxiliary;
} TRACKEVENT;


void computePath(track_node* tracks, PATH* path, int source, int dest);

void parsePath(track_node* tracks, PATH* path, TRACKEVENT* trackevents, int eventBufferSize, int dest);

void generatePath(track_node* tracks, PATH* path, char* buffer, int dest);

#endif