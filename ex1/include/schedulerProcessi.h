#define lineL 51     // variabile che indica la lunghezza delle stringhe usate per stampare il menù

/*
 * Struttura dati che memorizza per ciascun task l'id, il puntatore al successivo, nome, priorità e 
 * numero di esecuzioni rimanenti.
 */ 
typedef struct task
{
	int ID;
	struct task *next;
	char name[8];
	int priority;
	int spareEx;
} task;

void execHead();

void execute(int id);

void modPri(int id);

void switchSP();

void freeNode(listNode ** curNode);

void printQueue(task *start);

int getId();

void sort(); 

void insert();
