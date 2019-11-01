#include <charay.h>
#include <bwio.h>
#include <terminal.h>

void flush(TerminalOutput* payload){
    payload->compositePayload[0] = 0;
    payload->length = 0;
}

void clear(TerminalOutput* payload){
    attachMessage(payload, CLEAR);
}

void setColor(TerminalOutput* payload, int color){
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

void jumpCursor(TerminalOutput* payload, int r, int c){
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

void hideCursor(TerminalOutput* payload){
    attachMessage(payload, HIDE);
}

void deleteLine(TerminalOutput* payload){
    attachMessage(payload, DELETE);
}

void saveCursor(TerminalOutput* payload){
    attachMessage(payload, SAVECURSOR);
}

void restoreCursor(TerminalOutput* payload){
    attachMessage(payload, RESTORECURSOR);
}

//only backspaces to a max
void backSpace(TerminalOutput* payload, int num){
    attachMessage(payload, MOVETO_PREF);
    char buf[3];
    noneZeroIntString(buf, 3, num, 10);
    attachMessage(payload, buf);
    attachMessage(payload, "D");
    for(int i=0; i<num && payload->length < PAYLOAD_SIZE - 5; i++){
        payload->compositePayload[payload->length++] = ' ';
    }
    payload->length += num;
    attachMessage(payload, MOVETO_PREF);
    attachMessage(payload, buf);
    attachMessage(payload, "D");
}

void attachMessage(TerminalOutput* payload, char* message){
    int i;
    for(i=0; payload->length<PAYLOAD_SIZE-1 && message[i]; payload->length++, i++){
        // bwprintf(COM2, "Attaching the fucking message: %d", payload->length);
	    payload->compositePayload[payload->length] = message[i];
    }
    payload->compositePayload[payload->length] = 0;
}

void attachMessageExplicit(TerminalOutput* payload, char* message, int length){
    int i;
    for(i=0; payload->length<PAYLOAD_SIZE-1 && i<length; payload->length++, i++){
	    payload->compositePayload[payload->length] = message[i];
    }
    payload->compositePayload[payload->length] = 0;
}
