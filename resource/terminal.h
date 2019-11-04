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

typedef struct stringFormatter{
    char* content;
    int length;
    int size;
} StringFormatter;

void initializeString(StringFormatter* payload, char* buffer, int size);
void flush(StringFormatter* payload);
void clear(StringFormatter* payload);
void setColor(StringFormatter* payload, int color);
void jumpCursor(StringFormatter* payload, int r, int c);
void hideCursor(StringFormatter* payload);
void deleteLine(StringFormatter* payload);
void saveCursor(StringFormatter* payload);
void restoreCursor(StringFormatter* payload);
void backSpace(StringFormatter* payload, int num);
void setWindowBoundary(StringFormatter* payload, int top, int bottom);
void attachMessage(StringFormatter* payload, const char* message);

#endif
