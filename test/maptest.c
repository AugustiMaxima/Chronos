#include <map.h>
#include <stdlib.h>

void MapTest(){
    Map map;
    Map* m = &map;
    initializeMap(m);
    insertMap(m, 1, NULL);
    printTree(m);
    insertMap(m, 2, NULL);
    printTree(m);
    insertMap(m, 3, NULL);
    printTree(m);
    insertMap(m, 4, NULL);
    printTree(m);
    insertMap(m, 5, NULL);
    printTree(m);
    insertMap(m, 6, NULL);
    printTree(m);
    insertMap(m, 7, NULL);
    printTree(m);
    insertMap(m, 8, NULL);
    printTree(m);
    insertMap(m, 9, NULL);
    printTree(m);
    removeMap(m, 23);
    printTree(m);
    removeMap(m, 25);
    insertMap(m, 35, NULL);
    insertMap(m, 6, NULL);
    insertMap(m, 2, NULL);
    insertMap(m, 5, NULL);
    insertMap(m, 43, NULL);
    insertMap(m, 62, NULL);
    removeMap(m, 63);
    removeMap(m, 35);
    printTree(m);
    insertMap(m, 15, NULL);
}