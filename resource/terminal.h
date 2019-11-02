#ifndef TERMINAL_H
#define TERMINAL_H

//Instructions
#define CLEAR "\033[2J"
#define MOVE "\033[H"
#define MOVETO "\033[r;cH"
#define MOVETO_PREF "\033["
#define MOVETO_SEPARATOR ";"
#define MOVETO_SUFF "H"
#define HIDE "\033[?25l"
#define DELETE "\033[K"
#define SAVECURSOR "\0337"
#define RESTORECURSOR "\0338"

//Colors
#define RESET "\033[0m"	
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGNETA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define COLOR_RESET 0
#define COLOR_BLACK 1
#define COLOR_RED 2
#define COLOR_GREEN 3
#define COLOR_YELLOW 4
#define COLOR_BLUE 5
#define COLOR_MAGNETA 6
#define COLOR_CYAN 7
#define COLOR_WHITE 8

#define PAYLOAD_SIZE 64

typedef struct terminal_Formatter{
    char compositePayload[PAYLOAD_SIZE];
    int length;
} TerminalOutput;

void flush(TerminalOutput* payload);
void clear(TerminalOutput* payload);
void setColor(TerminalOutput* payload, int color);
void jumpCursor(TerminalOutput* payload, int r, int c);
void hideCursor(TerminalOutput* payload);
void deleteLine(TerminalOutput* payload);
void saveCursor(TerminalOutput* payload);
void restoreCursor(TerminalOutput* payload);
void backSpace(TerminalOutput* payload, int num);
void setWindowBoundary(TerminalOutput* payload, int top, int bottom);
void attachMessage(TerminalOutput* payload, char* message);

#endif
