#include "math.h"

int max(int a, int b){
    return a>b? a: b;
}


int abs(int i){
    return i>0? i: -i;
}


int expo(int i, int e){
    if(e==1){
        return i;
    }
    int ep = e/2;
    int r = e%2;

    int sp = expo(i, ep);
    int result = sp*sp;
    if(r)
        result*=e;
    return result;
}

