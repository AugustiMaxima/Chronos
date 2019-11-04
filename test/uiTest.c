#include <uiTest.h>
#include <terminal.h>
#include <bwio.h>

void cursorTest(){
    char buffer[64];
    StringFormatter output;
    initializeString(&output, buffer, 64);
    output.content = buffer;
    output.size = 64; 
    flush(&output);
    clear(&output);
    jumpCursor(&output, 12, 0);
    attachMessage(&output, "b");
    saveCursor(&output);
    jumpCursor(&output, 0, 0);
    attachMessage(&output, "c");
    restoreCursor(&output);
    attachMessage(&output, "a");
    bwprintf(COM2, "%s", output.content);
}
