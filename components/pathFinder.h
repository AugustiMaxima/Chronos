#ifndef PATH_FINDER
#define PATH_FINDER

#include <stdbool.h>
#include <track_data.h>

typedef struct pathSet{
    int source;
    int cost[TRACK_MAX];
    int intermediate[TRACK_MAX];
} PATH;

//all event processing is based on the most adjacent sensor
typedef struct trackEvents{
    enum trackEventType{
        SENSOR,
        REVERSE,
        BRANCH,
        END
    } type;
    int id;
    int nodeId;
    int auxiliary;
    int distance;
} TRACKEVENT;


void computePath(track_node* tracks, PATH* path, bool* graphMask, int source, int dest);

void parsePath(track_node* tracks, PATH* path, TRACKEVENT* trackevents, int eventBufferSize, int dest);

void generatePath(track_node* tracks, PATH* path, char* buffer, int dest);

#endif