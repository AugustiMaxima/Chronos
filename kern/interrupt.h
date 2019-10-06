#ifndef INTERRUPT_H
#define INTERRUPT_H



//Refer to the EP93xx user guide for more details
//We use the vic 2 primarily for clock and uart
#define VIC1ADDR 0x800B0000
#define VIC2ADDR 0x800C0000
#define VIC_RAW_INTR 0x08
#define VIC_ENABLE 0x10
#define VIC_ENABLE_CLEAR 0x14
#define VIC_ADDR_REG 0x30


void interruptHandler();

void installInterruptHandler(void* handler);

void enableDevice(unsigned deviceList1, unsigned deviceList2);

#endif
