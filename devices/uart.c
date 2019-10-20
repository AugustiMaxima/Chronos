#include <ts7200.h>
#include <uart.h>

#include <bwio.h>

int getUARTBase(int channel) {
    switch (channel) {
        case 1:
            return UART1_BASE;
        case 2:
            return UART2_BASE;
        default:
            return 0;
    }
}

void setFifo(int channel, bool fifo){
    int* CTL = (int*)(getUARTBase(channel) + UART_LCRH_OFFSET);
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

void setTransmitInterrupt(int channel, bool enable){
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
    setTransmitInterrupt(channel, TIE);
    setReceiveInterrupt(channel, RIE);
    setModemStatusInterrupt(channel, MSIE);
    enableUart(channel, true);
}

int putUart(int channel, char byte) {
    int BASE = getUARTBase(channel);
    unsigned flag = getUartFlag(channel);
    if (!flag & CTS_MASK){
        //inverts the CTS signal for easier error processing
        flag |= CTS_MASK;
        return flag;
    } else {
        flag &= ~CTS_MASK;
    }
    if(flag & TXFF_MASK){
        return flag;
    }
    *(int*)(BASE + UART_DATA_OFFSET) = byte;
    return 0;
}

int getUart(int channel, char* byte) {
    int BASE = getUARTBase(channel);
    unsigned flag = getUartFlag(channel);
    if (flag & RXFE_MASK) {
        return flag;
    }
    *byte = *(char*)(BASE + UART_DATA_OFFSET);
    return 0;
}

unsigned processUartInterrupt(int channel){
    unsigned flag = *(volatile unsigned*)(getUARTBase(channel) + UART_INTR_OFFSET);

    bwprintf(COM2, "Trying (and failing) to process the general UART interrupt with flag status %x\r\n", flag);
    // refer to the docs on UART1IntIDIntClr for how this is done
    if(flag & 0x1){
        //MIS cleared by writing to the register
        bwprintf(COM2, "Triggered MIS\r\n");
        *(volatile unsigned*)(getUARTBase(channel) + UART_INTR_OFFSET) &= ~0x1;
    }
    if(flag & 0x2){
        bwprintf(COM2, "Triggered RX\r\n");
        setReceiveInterrupt(channel, false);
    }
    if(flag & 0x4){
        bwprintf(COM2, "Triggered TX\r\n");
        setTransmitInterrupt(channel, false);
    }
    if(flag & 0x8){
        bwprintf(COM2, "Triggered timeout\r\n");
        setReceiveTimeout(channel, false);
    }
    return flag;
}

unsigned getUartFlag(int channel){
    return *(volatile unsigned*)(getUARTBase(channel) + UART_FLAG_OFFSET);
}
