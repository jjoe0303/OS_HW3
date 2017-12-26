#include "scheduling_simulator.h"

struct task *head;
struct task *temp;
struct task *now;
static ucontext_t ucp;

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
	(newnode->uc).uc_link = &ucp;
	(newnode->uc).uc_stack.ss_sp = newnode->st;
	(newnode->uc).uc_stack.ss_size = sizeof(newnode->st);
	return newnode;
}

void removetask(int dpid) //dpid = delete pid
{
	struct task *tmp = head;
	while(tmp!=NULL) {
		if((tmp->pid == dpid) && (tmp->next==NULL) && (tmp->prev == NULL)) {
			head = NULL;
			free(tmp);
			printf("Remove pid %d ~\n",dpid);
			return;
		} else if((tmp->pid == dpid) && (tmp->next==NULL)) { //is tail
			tmp->prev->next = NULL;
			free(tmp);
			printf("Remove pid %d ~\n",dpid);
			return;
		} else if((tmp->pid == dpid) && (tmp->prev==NULL)) { //is head
			head->next->prev = NULL;
			head = head->next;
			free(tmp);
			printf("Remove pid %d ~\n",dpid);
			return;
		} else if(tmp->pid == dpid) {
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			//	tmp->nameprev->namenext = tmp->namenext;
			//	tmp->namenext->nameprev = tmp->nameprev;
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

void runtask(char *task_name)
{

	if(strcmp(task_name,"task1")) {
		task1();
	} else if(strcmp(task_name,"task2")) {
		task2();
	} else if(strcmp(task_name,"task3")) {
		task3();
	} else if(strcmp(task_name,"task4")) {
		task4();
	} else if(strcmp(task_name,"task5")) {
		task5();
	} else if(strcmp(task_name,"task6")) {
		task6();
	}
	return;
}

void handler()
{

	setcontext(&ucp);
	return;
}

int main()
{
	char command[30];
	struct task *psp;
	char *newtaskname=(char *)malloc(sizeof(char)*20);
	head = NULL; //set empty list
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
					printf("%d\t%s\t%s\t%d\t%d\n",psp->pid,psp->name,task_string[psp->state],
					       psp->queuetime,psp->quantum);
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

				signal(SIGTSTP,handler);
				printf("simulating...\n");
				while(1) {}
			}
		}
	}
	return 0;
}
