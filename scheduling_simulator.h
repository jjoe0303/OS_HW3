#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <ucontext.h>
#include <string.h>
#include <stdlib.h>

#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};

static const char *task_string[] = {
        "TASK_RUNNING",
        "TASK_READY",
        "TASK_WAITING",
        "TASK_TERMINATED"
};

struct task {
	int pid;
	char *name;
	int state;
	int queuetime;
    int quantum;
    int runtime;
    struct task *next;
	struct task *prev;
	struct task *namenext;//search for name
	struct task *nameprev;
//    ucontext_t uc;
};

int gpid=0;//global pid
void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);

#endif
