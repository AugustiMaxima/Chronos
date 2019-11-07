#include <pathFinder.h>


void computePATH(track_node* tracks, PATH* path, int source, int dest){
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
        int opt_e = -1;
        int opt_s = -1;
        int opt_w = -1;
        int scope = reachableSetSize;

        for(int i=0; i<scope; i++){
            int alpha = inSet[i];
            int remote = (tracks[alpha].edge[0].dest - tracks) / sizeof(track_node);
            if(path->cost[remote] == -1){
                if(opt_w > tracks[alpha].edge[0].dist || opt_w == -1){
                    opt_w = tracks[alpha].edge[0].dist;
                    opt_s = alpha;
                    opt_e = 0;
                }
            }

            if(tracks[alpha].type == NODE_BRANCH){
                int r2 = (tracks[alpha].edge[1].dest - tracks) / sizeof(track_node);
                if(path->cost[r2] == -1){
                    if(opt_w > tracks[alpha].edge[0].dist || opt_w == -1){
                        opt_w = tracks[alpha].edge[0].dist;
                        opt_s = alpha;
                        opt_e = 0;
                    }
                }
            }
        }

        if(opt_w == -1){
            break;
        }

        int newMember = (tracks[opt_s].edge[opt_e].dest - tracks) / sizeof(track_node);
        path->cost[newMember] = path->cost[opt_s] + tracks[opt_s].edge[opt_e].dist;
        path->intermediate[newMember] = opt_s;        
        inSet[reachableSetSize++] = newMember;
    }    
}


void parsePath(track_node* tracks, PATH* path, TRACKEVENT* trackevents, int eventBufferSize, int dest){
    int source = path->source;
    int current = dest;
    int prev = dest;
    int index = eventBufferSize - 2;

    for(int i=0; i<eventBufferSize; i++){
        trackevents[i].sensorID = -1;
    }

    trackevents[eventBufferSize - 1].type = END;

    //when done, move by index + 1
    while(current!=source && index>=0){
        if(tracks[current].type == NODE_BRANCH){
            trackevents[index].type = BRANCH;
            if(tracks[current].edge[0].dest == tracks + prev){
                trackevents[index].auxiliary = 0;
            } else if(tracks[current].edge[1].dest == tracks + prev){
                trackevents[index].auxiliary = 1;
            }
        } else if(tracks[current].type == NODE_SENSOR){
            if(trackevents[index].sensorID == -1)
                trackevents[index--].sensorID = tracks[current].num;
        }
        prev = current;
        current = path->intermediate[current];
    }

    for(int i=index + 1, j=0; i<eventBufferSize; i++, j++){
        trackevents[j].auxiliary = trackevents[i].auxiliary;
        trackevents[j].sensorID = trackevents[i].sensorID;
        trackevents[j].type = trackevents[i].type;
    }

}


void generatePath(track_node* tracks, PATH* path, char* buffer, int dest){
    int current = dest;
    int source = path->source;
    int index = 0;
    while(current!=source){
        char* name = tracks[current].name;
        for(int i=0; name[i]; i++){
            buffer[index++] = name[i];
        }
        buffer[index++] = '<';
        buffer[index++] = '-';
    }
    buffer[index] = 0;
}

