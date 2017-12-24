#include "scheduling_simulator.h"

struct task *head;

void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    return 0;
}

struct task *getnewtask(char *task_name){
    struct task *newnode = (struct task*)malloc(sizeof(struct task));
    newnode->prev = NULL;
    newnode->next = NULL;
    newnode->namenext = NULL;
    newnode->nameprev = NULL;
    newnode->pid = ++gpid;
    newnode->name = task_name;
    newnode->state = TASK_READY;
    newnode->queuetime = 0;
    return newnode;
}

void removetask(int dpid){//dpid = delete pid
    struct task *tmp = head;
    while(tmp!=NULL){
        if(tmp->pid == dpid){
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            tmp->nameprev->namenext = tmp->namenext;
            tmp->namenext->nameprev = tmp->nameprev;
            free(tmp);
            printf("Remove pid %d ~\n",dpid);
            return;
        }
        tmp = tmp->next;
    }

    printf("Invalid pid remove action !!\n");
    return;
}

int hw_task_create(char *task_name)
{
    if(strcmp(task_name,"task1")!=0 && strcmp(task_name,"task2")!=0 && strcmp(task_name,"task3")!=0 && strcmp(task_name,"task4")!=0 && strcmp(task_name,"task5")!=0 && strcmp(task_name,"task6")!=0) return -1;
    struct task *tmp = head;
    struct task *nametmp = head;
    struct task *newtask = getnewtask(task_name);
    if(head == NULL){
        head = newtask;
        return newtask->pid;
    }

    while(tmp->next != NULL) tmp = tmp->next; //go to last node
    tmp->next = newtask;
    newtask->prev = tmp;

/*    while(nametmp != NULL) {
        if(strcmp(nametmp->name,task_name)==0){
            while(nametmp->namenext!=NULL) nametmp=nametmp->namenext;
            nametmp->namenext = newtask;
            newtask->nameprev = nametmp;
        }
        nametmp = nametmp->next;//search for same name node
    }*/

    return newtask->pid; // the pid of created task name
}


//void insertnode()
int main()
{
    char command[10];
    //struct task *head;
    head = NULL; //set empty list
    while(1){
        printf("$");
        scanf("%s",command);
        printf("%s\n",command);
        printf("pid=%d\n",hw_task_create("task1"));
    }
	return 0;
}
