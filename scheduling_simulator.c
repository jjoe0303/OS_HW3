#include "scheduling_simulator.h"

//task list pointer
struct task *head;
struct task *temp;
struct task *wake;
struct task *wakename;
struct task *now;
//shell ucp
static ucontext_t ucp;
//scheduler ucs
static ucontext_t ucs;
//terminate uct
static ucontext_t uct;
//trash context
//static ucontext_t trash;

//timer setup
struct itimerval value,ovalue;

//ctrl+z flag
int flag=0;

//suspend flag
int sus=0;

void hw_suspend(int msec_10)
{
    now->waittime = msec_10;
    now->state = TASK_WAITING;
    printf("pid %d now waiting...\n",now->pid);
    swapcontext(&(now->uc),&ucs);
    //sleep(1);
    return;
}

void hw_wakeup_pid(int pid)
{
    wake = head;
    while(wake!=NULL){
        if(wake->pid == pid){
            if(wake->state == TASK_WAITING){
                wake->state = TASK_READY;
                printf("wake up pid %d\n",wake->pid);
            }
            else{
                printf("pid %d is not in waiting queue!!\n",wake->pid);
            }
            return;
        }
        wake = wake->next;
    }
    printf("No such pid~\n");
    return;
}

int hw_wakeup_taskname(char *task_name)
{
    int count=0;
    wakename = head;
    while(wakename!=NULL){
        if(strcmp(wakename->name,task_name)==0){
            if(wakename->state == TASK_WAITING){
                wakename->state = TASK_READY;
                count++;
                printf("wake up pid %d , name = %s\n",wakename->pid,wakename->name);
            }
            else{
                printf("pid %d %s is not in waiting queue!!\n",wakename->pid,wakename->name);
            }
        }
        wakename = wakename->next;
    }

    return count;
}

struct task *getnewtask(char *task_name)
{
    struct task *newnode = (struct task*)malloc(sizeof(struct task));
    newnode->prev = NULL;
    newnode->next = NULL;
    newnode->namenext = NULL;
    newnode->nameprev = NULL;
    newnode->pid = ++gpid;
    newnode->name = (char *)malloc(sizeof(char)*20);
    strcpy(newnode->name,task_name);
    newnode->state = TASK_READY;
    newnode->queuetime = 0;
    newnode->quantum = 10;
    getcontext(&(newnode->uc));
    (newnode->uc).uc_link = &uct;
    (newnode->uc).uc_stack.ss_sp = newnode->st;
    (newnode->uc).uc_stack.ss_size = sizeof(newnode->st);
    return newnode;
}

void removetask(int dpid) //dpid = delete pid
{
    if(now->pid==dpid) flag=0;
    struct task *tmp = head;
    while(tmp!=NULL) {
        if((tmp->pid == dpid) && (tmp->next==NULL) && (tmp->prev == NULL)) {
            head = NULL;
            //			free(tmp);
            printf("Remove pid %d ~\n",dpid);
            return;
        } else if((tmp->pid == dpid) && (tmp->next==NULL)) { //is tail
            tmp->prev->next = NULL;
            //			free(tmp);
            printf("Remove pid %d ~\n",dpid);
            return;
        } else if((tmp->pid == dpid) && (tmp->prev==NULL)) { //is head
            head->next->prev = NULL;
            head = head->next;
            //			free(tmp);
            printf("Remove pid %d ~\n",dpid);
            return;
        } else if(tmp->pid == dpid) {
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            //	tmp->nameprev->namenext = tmp->namenext;
            //	tmp->namenext->nameprev = tmp->nameprev;
            //			free(tmp);
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
    if(strcmp(task_name,"task1")!=0 && strcmp(task_name,"task2")!=0
            && strcmp(task_name,"task3")!=0 && strcmp(task_name,"task4")!=0
            && strcmp(task_name,"task5")!=0 && strcmp(task_name,"task6")!=0) return -1;
    struct task *tmp = head;
    //struct task *nametmp = head;
    struct task *newtask = getnewtask(task_name);
    if(strcmp((newtask->name),"task1")==0) {
        makecontext(&(newtask->uc),task1,0);
        printf("set task1 context\n");
    } else if(strcmp((newtask->name),"task2")==0) {
        makecontext(&(newtask->uc),task2,0);
        printf("set task2 context\n");
    } else if(strcmp((newtask->name),"task3")==0) {
        makecontext(&(newtask->uc),task3,0);
        printf("set task3 context\n");
    } else if(strcmp((newtask->name),"task4")==0) {
        makecontext(&(newtask->uc),task4,0);
        printf("set task4 context\n");
    } else if(strcmp((newtask->name),"task5")==0) {
        makecontext(&(newtask->uc),task5,0);
        printf("set task5 context\n");
    } else if(strcmp((newtask->name),"task6")==0) {
        makecontext(&(newtask->uc),task6,0);
        printf("set task6 context\n");
    } else {
        printf("No such task!!\n");
    }

    if(head == NULL) {
        head = newtask;
        return newtask->pid;
    }

    while(tmp->next != NULL) tmp = tmp->next; //go to last node
    tmp->next = newtask;
    newtask->prev = tmp;

    /*	    while(nametmp != NULL) {
            if(strcmp(nametmp->name,task_name)==0){
            while(nametmp->namenext!=NULL) nametmp=nametmp->namenext;
            nametmp->namenext = newtask;
            newtask->nameprev = nametmp;
            }
            nametmp = nametmp->next;//search for same name node
            }
     */
    return newtask->pid; // the pid of created task name
}

void (*oldahandler)(int);
void (*oldzhandler)(int);

void init_shell()
{
    //close timer
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);

    //reset signal
    signal(SIGALRM,oldahandler);
    signal(SIGTSTP,oldzhandler);
}

void zhandler()
{
    init_shell();
    flag=1;
    swapcontext(&(now->uc),&ucp); //why this line not restore ucs (not true yet)
    return;
}

void scanlist()
{
    temp = head;
    while(temp->next!=NULL){
        if(temp->state == TASK_READY){
            (temp->queuetime)+=10;
        }
        else if(temp->state == TASK_RUNNING){
            (temp->runtime)-=10;
        }
        else if(temp->state == TASK_WAITING){
            temp->waittime=(temp->waittime)-1;
            if((temp->waittime)<=0){
                temp->state = TASK_READY;
            }
        }
        temp = temp->next;
    }
    //the last task(task->next == NULL)
    if(temp->state == TASK_READY){
        (temp->queuetime)+=10;
    }
    else if(temp->state == TASK_RUNNING){
        (temp->runtime)-=10;
    }
    else if(temp->state == TASK_WAITING){
        temp->waittime=(temp->waittime)-1;
        if((temp->waittime)<=0){
            temp->state = TASK_READY;
        }
    }
}

void timeout()
{
    //chack every task
    scanlist();

    if((now->state==TASK_RUNNING)&&(now->runtime<=0)) {
     //   printf("pid %d change from running to ready!!\n",now->pid);
        now->state=TASK_READY;
        swapcontext(&(now->uc),&ucs);
    }
    //printf("haha\n");
    return;
}

void terminate()
{
    now->state=TASK_TERMINATED;
    printf("pid %d terminated~\n",now->pid);
    scanlist();
    swapcontext(&(now->uc),&ucs);
    //	return;
}

void scheduler()
{
    if(now==NULL) now=head;
    while(1) {
        if(now->state==TASK_READY) {
            now->state = TASK_RUNNING;
            now->runtime = now->quantum;
//            printf("pid %d is running\n",now->pid);
            swapcontext(&ucs,&(now->uc));
            //printf("Im pid:%d\n",now->pid);
        }
 //       if((now->runtime)<=0){
            if(now->next!=NULL) {
                now=now->next;
            } else if(now->next == NULL) { //tail
                now=head;
            }
 //       }
    }

}

int main()
{
    char command[30];
    struct task *psp;
    char st1[8192];
    char *newtaskname=(char *)malloc(sizeof(char)*20);
    head = NULL; //set empty list
    //initalize scheduler context
    getcontext(&ucs);
    ucs.uc_link = &ucp;
    ucs.uc_stack.ss_sp = st1;
    ucs.uc_stack.ss_size = sizeof st1;
    makecontext(&ucs,scheduler,0);

    char stt[8192];
    getcontext(&uct);
    uct.uc_link = &ucs;
    uct.uc_stack.ss_sp = stt;
    uct.uc_stack.ss_size = sizeof stt;
    makecontext(&uct,terminate,0);
    while(1) {
        getcontext(&ucp);
        printf("$");
        fgets(command,sizeof(command),stdin);
        printf("%s",command);

        //ps task
        if(strcmp(command,"ps\n")==0) {
            if(head==NULL) {
                continue;
            } else {
                psp = head;
                while(psp!=NULL) {
                    printf("%d\t%s\t%s\t%li\n",psp->pid,psp->name,task_string[psp->state],
                            psp->queuetime);
                    psp=psp->next;
                }
            }
            continue;
        }

        else if(command[0]=='a'&&command[1]=='d'&&command[2]=='d') {
            int i=3;
            int flag=0;
            char quantum;
            memset(newtaskname,'\0',sizeof(newtaskname));
            quantum=' ';
            while(command[i]!='\n') {
                if(command[i]==' ') {
                    i++;
                    continue;
                }
                if(command[i]=='-'&&command[i+1]=='t') {
                    flag=1;
                    i=i+2;
                    continue;
                }
                if(flag==0) {
                    sprintf(newtaskname,"%s%c",newtaskname,command[i]);
                    //printf("newtaskname=%s\n",newtaskname);
                } else {
                    quantum=command[i];
                    //printf("quantum = %c\n",quantum);
                }
                i++;
            }
            if(strcmp(newtaskname,"")==0) {
                printf("Invalid argument to add !!\n");
                continue;
            }
            printf("newtask=%s,quantum=%c\n",newtaskname,quantum);
            int pp=hw_task_create(newtaskname);
            printf("pp=%d\n",pp);
            if(pp==-1) {
                printf("Invalid task to create !!\n");
                continue;
            }
            struct task* tail = head;
            while(tail->next!=NULL) tail=tail->next;
            if(quantum=='L') {
                tail->quantum = 20;
            }
        }

        else if(command[0]=='r'&&command[1]=='e'&&command[2]=='m'&&command[3]=='o'
                &&command[4]=='v'&&command[5]=='e') {
            char ddpid[10];
            memset(ddpid,'\0',sizeof(ddpid));
            int i =6;
            while(command[i]!='\n') {
                if(command[i]==' ') {
                    i++;
                    continue;
                }

                sprintf(ddpid,"%s%c",ddpid,command[i]);
                i++;
            }

            int dpid = atoi(ddpid);
            removetask(dpid);
        }

        else if(strcmp(command,"start\n")==0) {
            if(head==NULL) {
                printf("there's no job to start!\n");
                continue;
            } else {
                //set signal
                oldahandler=signal(SIGALRM,timeout);
                oldzhandler=signal(SIGTSTP,zhandler);
                //set timer
                value.it_value.tv_sec = 0;
                value.it_value.tv_usec = 10000;
                value.it_interval.tv_sec = 0;
                value.it_interval.tv_usec = 10000;
                setitimer(ITIMER_REAL, &value, &ovalue);
                printf("simulating...\n");
                if(flag==1) {
                    flag=0;
                    swapcontext(&ucp,&now->uc);
                    continue;
                } else {
                    swapcontext(&ucp,&ucs);
                }
            }
        }
    }
    return 0;
}
