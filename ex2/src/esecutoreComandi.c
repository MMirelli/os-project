/* Command executor
 * 
 * Nel codice e nei commenti sottostanti si menziona il termine "comando" indicando una istruzione nella sua interezza (<nomeComando> <arg1> <arg2> ...);
 * una "parola di comando" è altresì una componente del comando, essa può dunque essere il nome del comando stesso o un suo argomento.
 */
#include "../include/esecutoreComandi.h"

#include <malloc.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int status = 0;
int isSequential = 0; 

int main()
{

	do
	{

		printf("Select the execution type\n -> 0: parallel\n -> 1: sequential\n");
		isSequential = getchar() - '0';

	}while (isSequential != 0 && isSequential != 1);

	if(isSequential)
	{
		printf("Sequential execution selected\n");
	}
	else
	{
		printf("Parallel execution selected\n");
	}

	printf("Insert a command at a time and press enter (enter to terminate).\n");
	
	char buf [len];
	int readB;
	int cmdNum = 0;
	char *** cmdBlock = 0;		// si dichiara puntatore di terzo livello. Alla lettura di ciascuna riga da STDIN (corrispondente ad un ciclo
  					// del while di seguito), verrà aggiunto un componente alla lista di comandi (ciascuno: nome comando + argomenti)
					// puntata da cmdBlock 	

//	printf("Before any allocation\n");
//STAT	malloc_stats();

	while((readB = read(STDIN_FILENO, buf, len)) > 1) // se viene letto un solo byte si termina uscendo dal ciclo
	{		
		buf[readB - 1] = '\0';

		cmdBlock = realloc(cmdBlock, sizeof(char **) * ++cmdNum); // viene allocato spazio in memoria dinamica per la memorizzazione del comando
    						         		  // appena letto
//		printf("Allocating for cmd\n");	
//STAT		malloc_stats();
		cmdBlock[cmdNum-1] = genCommandStr(buf);
	
		if(isSequential)	
			childExec(cmdBlock[cmdNum-1], cmdNum);
	}

	if(!isSequential)					// nel caso in cui l'esecuzione sia parallela viene generato un numero di figli pari
								// al numero di comandi inseriti. Il parent attenderà poi ciascun child
	{
		for(int i = 0; i < cmdNum; i++)
			childExec(cmdBlock[i], i + 1);

		for(int i = 0; i < cmdNum; i++)
			wait(&status);
	}

	freeMem(cmdBlock, cmdNum);				// l'array di stringhe viene deallocato
	
	// malloc_stats();					// commentando la chiamata a freeMem ed attivando questo comando e gli altri mem_stats 
								// si può osservare che la memoria dinamica allocata viene tutta disallocata con successo.

	if (readB < 0)

	{
		perror("Reading error\n");
		exit(EXIT_FAILURE);
	}

	printf("\nCommands executed, outputs stored in ../output.\n");

	return EXIT_SUCCESS;
}

char ** genCommandStr(char buf[len])
{
	char * cmdWord;
	char ** cmd = 0;		// dichiarazione di puntatore di secondo livello che verrà utilizzato come array dinamico delle stringhe
					// che conterranno le cd "parole" di un comando (suo nome ed argomenti)
	int cmdL = 0;		
	const char delim[2] =  " ";
	
	cmdWord = strtok(buf, delim); // si memorizza la prima parola di comando. La funzione di libreria strtok si occupa di "troncare" 
				      // la stringa originaria in corrispondenza di delim
			
	while(cmdWord)
	{ 

		cmd = realloc(cmd, sizeof( char *) * ++cmdL); // si alloca una nuova posizione per il  puntatore alla nuova parola di comando 
	
//		printf("Allocating to cmdWord\n");	
//STAT		malloc_stats();

		cmd[cmdL-1] = calloc(sizeof(char),  strlen(cmdWord)); // si alloca spazio necessario per memorizzare la parola di comando appena
	       							      // estratta dal buffer tramite la strtok() 
		
//		printf("Allocating to cmd\n");	
//STAT		malloc_stats();

		strcpy(cmd[cmdL-1], cmdWord);

		cmdWord = strtok(NULL, delim); 			// si passa alla nuova parola di comando 	
			
		// printf("%s \n", cmd[i]);
		// cmdWord = strtok(NULL, delim);

	}

	cmd = realloc(cmd, sizeof(char *) * ++cmdL);	// allocazione dell'ultima posizione che conterrà NULL in modo da preparare l'array di
							// di stringhe alla execvp di seguito	
	cmd[cmdL-1] = NULL;

	return cmd;
}

void childExec(char ** cmd, int cmdNum)
{
	
	if(fork() == 0)
	{
		write(STDOUT_FILENO, "Executing command: ", 24); // si usa write poichè essa scrive subito sullo STDOUT, al contrario di printf che 
								       // memorizza prima su buffer addizionale offerto dalle funzioni di libreria C
		
		write(STDOUT_FILENO, cmd[0], strlen(cmd[0]));		// si stampa la prima parola di comando dell'istruzione eseguita 
		write(STDOUT_FILENO, "\n", 2);
		char outFiName[14 * sizeof(char) + sizeof(int)];

		sprintf(outFiName,"../output/out.%d", cmdNum);
	
		int fd = open(outFiName, O_WRONLY|O_CREAT|O_TRUNC, 0744); // nel caso in cui il file venga creato vengono assegnati permessi rw
							      		  // al solo proprietario gli altri utenti avranno il solo diritto di lettura	
	
		dup2(fd, STDOUT_FILENO);

		dup2(fd, STDERR_FILENO);				// anche gli errori, eventualmente riportati nella esecuzione di comandi vengono
									// reinderizzati nel file out.k
		
		execvp(cmd[0], &cmd[0]);
		
		perror("Command execution failed");
		exit(EXIT_FAILURE);
	}

	if(isSequential)					// nel caso si scelga una esecuzione sequenziale il parent aspetterà ciascun figlio
		wait(&status);					// prima di ritornare in esecuzione; ciò insieme al while che chiama childExec garantirà 
								// la sequenzialità 
		
}	

void freeMem(char *** cmdBlock, int cmdNum)
{
	for(int i = 0, j = 0; i < cmdNum; i++)
	{
		for(j = 0; cmdBlock[i][j]; j++)
		{
		//	printf("DEALLOCATING ** (strings)\n");		// stampe utili per la verifica delle disallocazioni da memoria
		//	printf(" commandWord n %d\n ", j + 1);
		
			free((cmdBlock)[i][j]);
		
		//	malloc_stats();
		}
		
	//	printf("DEALLOCATING * (arrays of strings)\n");
	//	printf("command n %d\n", i + 1);
	
		free(cmdBlock[i]);
	
	//	malloc_stats();
	}

	free(cmdBlock);
/*
	printf("DEALLOCATING pointer to arrays of strings\n");
	malloc_stats();
*/
}
