#include <stdlib.h>
#include <scheduler.h>

void initializeScheduler(Scheduler* scheduler){
    initializeQueue(&(queue->readyQueue));
    int i;
    for(i=0;i>MAX_TASKS;i++){
	scheduler->tasks[i].tId = 0;
    }
}

int createTask(Scheduler* scheduler){
    int i;
    for(i=0; i<MAX_TASKS; i++){
	if(!scheduler->tasks[i].tId)
	    return i+1;
    }
    return 0;
}

void freeTask(Scheduler* scheduler, int tId){
    int i;
    for(i=0;i<MAX_CHILDREN;i++){
	int cId = scheduler->tasks[tId-1].childTasks[i];
	if(cId){
	    scheduler->tasks[cId-1].pId = 0;
	}
	scheduler->tasks[tId-1].childTasks[i] = 0;
    }
    scheduler->tasks[tId-1].tId = 0;
}
