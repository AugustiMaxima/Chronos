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


void uiTimeStamp(StringFormatter* payload, int time){
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
	timeBuff[0] = '0' + hours / 10;
	timeBuff[1] = '0' + hours % 10;
	timeBuff[2] = ':';
	timeBuff[3] = '0' + minutes / 10;
	timeBuff[4] = '0' + minutes % 10;
	timeBuff[5] = ':';
	timeBuff[6] = '0' + seconds / 10;
	timeBuff[7] = '0' + seconds % 10;
	timeBuff[8] = ':';
	timeBuff[9] = '0' + miliseconds / 10;
	timeBuff[10] = '0' + miliseconds % 10;
	timeBuff[11] = 0;
    attachMessage(payload, timeBuff);
    restoreCursor(payload);
}

void uiUtilizationRate(StringFormatter* payload, int utilRate){
    flush(payload);
    saveCursor(payload);
    jumpCursor(payload, 0, 0);
    int percentile = utilRate / 100;
    int subp = utilRate % 100;
    char buffer[5];
    attachMessage(payload, "Utilization Rate: ");
    formatStrn(buffer, 5, "%d.%d", percentile, subp);
    if(percentile > 75){
        setColor(payload, COLOR_GREEN);
    } else if(percentile > 50){
        setColor(payload, COLOR_CYAN);
    } else if(percentile > 25){
        setColor(payload, COLOR_YELLOW);
    } else {
        setColor(payload, COLOR_RED);
    }
    attachMessage(payload, buffer);
    setColor(payload, COLOR_RESET);
    restoreCursor(payload);
}
