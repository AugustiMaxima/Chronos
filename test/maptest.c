#include <map.h>
#include <stdlib.h>
#include <bwio.h>

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

void MapTestPut(){
    Map map;
    Map* m = &map;
    initializeMap(m);
    putMap(m, 1, NULL);
    putMap(m, 2, NULL);
    putMap(m, 3, NULL);
    putMap(m, 4, NULL);
    putMap(m, 5, NULL);
    putMap(m, 6, NULL);
    putMap(m, -1, NULL);
    putMap(m, 2, NULL);
    putMap(m, 1, 2);
    putMap(m, 2, NULL);
    putMap(m, 2, NULL);
    bwputr(COM2, getMap(m, 1));

}