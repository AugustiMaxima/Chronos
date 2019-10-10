#include <ts7200.h>
#include <uart.h>

int getUARTBase(int channel) {
    switch (channel) {
	case 1:
	    return UART1_BASE;
	case 2:
	    return UART2_BASE;
    }
}

void setFifo(int channel, bool fifo){
    int* CTL = getUARTBase(channel) + UART_LCRH_OFFSET;
    if (fifo){
	*CTL |= FEN_MASK;
    } else {
	*CTL &= ~FEN_MASK;
    }
}

void setBitRate(int channel, int bitRate) {
    int BASE = getUARTBase(channel);
    int bauDiv = UARTCLK/(16*bitRate) - 1;
    int lowerDiv = bauDiv & 0xff;
    int upperDiv = bauDiv & ~0xff;
    *(int*)(BASE + UART_LCRM_OFFSET) = upperDiv;
    *(int*)(BASE + UART_LCRL_OFFSET) = lowerDiv;   
}

void setReceiveTimeout(int channel, bool enable){
    int BASE = getUARTBase(channel);
    if(enable){
        *(volatile int *)(BASE + UART_CTLR_OFFSET) |= RTIEN_MASK;
    } else {
        *(volatile int *)(BASE + UART_CTLR_OFFSET) &= ~RTIEN_MASK;
    }
}

void setTransmittInterrupt(int channel, bool enable){
    int BASE = getUARTBase(channel);
    if(enable){
        *(volatile int *)(BASE + UART_CTLR_OFFSET) |= TIEN_MASK;
    } else {
        *(volatile int *)(BASE + UART_CTLR_OFFSET) &= ~TIEN_MASK;
    }
}

void setReceiveInterrupt(int channel, bool enable){
    int BASE = getUARTBase(channel);
    if(enable){
        *(volatile int *)(BASE + UART_CTLR_OFFSET) |= RIEN_MASK;
    } else {
        *(volatile int *)(BASE + UART_CTLR_OFFSET) &= ~RIEN_MASK;
    }
}

void setModemStatusInterrupt(int channel, bool enable){
    int BASE = getUARTBase(channel);
    if(enable){
        *(volatile int *)(BASE + UART_CTLR_OFFSET) |= MSIEN_MASK;
    } else {
        *(volatile int *)(BASE + UART_CTLR_OFFSET) &= ~MSIEN_MASK;
    }
}

void enableUart(int channel, bool enable){
    int BASE = getUARTBase(channel);
    if(enable){
        *(volatile int *)(BASE + UART_CTLR_OFFSET) |= 0x1;
    } else {
        *(volatile int *)(BASE + UART_CTLR_OFFSET) &= ~0x1;
    }
}

void initializeUART(int channel, int bitRate, bool fifo, bool RTIE, bool TIE, bool RIE, bool MSIE){
    setBitRate(channel, bitRate);
    setFifo(channel, fifo);
    setReceiveTimeout(channel, RTIE);
    setTransmittInterrupt(channel, TIE);
    setReceiveInterrupt(channel, RIE);
    setModemStatusInterrupt(channel, MSIE);
    enableUart(channel, true);
}

int put(int channel, char byte) {
    int BASE = getUARTBase(channel);
    if (*(int*)(BASE + UART_FLAG_OFFSET) & TXFE_MASK){
        *(int*)(BASE + UART_DATA_OFFSET) = byte;
        return 0;
    }
    return 1;
}

int get(int channel, char* byte) {
    int BASE = getUARTBase(channel);
    if (*(int*)(BASE + UART_FLAG_OFFSET) & RXFE_MASK) {
        return 1;
    }
	byte = *(char*)(BASE + UART_DATA_OFFSET);
    return 0;
}