#ifndef MAP_H
#define MAP_H

#include <queue.h>

#define MAP_SIZE 128

typedef struct genericNode{
    int key;
    void* value;
    int height;
    struct genericNode* left;
    struct genericNode* right;
    struct genericNode*	up;
} Node;

typedef struct genericMap{
    Node* root;
    Node nodes[MAP_SIZE];
    Queue freeQueue;
    void* retainer;
} Map;

void initializeMap(Map* map);

// returns -1 if not enough space
// no one reads the return value
int insertMap(Map* map, int key, void* value);

int putMap(Map* map, int key, void* value);

void* getMap(Map* map, int key);

void* removeMap(Map* map, int key);

Node* iterateMap(Map* map, Node* key);

void printTree(Map* map);

#endif
