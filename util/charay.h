#ifndef CHARAY_H
#define CHARAY_H
/*
Some basic char array manipulation tools because I am tired and sick of writing the same shitty code
*/

//Writes a string representation of numbers with the configured base, supports hex
//buffer length must be power+1 or greater or you will get memory corruption
int noneZeroIntString(char* writeBuffer, int buflen, int num, int base);

int stringToNum(char* string, int base);

int strlcmp(char* str1, char* str2);

int alphaNumericHash(char* name);

void split(char** tokens, int length, char* base, char divider);

int strlcpy(char* source, char* dest, int length);

void formatStrn(char* result, int length, char* format, ...);

#endif
