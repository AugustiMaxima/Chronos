#include <charay.h>
#include <bwio.h>
#include <terminal.h>

void initializeString(StringFormatter* payload, char* buffer, int size){
    payload->size = size;
    payload->content = buffer;
    flush(payload);
}

void flush(StringFormatter* payload){
    payload->content[0] = 0;
    payload->length = 0;
}

void clear(StringFormatter* payload){
    attachMessage(payload, CLEAR);
}

void setColor(StringFormatter* payload, int color){
    char* color_code;
    switch (color) {
    case COLOR_RESET:
	color_code = RESET;
        break;
    case COLOR_BLACK:
	    color_code = BLACK;
	    break;
    case COLOR_RED:
	    color_code = RED;
	    break;
    case COLOR_GREEN:
	    color_code = GREEN;
	    break;
    case COLOR_YELLOW:
	    color_code = YELLOW;
	    break;
    case COLOR_BLUE:
	    color_code = BLUE;
	    break;
    case COLOR_MAGNETA:
	    color_code = MAGNETA;
	    break;
    case COLOR_CYAN:
	    color_code = CYAN;
	    break;
    case COLOR_WHITE:
	    color_code = WHITE;
	    break;
    default:
        color_code = RESET;
    }
    attachMessage(payload, color_code);
}

void jumpCursor(StringFormatter* payload, int r, int c){
    char num[4];
    //will only support numerals up to 999, anything beyond that is ridiculous
    r %= 1000;
    c %= 1000;
    attachMessage(payload, MOVETO_PREF);
    noneZeroIntString(num, 4, r, 10);
    attachMessage(payload, num);
    attachMessage(payload, MOVETO_SEPARATOR);
    noneZeroIntString(num, 4, c, 10);
    attachMessage(payload, num);
    attachMessage(payload, MOVETO_SUFF);
}

void hideCursor(StringFormatter* payload){
    attachMessage(payload, HIDE);
}

void deleteLine(StringFormatter* payload){
    attachMessage(payload, DELETE);
}

void saveCursor(StringFormatter* payload){
    attachMessage(payload, SAVECURSOR);
}

void restoreCursor(StringFormatter* payload){
    attachMessage(payload, RESTORECURSOR);
}

//only backspaces to a max
void backSpace(StringFormatter* payload, int num){
    attachMessage(payload, MOVETO_PREF);
    char buf[3];
    noneZeroIntString(buf, 3, num, 10);
    attachMessage(payload, buf);
    attachMessage(payload, "D");
    for(int i=0; i<num && payload->length < payload->size - 6; i++){
        payload->content[payload->length++] = ' ';
    }
    payload->length += num;
    attachMessage(payload, MOVETO_PREF);
    attachMessage(payload, buf);
    attachMessage(payload, "D");
}

void setWindowBoundary(StringFormatter* payload, int top, int bottom){
    char num[4];
    //will only support numerals up to 999, anything beyond that is ridiculous
    attachMessage(payload, MOVETO_PREF);
    noneZeroIntString(num, 4, top, 10);
    attachMessage(payload, num);
    attachMessage(payload, MOVETO_SEPARATOR);
    noneZeroIntString(num, 4, bottom, 10);
    attachMessage(payload, num);
    attachMessage(payload, "r");
}

void attachMessage(StringFormatter* payload, const char* message){
    int i;
    for(i=0; payload->length<payload->size-1 && message[i]; payload->length++, i++){
	payload->content[payload->length] = message[i];
    }
    payload->content[payload->length] = 0;
}

void attachMessageExplicit(StringFormatter* payload, const char* message, int length){
    int i;
    for(i=0; payload->length<payload->size-1 && i<length; payload->length++, i++){
	    payload->content[payload->length] = message[i];
    }
    payload->content[payload->length] = 0;
}

