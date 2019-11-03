#include <uiHelper.h>
#include <terminal.h>
#include <charay.h>
#include <bwio.h>

//needs to use this to generate responsiveness
int utilBlinker = 0;

char frame[56][10] = {			
            "⢀⠀",
			"⡀⠀",
			"⠄⠀",
			"⢂⠀",
			"⡂⠀",
			"⠅⠀",
			"⢃⠀",
			"⡃⠀",
			"⠍⠀",
			"⢋⠀",
			"⡋⠀",
			"⠍⠁",
			"⢋⠁",
			"⡋⠁",
			"⠍⠉",
			"⠋⠉",
			"⠋⠉",
			"⠉⠙",
			"⠉⠙",
			"⠉⠩",
			"⠈⢙",
			"⠈⡙",
			"⢈⠩",
			"⡀⢙",
			"⠄⡙",
			"⢂⠩",
			"⡂⢘",
			"⠅⡘",
			"⢃⠨",
			"⡃⢐",
			"⠍⡐",
			"⢋⠠",
			"⡋⢀",
			"⠍⡁",
			"⢋⠁",
			"⡋⠁",
			"⠍⠉",
			"⠋⠉",
			"⠋⠉",
			"⠉⠙",
			"⠉⠙",
			"⠉⠩",
			"⠈⢙",
			"⠈⡙",
			"⠈⠩",
			"⠀⢙",
			"⠀⡙",
			"⠀⠩",
			"⠀⢘",
			"⠀⡘",
			"⠀⠨",
			"⠀⢐",
			"⠀⡐",
			"⠀⠠",
			"⠀⢀",
			"⠀⡀"
            };


void uiTimeStamp(TerminalOutput* payload, int time){
    flush(payload);
    saveCursor(payload);
    jumpCursor(payload, 0, 64);
    int hours, minutes, seconds, miliseconds;
    miliseconds = time%100;
    time/=100;
    seconds = time%60;
    time/=60;
    minutes = time%60;
    time/=60;
    hours = time;
    char timeBuff[12];
    formatStrn(timeBuff, 12 , "%d:%d:%d:%d", hours, minutes, seconds, miliseconds);
    attachMessage(payload, timeBuff);
    restoreCursor(payload);
}

void uiUtilizationRate(TerminalOutput* payload, int utilRate){
    flush(payload);
    saveCursor(payload);
    jumpCursor(payload, 0, 0);
    int percentile = utilRate / 10;
    int subp = utilRate % 10;
    char buffer[5];
    attachMessage(payload, "Utilization Rate:");
    formatStrn(buffer, 5, "%d.%d", percentile, subp);
    /*if(percentile > 75){
        setColor(payload, COLOR_GREEN);
    } else if(percentile > 50){
        setColor(payload, COLOR_CYAN);
    } else if(percentile > 25){
        setColor(payload, COLOR_YELLOW);
    } else {
        setColor(payload, COLOR_RED);
    }*/
    attachMessage(payload, buffer);
    setColor(payload, COLOR_RESET);
    attachMessage(payload, " "); 
    attachMessage(payload, frame[(utilBlinker++)%56]);
    restoreCursor(payload);
}
