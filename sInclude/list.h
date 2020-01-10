/*
 * Struttura dati usata come generalizzazione di un nodo di una lista.
 * Essa viene usata per uniformare il comportamento delle due diverse 
 * strutture dati usate nel primo (task) e terzo (client) esercizio.
 * In tal modo possono essere usate le medesime utility di gestione,
 * con conseguente risparmio di tempo per implementazione e righe di 
 * codice. Inoltre esso acquisisce maggiore generalità e capacità di 
 * essere riusato ed esteso.
 *
 */ 

typedef struct listNode
{
		int ID;
		struct listNode *next;
} listNode;


void append(listNode ** head, listNode *newNode);

listNode ** search(int id, listNode ** headP);

/*
 * Occorre notare come ogni src che includa questo header debba imple-
 * mentare in modo diverso questa funzione. Essa, difatti, ha il compito
 * di disallocare memoria dinamica usata per la memorizzazione di un nodo
 * della struttura dati usata nel codice sorgente che si serve delle utility
 * offerte da list.c. Tuttavia la dimensione di memoria usata dal singolo nodo
 * cambia in base alla struttura dati implementata; da ciò la necessità di
 * differenziare il comportamento della funzione.
 */

void freeNode(listNode ** curN);

void delete(int id, listNode ** head);
