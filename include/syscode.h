#ifndef SYSCODE_H
#define SYSCODE_H

// https://stackoverflow.com/a/240370
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define YIELD_CODE 0
#define CREATE_CODE 1
#define MYTID_CODE 2
#define MYPARENTTID_CODE 3
#define EXIT_CODE 4

#endif