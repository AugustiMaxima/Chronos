#ifndef CHARAY_H
#define CHARAY_H
/*
Some basic char array manipulation tools because I am tired and sick of writing the same shitty code
*/

//Writes a string representation of numbers with the configured base, supports hex
//buffer length must be power+1 or greater or you will get memory corruption
int noneZeroIntString(char* writeBuffer, int buflen, int num, int base);

int stringToNum(char* string, int base);

int strcmp(const char* s1, const char* s2);

int alphaNumericHash(char* name);

void split(char** tokens, int length, char* base, char divider);

int strncpy(const char* source, char* dest, int length);

int strlen(const char* str);

int formatStrn(char* result, int length, char* format, ...);

int chos_strlen (const char *s);

char* tokenizeString(const char* source, char delimiter, int length);

#endif
