#include <uiTest.h>
#include <terminal.h>
#include <bwio.h>

void cursorTest(){
    TerminalOutput output;
    flush(&output);
    clear(&output);
    jumpCursor(&output, 12, 0);
    attachMessage(&output, "b");
    saveCursor(&output);
    jumpCursor(&output, 0, 0);
    attachMessage(&output, "c");
    restoreCursor(&output);
    attachMessage(&output, "a");
    bwprintf(COM2, "%s", output.compositePayload);
}
