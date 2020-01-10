#include "../../sInclude/list.h"
#include "../include/util.h"
#include "../include/server.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h> 

#define reqL 45 	// massima lunghezza della stringa che comunica la richiesta di un comando generata da un client. 
			// Il comando di lunghezza maggiore può essere la send con formato: "<char comando><pid mittente>
			// <pid destinatario> <messaggio>". Ammesso che la piattaforma usata per l'esecuzione codifichi gli
			// interi su 4 byte occorreranno al più 10 char (2^31) per codificare il numero in base 10. A questi
			// 20 byte per gli id si aggiungono: il char di comando iniziale, i due spazi di delimitazione, 20 
			// byte di messaggio, il carattere di invio ed il delimitatore di fine stringa. Il totale è appunto 45.


listNode * head = 0;
int nodeN = 0;

/*
 * Ciascun client è memorizzato in una lista che contiene il solo identificatore che è anche il pid del processo client.
 * Ad ogni client registrato al server è associata una FIFO contenuta nella directory "pipes" del progetto.
 */
void main()
{
	printf("Server waiting for inputs\n");
	unlink("./pipes/cToS");

	if(mknod("./pipes/cToS", S_IFIFO|0666, 0) < 0)	 	// vengono dati permessi di w e r a tutti gli utenti 
	{	
		perror("Impossible to generate FIFO");
		return;
	}
	
	int cToSFd;
	if((cToSFd = open("./pipes/cToS", O_TRUNC|O_RDONLY)) < 0) // si blocca l'esecuzione fino a che la FIFO non è aperta in SCRITTURA 
	{
		perror("Impossible to open pipeClientToServer"); 
		return;
	}

	while(1)
	{
		char buf[reqL];
		int justRead = 0;

		while((justRead = read(cToSFd, buf, reqL)))
		{
			char * bufSafe = calloc(justRead + 1, sizeof(char));
			
			memcpy(bufSafe, buf, justRead);

				printf("Message read from pipe: %s\n", bufSafe);
				switch(bufSafe[0])		// il primo char della richiesta contiene il comando, filtrato dallo switch
								// alle varie funzioni si passa la stringa dal carattere successivo
				{	
					case 'C': connect(bufSafe + 1);break;
					case 'L': list(head, bufSafe + 1);break;
					case 'S': send(bufSafe + 1); break;
					case 'R': rem(bufSafe + 1); break;
				}
			free(bufSafe);
		}
	}
}

/*
 * Rimuove dalla lista il client specificato dal comando (*inBuff).
 */

void rem (char * inBuf)
{
	int pid = atoi(inBuf);
	
	if(openClientPipe(pid) >= 0)
	{
		char pipName[dirNameL + getPidLength(pid)];
		
		sprintf(pipName, "./pipes/%d", pid);
			
		printf("%d disconnecting and deleting pipe %s\n", pid, pipName);
			
		unlink(pipName);
		
		delete(pid, &head);
	}
	printf("Connessioni attive:\n");
	printQueue(head);
}	
		



int  openClientPipe(int pid)
{
	char pipName[dirNameL + getPidLength(pid)];
	
	sprintf(pipName, "./pipes/%d", pid);
	
	int rst = 0; 
	
	if((rst = open(pipName, O_WRONLY)) < 0)
	{
		perror("Impossible to open the FIFO");
		printf("Operation not successful, is the client connected?\n");
	}
	return rst;
}

void connect(char * curSPid)
{
	
	int pid = atoi(curSPid);	
	
	listNode ** searchedP = search(pid, &head);

	if(!(*searchedP)) // se l'elemento non fosse già presente nella lista lo si aggiunge, altrimenti la connect non ha alcun effetto 
	{
		char pipName[dirNameL + getPidLength(pid)];
	
		sprintf(pipName, "./pipes/%d", pid);
		
		printf("Client pipe name %s\n", pipName);
		
		unlink(pipName);

		if(mknod(pipName, S_IFIFO|0666, 0) < 0) // generazione della FIFO server to client
			perror("FIFO not generated");

		
		listNode * newC = (listNode *) calloc(1, sizeof(listNode)); // allocazione di memoria per il nuovo client
		
		append(&head, newC);
		
		nodeN++;

		newC->ID = pid;
		newC->next = 0;
	}	
	printf("Active connection:\n");
	printQueue(head);
}

/*
 * Scrive nella pipe del client invocante un id alla volta dei client registrati 
 */ 

void list(listNode *start, char * curSPid)
 {
	listNode *curC = start;
					// affinchè tale richiesta venga presa in carico la lista deve contenere
					// almeno un nodo -> non è necessario alcun controllo sulla testa della coda	
	
	int pid = atoi(curSPid);	
	
	int pFD;
	if((pFD	= openClientPipe(pid)) < 0)
		return;

	while(curC!=0)
		{
			char buf[getPidLength(curC->ID) + 1];
			
			int wri = sprintf(buf,"%d", curC->ID); // riduce in stringa gli id numerici ritornando il numero di byte scritti
			curC = (*curC).next;
			buf[wri] = '\0'; 			// aggiunge il terminatore di stringa
			write(pFD, buf, strlen(buf)+1);
		}
	close(pFD);	

}

/*
 * Fornisce all'utente un'interfaccia con la lista dei client connessi.
 */

void printQueue(listNode *start)
 {
	listNode *curC = start;
	if(curC == 0 && curC == head)
	{
		 printf("Empty queue\n");
	}
	else
	{
		while(curC != 0)
		{
			printf("|  %d  |\n", curC->ID);
			curC = (*curC).next;
		}
	}
}

/*
 * Disalloca la memoria che memorizza un listNode. Questa funzione viene usata dalla funzione delete 
 * nel src in condivisione con il primo esercizio.
 */

void freeNode(listNode ** curNode)
{
	free(*curNode);
}

/*
 * Decompone il comando di una send (dal seguente formato "<char comando>
 * <pid mittente> <pid destinatario> <messaggio>") ritornando un array di 
 * interi, il cui primo elemento è il pid del mittente, il secondo quello 
 * del destinatario ed il terzo la posizione del cursore dopo aver letto
 * i precedenti. 
 */

int * trimData(char * curData)
{
	int * rst = calloc(3, sizeof(int)); // si alloca memoria dinamica per il risultato, qualora non lo
					    // si facesse potrebbe difatti andare perduto (in quanto variabile
					    // locale in stack)
	char tmp = *curData;
	int i = 0;
	rst[0] = 0;
	while(tmp != ' ')
	{
		rst[0] = (rst[0] * 10) + tmp - '0'; // conversione di una cifra alla volta del pid del mittente
		tmp = curData[++i];
	}

	tmp = curData[++i]; // si passa alla prima cifra del destinatario
	

	rst[1] = 0;
	while(tmp != ' ')
	{
		rst[1] = (rst[1] * 10) + tmp - '0'; // conversione di una cifra alla volta del pid del destinatario
		tmp = curData[++i];
	}
	rst[2] = i;
	return rst;

}
/*
 * Decodifica la richiesta di send del client e verifica se il destinatario è registrator.
 * In tal caso gli invia un signal USR1, depositando nella sua pipe il messaggio ed il pid
 * del mittente; altrimenti invia un signal al mittente e memorizza sulla sua pipe l'iden-
 * tificativo del destinatario. 
 */

void send(char * curData)
{
	int *data = trimData(curData); // data[0] sender, data[1] receiver, data[2] posizione nella stringa

	int i = data[2];

	char tmp = curData[++i]; // si passa al primo carattere del messaggio 

	char * msg = (char *) calloc(msgL + 1, sizeof(char)); // poichè msg in client ha dimensione 20 (+ 1 char fine stringa)

	int j = i;

	while(tmp != '\n' && tmp != '\0')
	{
		msg[i - j] = tmp;
		tmp = curData[++i];
	}
	
	j = i; // j è il numero di char del messaggio letti

	int fd;
	if(*search(data[1], &head))
	{
		printf("Receiver found, forwarding...\n");

		kill(data[1], SIGUSR1);	
		
		if((fd = openClientPipe(data[1])) < 0)
		       goto OERR;			// nel caso il client non sia connesso è tuttavia necessario disallocare memoria dinamica	

		for(i = 0; curData[i] != ' '; i++) // scrittura pid del sender
		{
			write(fd, curData + i, 1);
		}
		write(fd, curData+i, 1);	// scrittura ' '		
		write(fd, msg, j);		// scrittura messaggio
	}
	else
	{
		printf("Receiver not found, sending the message back...\n");

		kill(data[0], SIGUSR2);

		if((fd = openClientPipe(data[0])) < 0)
			goto OERR;

		char destString[13]; 			// i pid hanno massima lunghezza di 12 char essendo codificati su 4 sizeof char
		int written = sprintf(destString,"%d", data[1]); 

		destString[written] = ' ';  // si inserisce spazio per rendere uniforme il tipo di lettura che verrà svolto dal receiver

		write(fd, destString, written + 1);
		
	}

	close(fd);
OERR:	free(data);
	free(msg);
}

