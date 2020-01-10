#include <stdio.h>		
#include "../sInclude/list.h"

void append(listNode ** head, listNode *newNode)
{
	if(*head == 0)
	{
		*head = newNode;
	}
	else
	{
		listNode *curT = *head;
		while(curT->next != 0)
		{
			curT = (*curT).next;
		}
		curT->next = newNode;
	}
}

/*
 * Ritorna il puntatore doppio al nodo il cui ID è pari a id 
 * =>
 *
 * Il vantaggio di questo approccio sta nella maggiore facilità 
 * ed ottimizzazione proprie dell'implementazione di una lista 
 * a puntatore singolo. Con una lista a puntatori doppi (figlio 
 * e padre) la search sarebbe stata, forse meno complessa, ma 
 * avremmo dovuto gestire un numero di puntatori doppio.
 *
 */

listNode ** search(int id, listNode ** headP)
{
	listNode ** pPQ = headP;
	if(*headP)
	{
		while(*pPQ != 0 && (*pPQ)->ID != id)
		{
			pPQ =  &((*pPQ)->next);
		}
		
	}
	return pPQ;
}

void freeNode(listNode ** curN);

void delete(int id, listNode ** head)
{
	listNode **toBeDel = search(id, head);

	
	if(*toBeDel)
	{
		listNode * tmp = *toBeDel;
		*toBeDel = (*toBeDel)->next;
		freeNode(&tmp);
	}
	else
	{
		printf("Rimozione impossibile: elemento non presente nella lista\n");
	}
}

