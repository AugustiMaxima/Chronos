#include <timer.h>
#include <ts7200.h>
#include <bwio.h>

int getTimerBase(int timer){
    switch (timer){
        case 1: return TIMER1_BASE;
        case 2: return TIMER2_BASE;
        case 3: return TIMER3_BASE;
    }
    return TIMER3_BASE;
}

void setMode(int timer, int mode) {
    int* CRTL = (int*)(getTimerBase(timer) + CRTL_OFFSET);
    switch (mode){
    case 1:
        *CRTL |= MODE_MASK;
        break;
    default:
        *CRTL &= ~MODE_MASK;
    }
}

void setFrequency(int timer, int frequency) {
    int* CRTL = (int*)(getTimerBase(timer) + CRTL_OFFSET);
    switch (frequency){
    case 508000:
        *CRTL |= CLKSEL_MASK;
        break;
    default:
        *CRTL &= ~CLKSEL_MASK;
    }
}

unsigned int readTimerValue(int timer) {
    unsigned int value = *(unsigned int*)(getTimerBase(timer) + VAL_OFFSET);
    return value;
}

void clearTimerInterrupt(int timer) {
    *(unsigned volatile int*)(getTimerBase(timer) + CLR_OFFSET) = 0;
}

unsigned int getWrap(int timer) {
    switch (timer){
    case 3:
        return 0xFFFFFFFF;
    default:
        return 0xFFFF;
    }
}

int sanitizeLength(int timer, int length){
    if(timer!=3)
	length &= 0xFFFF;
    return length;
}

// length = initial value
// mode: 1 for pre-load (wrap-to-loaded), 0 for free-running (wrap-to-maximum)
void initializeTimer(int timer, int frequency, unsigned int length, int mode){
    int BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ~ENABLE_MASK;
    unsigned int sanitizedLength = sanitizeLength(timer, length);
    *(int*)(BASE + LDR_OFFSET) = sanitizedLength;
    setFrequency(timer, frequency);
    setMode(timer, mode);
    *(int*)(BASE + CRTL_OFFSET) |= ENABLE_MASK;
}

void disableTimer(int timer) {
    *(int*)(getTimerBase(timer) + CRTL_OFFSET) &= ENABLE_MASK;
}
