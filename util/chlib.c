#include <chlib.h>
#include <bwio.h>

void * memcpy (void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

// Note: This does work everytime I've tested it, but seems to not play well with the compiler
void * memset ( void * ptr, int value, size_t num ){
    unsigned char value_downcast = value;
    value = value_downcast * 0x01010101;
    char* start = ptr;
    char* end = start + num;
    for(;(int)start%sizeof(value) && start<end;start++){
        *start = value_downcast;
    }
    int* block = (int*)start;
    for(;(char*)(block + 1) <= end;block++){
        *block = value;
    }
    start = (char*)block;
    for(;start<end;start++){
	//bwprintf(COM2, "%x\r\n", start);
        *start = value_downcast;
    }
    return ptr;
}
// Keeping it if I can figure out how to properly fix it
