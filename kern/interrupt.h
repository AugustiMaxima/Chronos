#ifndef INTERRUPT_H
#define INTERRUPT_H



//Refer to the EP93xx user guide for more details
//We use the vic 2 primarily for clock and uart
#define VIC1ADDR 0x800B0030 

#define VIC2ADDR 0x800C0030


void interruptHandler();

void installInterruptHandler(void* handler);

#endif
