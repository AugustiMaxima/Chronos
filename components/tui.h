#ifndef TUI_H
#define TUI_H

#include <stdbool.h>

typedef struct conductor Conductor;

//Used to avoid rerendering or rechecking components that have not been updated
typedef struct tui_Props{
    volatile bool sensorUpdate;
    volatile bool timeUpdate;
    volatile bool switchUpdate;
    struct msg{
        volatile bool read;
        volatile int length;
        char message[40];
        volatile enum {
            INFO,
            NOTIFICATION,
            WARNING,
            ERROR
        } style;
    } message;
} TUIRenderState;

void tuiThread();

int createTUI(int RX, int TX, int CLK, Conductor* conductor, TUIRenderState* prop);

#endif
