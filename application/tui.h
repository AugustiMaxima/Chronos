#ifndef TUI_H
#define TUI_H

//Used to avoid rerendering or rechecking components that have not been updated
typedef struct tui_Props{
    bool sensorUpdate;
    bool timeUpdate;
    bool switchUpdate;
} TUIRenderState;


void tuiThread();

int createTUI(int RX, int TX, int CLK, Conductor* conductor, TUIRenderState* prop);

void drawInput();

#endif