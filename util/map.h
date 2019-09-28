#ifndef MAP_H
#define MAP_H

#include <queue.h>

#define MAP_SIZE 100

typedef struct genericNode{
    int key;
    void* value;
    int height;
    struct genericNode* left;
    struct genericNode* right;
} Node;

typedef struct genericMap{
    Node* root;
    Node nodes[MAP_SIZE];
    Queue freeQueue;
} Map;

void initializeMap(Map* map);

//returns -1 if not enough space
int insertMap(Map* map, int key, void* value);

void* getMap(Map* map, int key);

void removeMap(Map* map, int key);

void printTree(Map* map);

#endif