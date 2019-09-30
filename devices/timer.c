#include <ts7200.h>
#include <bwio.h>
#include <timer.h>

#define WRAP_16 0xffff;
#define WRAP_32 0xffffffff;
#define LOWER_MASK 0x0000ffff;

int getTimerBase(int timer){
	switch (timer){
	    case 1:
		return TIMER1_BASE;
	    case 2:
		return TIMER2_BASE;
	    case 3:
		return TIMER3_BASE;
	}
    return TIMER3_BASE;
}

unsigned int sanitizeLength(int timer, unsigned int length){
	if (timer!=3){
	    length &= LOWER_MASK;
	}
	return length;
}

void setFrequency(int timer, int frequency) {
    int* CRTL = getTimerBase(timer) + CRTL_OFFSET;
    switch (frequency){
    case 508000:
	*CRTL |= CLKSEL_MASK;
	break;
    default:
	*CRTL &= ~CLKSEL_MASK;
    }
}

void setMode(int timer, int mode) {
    int* CRTL = getTimerBase(timer) + CRTL_OFFSET;
    switch (mode){
    case 1: 
	*CRTL |= MODE_MASK;
	break;
    default:
	*CRTL &= ~MODE_MASK;
    }
}


void initializeTimer(int timer, int frequency, unsigned int length, int mode){
    int BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ~ENABLE_MASK;
    unsigned int sanitizedLength = sanitizeLength(timer, length);
    *(int*)(BASE + LDR_OFFSET) = sanitizedLength;
    setFrequency(timer, frequency);
    setMode(timer, mode);
    *(int*)(BASE + CRTL_OFFSET) |= ENABLE_MASK;
}

unsigned int getValue(int timer) {
    int BASE = getTimerBase(timer);
    unsigned int value = *(int*)(BASE + VAL_OFFSET);
    if(timer!=3)
	    value &= LOWER_MASK;
    return value;
}

unsigned int getWrap(int timer) {
    switch (timer){
    case 3:
	return WRAP_32;
    default:
	return WRAP_16;
    }
}
