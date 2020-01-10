// Process scheduler

#include "../../sInclude/list.h"
#include "../include/schedulerProcessi.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

enum sortPolicy {pr, spareExe};

int curSortPol = pr;
task *head = 0;
int curID = 0;

char sepL[lineL] = "+----+-----------+-----------+-------------------+";
char header[lineL] = "| ID | PRIORITA' | NOME TASK | ESECUZ. RIMANENTI |"; 

/* 1.
 * Menu
 *
 * */

int main()
{
	while(1)
	{
		printf("Press 1 for inserting.\n");
		printf("Press 2 for executing the head of the queue.\n");
		printf("Press 3 for executing a specific task.\n");
		printf("Press 4 for deleting a task.\n");
		printf("Press 5 for modifying the priority of a process.\n");
		printf("Press 6 for modifying the sorting policy.\n");
		printf("Press 7 for exiting.\n");

		int opNum;
		scanf("%d", &opNum);
		switch(opNum)
		{
			case 1: insert(); break;
			case 2: execHead(); break;
			case 3: execute(getId()); break;
			case 4: delete(getId(), (listNode **) &head); break;
			case 5: modPri(getId()); break;
			case 6: switchSP();break;
			case 7: return(EXIT_SUCCESS); break;
		}
		sort(); // il sorting avviene dopo l'esecuzione di una qualunque operazione scelta dall'utente, 
			// secondo la politica di ordinamento corrente essa di default è per priorità decrescente
		printQueue(head);
	}
}

/* 2.
 * Operazioni come da interfaccia
 * 
 * */

void insert()
{	
	task *nT = (task *) calloc(1,sizeof(task));
	append( (listNode **) &head, (listNode *) nT);
	curID += 1;
	(*nT).ID = curID;	
	do
	{	
		printf("Insert task name (8 max)\n");
        	scanf("%s", (char *) &(nT->name));
	} while(strlen(nT->name) > 8);
	
	do
	{
		printf("Insert priority (0 to 9)\n");
		scanf("%d", &(nT->priority));
	} while(nT->priority < 0 || nT->priority > 9);
	
	do
	{
		printf("Insert remaining executions for the task (1 to 99)\n");
		scanf("%d", &(nT->spareEx));
	} while(nT->spareEx < 1 || nT->spareEx > 99);

	nT->next = 0;	
}



void execHead()
{
	if(head)
	{
		head->spareEx -= 1;
		if(!(head->spareEx))
		{
			task* tmp = head;
			
			if(head->next)
			{
				head = head->next;
			}
			else
			{
				head = 0;
			}
			free(tmp);
		}
	}
	else
	{
		printf("Impossible to execute the head process.\n");
	}
}

void execute(int id)
{
	task **toBeEx = (task **) search(id, (listNode **) &head);
	if(*toBeEx)
	{
		(*toBeEx)->spareEx -= 1;
		
		if(!(*toBeEx)->spareEx) 
			delete(id, (listNode **) &head);
	}

}

void freeNode(listNode ** curNode)
{
	free((task *) *curNode);
}


void modPri(int id)
{
	task **toBeMod = (task **) search(id, (listNode **) &head);	
	if(*toBeMod)
	{	
		do
		{
			printf("Insert priority (0 to 9)\n");
			scanf("%d", &((*toBeMod)->priority));
		}while ((*toBeMod)->priority < 0 || (*toBeMod)->priority > 9);
	}
}

void switchSP()
{
	curSortPol = (curSortPol + 1) % 2;
	if(curSortPol)
	{
		printf("Ascending sorting policy by priority \n");
	}
	else
	{
		printf("Descending sorting policy by priority \n");
	}
}


/* 3.
 * Utilities 
 *
 */

void printQueue(task *start)
{
		task *curT = start;
		if(curT == 0 && curT == head)
		{
			printf("Empty task queue\n");
		}
		else
		{
			printf("%s\n",sepL);	
			printf("%s\n",header);
			printf("%s\n",sepL);	
			while(curT != 0)
			{
				printf("| %d  |     %d     |    %s    |         %d         |\n", curT->ID, curT->priority, curT->name, curT->spareEx);
				curT = (*curT).next;
				printf("%s\n",sepL);	
			}
		}
}

int getId()
{
	int id;
	printf("Insert task id\n");
	scanf("%d", &id);
	return id;
}

int notSorted(task *fst, task *snd);

/*
 * L'algoritmo di ordinamento usato è un selection sort su di una lista.
 */

void sort()
{
	task **ptrPrev = &head;
	int e = 0, i = 0;
	while(*ptrPrev != 0)
	{
		task *ptrTmp = *ptrPrev;
		task *ptrToChange = ptrTmp;
		while(ptrTmp != 0)
		{
			ptrTmp = ptrTmp->next;
			
			if(notSorted(ptrToChange, ptrTmp))
			{
				ptrToChange = ptrTmp;
			}
		}
		task *tmp1 = *ptrPrev;
		task *tmp2 = ptrToChange->next;
		
		*ptrPrev = ptrToChange;
		ptrToChange->next = tmp1;
		
		while(tmp1->next != ptrToChange)
			tmp1 = tmp1->next;

		tmp1->next = tmp2;
		ptrPrev = &((*ptrPrev)->next);

	}
}

/*
 * Procedura che si occupa di verificare se due task nella coda 
 * sono in ordine rispetto alla politica di ordinamento vigente
 *
 * */

int notSorted(task *fst, task *snd)
{
	int rst = 0;
	if(snd)
	{	
		if(curSortPol == spareExe)
		{
			if(fst->spareEx == snd->spareEx)
			{
				rst = fst->ID < snd->ID;
			}
			else
			{
				rst = fst->spareEx > snd->spareEx;
			}
		}
		else
		{
			if(fst->priority == snd->priority)
			{
				rst = fst->ID < snd->ID;
			}
			else
			{
				rst = fst->priority < snd->priority;
			}
		}
	}
			return rst; 
}
