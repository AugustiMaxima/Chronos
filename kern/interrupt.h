#ifndef INTERRUPT_H
#define INTERRUPT_H



//Refer to the section "6.2: Registers" in the EP93xx user guide page 170
#define VIC1ADDR 0x800B0000
#define VIC2ADDR 0x800C0000
#define VIC_RAW_INTR 0x08 // VICxRawIntr
#define VIC_ENABLE 0x10 // VICxIntEnable
#define VIC_ENABLE_CLEAR 0x14 // VICxIntEnClear

#define TC1UI_DEV_ID 4
#define UART1RX_DEV_ID 23
#define UART1TX_DEV_ID 24
#define UART2RX_DEV_ID 25
#define UART2TX_DEV_ID 26

void interruptHandler();

void installInterruptHandler(void* handler);

void setEnabledDevices(unsigned deviceList1, unsigned deviceList2);

void enableDevice(unsigned deviceList1, unsigned deviceList2);

void disableDevice(unsigned deviceList1, unsigned deviceList2);

#endif
