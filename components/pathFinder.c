#include <chlib.h>
#include <pathFinder.h>

#define REVERSE_COST 10

void computePath(track_node* tracks, PATH* path, bool* graphMask, int source, int dest){
    int inSet[TRACK_MAX];
    for(int i=0; i<TRACK_MAX; i++){
        path->cost[i] = -1;
        inSet[i] = -1;
    }
    path->source = source;
    path->cost[source] = 0;
    inSet[0] = source;

    int reachableSetSize = 1;

    while(path->cost[dest] == -1){
        int opt_s = -1;
        int opt_w = -1;
        int opt_i = -1;
        int scope = reachableSetSize;

        for(int i=0; i<scope; i++){
            int alpha = inSet[i];

            int r0 = tracks[alpha].reverse - tracks;
            
            if(path->cost[r0] == -1 && graphMask[r0]){
                if(opt_w > REVERSE_COST || opt_w == -1){
                    opt_w = REVERSE_COST;
                    opt_s = alpha;
                    opt_i = r0;
                }
            }
            
            if(tracks[alpha].type == NODE_EXIT){
              continue;
            }
            
            int r1 = tracks[alpha].edge[0].dest - tracks;
            if(path->cost[r1] == -1 && graphMask[r1]){
                if(opt_w > tracks[alpha].edge[0].dist || opt_w == -1){
                    opt_w = tracks[alpha].edge[0].dist;
                    opt_s = alpha;
                    opt_i = r1;
                }
            }

            if(tracks[alpha].type == NODE_BRANCH){
                int r2 = tracks[alpha].edge[1].dest - tracks;
                if(path->cost[r2] == -1 && graphMask[r2]){
                    if(opt_w > tracks[alpha].edge[1].dist || opt_w == -1){
                        opt_w = tracks[alpha].edge[1].dist;
                        opt_s = alpha;
                        opt_i = r2;
                    }
                }
            }

        }

        if(opt_w == -1){
            break;
        }

        path->cost[opt_i] = path->cost[opt_s] + opt_w;
        path->intermediate[opt_i] = opt_s;        
        inSet[reachableSetSize++] = opt_i;
    }
}


void parsePath(track_node* tracks, PATH* path, TRACKEVENT* trackEvents, int eventBufferSize, int dest){
    int source = path->source;
    int current = dest;
    int prev = dest;
    int index = eventBufferSize - 2;

    for(int i=0; i<eventBufferSize; i++){
        trackEvents[i].id = -1;
    }

    trackEvents[eventBufferSize - 1].type = END;
    trackEvents[eventBufferSize - 1].id = dest;
    trackEvents[eventBufferSize - 1].distance = 0;

    current = path->intermediate[current];

    //when done, move by index + 1
    while(current!=source && index>=0){
        bool newEvent = false;
        if(tracks[current].reverse == tracks + prev){
            //indication that a reverse event has occured
            trackEvents[index].type = REVERSE;
            trackEvents[index].id = tracks[current].num;
            newEvent = true;
        }
        if(tracks[current].type == NODE_BRANCH){
            trackEvents[index].type = BRANCH;
            trackEvents[index].id = tracks[current].num;
            if(tracks[current].edge[0].dest == tracks + prev){
                trackEvents[index].auxiliary = 0;
            } else if(tracks[current].edge[1].dest == tracks + prev){
                trackEvents[index].auxiliary = 1;
            }
            newEvent = true;
        } else if(tracks[current].type == NODE_SENSOR){
            trackEvents[index].type = SENSOR;
            trackEvents[index].id = tracks[current].num;
            newEvent = true;
        }

        if(newEvent){
            trackEvents[index].distance = path->cost[trackEvents[index+1].nodeId] - path->cost[current];
            trackEvents[index].nodeId = current;
            index--;
        }

        prev = current;
        current = path->intermediate[current];
    }

    for(int i=index + 1, j=0; i<eventBufferSize; i++, j++){
        trackEvents[j].auxiliary = trackEvents[i].auxiliary;
        trackEvents[j].id = trackEvents[i].id;
        trackEvents[j].type = trackEvents[i].type;
    }

}


void generatePath(track_node* tracks, PATH* path, char* buffer, int dest){
    int current = dest;
    int source = path->source;
    int index = 0;
    while(current!=source){
        const char* name = tracks[current].name;
        for(int i=0; name[i]; i++){
            buffer[index++] = name[i];
        }
        buffer[index++] = '<';
        buffer[index++] = '-';
        current = path->intermediate[current];
    }
    buffer[index] = 0;
}

