
#include <task.h>

void initializeTask(Task* task, int tId, int pId, int priority){
    task->tId = tId;
    task->pId = pId;
    task->priority = priority;
    int i;
    for(i=0;i<MAX_CHILDREN;i++){
	task->childTasks[i] = 0;
    }
}

int appendChildTasks(Task* task, int tId){
    int i;
    for(i=0; i<MAX_CHILDREN; i++){
	if(!task->childTasks[i]){
	    task->childTasks[tId];
	    return 0;
	}
    }
    return 1;
}
