#include <chlib.h>
#include <pathFinder.h>

void computePath(track_node* tracks, PATH* path, int source, int dest){
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
            if(tracks[alpha].type == NODE_EXIT){
              continue;
            }
            int remote = tracks[alpha].edge[0].dest - tracks;
            if(path->cost[remote] == -1){
                if(opt_w > tracks[alpha].edge[0].dist || opt_w == -1){
                    opt_w = tracks[alpha].edge[0].dist;
                    opt_s = alpha;
                    opt_e = 0;
                }
            }

            if(tracks[alpha].type == NODE_BRANCH){
                int r2 = tracks[alpha].edge[1].dest - tracks;
                if(path->cost[r2] == -1){
                    if(opt_w > tracks[alpha].edge[1].dist || opt_w == -1){
                        opt_w = tracks[alpha].edge[1].dist;
                        opt_s = alpha;
                        opt_e = 1;
                    }
                }
            }
        }

        if(opt_w == -1){
            break;
        }

        int newMember = tracks[opt_s].edge[opt_e].dest - tracks;
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
        trackevents[i].id = -1;
    }

    trackevents[eventBufferSize - 1].type = END;
    trackevents[eventBufferSize - 1].id = dest;

    current = path->intermediate[current];

    //when done, move by index + 1
    while(current!=source && index>=0){
        if(tracks[current].type == NODE_BRANCH){
            trackevents[index].type = BRANCH;
            trackevents[index].id = tracks[current].num;
            if(tracks[current].edge[0].dest == tracks + prev){
                trackevents[index].auxiliary = 0;
            } else if(tracks[current].edge[1].dest == tracks + prev){
                trackevents[index].auxiliary = 1;
            }
            index--;
        } else if(tracks[current].type == NODE_SENSOR){
            if(trackevents[index + 1].type != SENSOR){
                trackevents[index].type = SENSOR;
                trackevents[index].id = tracks[current].num;
                index--;
            }
        }
        prev = current;
        current = path->intermediate[current];
    }

    for(int i=index + 1, j=0; i<eventBufferSize; i++, j++){
        trackevents[j].auxiliary = trackevents[i].auxiliary;
        trackevents[j].id = trackevents[i].id;
        trackevents[j].type = trackevents[i].type;
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

