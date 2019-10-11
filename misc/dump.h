#include <sendReceiveReply.h>

void printSp();
void printCurrentMode();

/*
Sets the `I` bit of the current CPSR. Useful for enabling interrupts without
switching mode.
*/
void enableCpsrI();
void warnAtEndOfKernel(COMM* com);