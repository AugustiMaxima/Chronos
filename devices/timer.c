#include <timer.h>
#include <ts7200.h>

int getTimerBase(int timer){
    switch (timer){
        case 1: return TIMER1_BASE;
        case 2: return TIMER2_BASE;
        case 3: return TIMER3_BASE;
    }
    return TIMER3_BASE;
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

unsigned int getValue(int timer) {
    void* BASE = getTimerBase(timer);
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

void initializeTimer(int timer, int frequency, unsigned int length, int mode){
    void* BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ~ENABLE_MASK;
    unsigned int sanitizedLength = sanitizeLength(timer, length);
    *(int*)(BASE + LDR_OFFSET) = sanitizedLength;
    setFrequency(timer, frequency);
    setMode(timer, mode);
    *(int*)(BASE + CRTL_OFFSET) |= ENABLE_MASK;
}
