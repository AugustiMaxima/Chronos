
#include <chlib.h>

void * memcpy (void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

void * memset ( void * ptr, int value, size_t num ){
    unsigned char value_downcast = value;
    value = value_downcast * 0x01010101;
    char* start = ptr;
    char* end = start + num;
    for(;(int)start%4 && start<end;start++){
        *start = value_downcast;
    }
    int* block = (int*)start;
    for(;block + 1 <= (int*)end;block++){
        *block = value;
    }
    start = (char*)block;
    for(;start<end;start++){
        *start = value_downcast;
    }
    return ptr;
}
