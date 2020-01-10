#include "../include/util.h"
#include "../include/client.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h> 
#include <stdio.h> 

int cToSFd;
void (*oldIntHan)(int); 

/*
 * Installa gli handler e gestisce il menu. 
 */


int main()
{

	signal(SIGUSR1, msgReceivedHan);
	signal(SIGUSR2, recNotExisHan);
	oldIntHan = signal(SIGINT, usrInterHan);

	while(1)
	{	

		char selC[3];
		int sel = 0;
		printf("Select one among the following and press enter:\n1 -> server connection\n2 -> printing clients list\n3 -> sending message to a client\n4 -> disconnect from server\n5 -> terminate\n");

		while(sel < 1 || sel > 5)
		{
			fgets(selC, sizeof selC, stdin); // l'incremento di due è dovuto alla memorizzazione nella stringa di '\n' inserito dall'utente e '\0'
			
			sel = atoi(selC);
			if(sel < 1 || sel > 5)
				printf("Select one of the options:\n");
		}	
		
		if((cToSFd = open("./pipes/cToS", O_WRONLY)) < 0)
			perror("Error: impossible to open FIFO");
		switch(sel)
		{
			case 1: connect(); break;
			case 2: list(); break;
			case 3: send(); break;
			case 4: disconnect(); break;
			case 5: disconnect(); return(EXIT_SUCCESS);
		}
	}
}

/*
 * Funzione che si occupa di inviare al server il comando da eseguire. Esso è nel formato: "<char comando><id del richiedente>" l'id è 
 * anche il pid del client, univoco ed utilizzabile anche per generare i nomi delle pipe.
*/

void sendReq(char reqID)
{
	char s[getPidLength(getpid()) + 2 * sizeof(char)]; // i 2 supplementari sono uno per char di richiesta ed uno per terminatore di stringa
	sprintf(s, "%c%d", reqID, (int) getpid()); 
	printf("Command %s sent to server\n", s);
	write(cToSFd, s, strlen(s) + sizeof(char)); 	
}

/*
 * Apre la pipe server to client del client invocante, restituendo il file descriptor.
 */

int openMyPipe() 
{

	char pipName[dirNameL + getPidLength(getpid())];
	
	sprintf(pipName, "./pipes/%d", getpid());
	
	int rst = 0;
	
	if((rst = open(pipName, O_RDONLY)) < 0)
	{
		perror("Error: impossible to open FIFO ");
		printf("Please make sure that the client is connected to the server\n");
	}	
	return rst;

}

/*
 * Invia al server richiesta di connessione.
 */

void connect()
{
	sendReq('C');
	printf("Client %d connected to the server\n", getpid());
}

/*
 * Invia al server richiesta di listare i client registrati, legge e stampa in STDOUT l'elenco.
 */

void list()
{
	sendReq('L'); 	
	
	int pFD;

	if((pFD = openMyPipe()) < 0) // in tal caso client non è registrato a server
		return;
	
	char buf[1];
	printf("\nList of registered clients\n");
	printf("\tIDs\t\n");

	printf("\t");
	while(read(pFD, buf, 1))
	{
		if(*buf != '\0')
		{
			printf("%c", *buf);
		}
		else
		{
			printf("\n\t");
		}
	}
	printf("\n");
	close(pFD);
}

/*
 * Invia al server richiesta di disconnessione.
 */

void disconnect()
{
	sendReq('R');
	openMyPipe();   // se non è connesso informa che l'operazione non è avvenuta
	
	close(cToSFd);
}

/*
 * Invia al server richiesta di inoltrare un messaggio ad uno o più client registrati.
 */

void send()
{
		int destPid = 0, qt, readC;
		char destPidB[12], sQt[4], msg[msgL + 2 + 5];// destPidB: stringa usata come buffer per memorizzare id di client destinatari
							   // msg: buffer che memorizza il messaggio da inoltrare esso ha lunghezza 22 in
							   // quanto deve memorizzare anche il newline ed il carattere di fine stringa. Infine 
							   // 5 char in più vengono aggiunti per rendere più robusta la reazione a possibili 
							   // errori di inserimento dell'utente.

		do
		{
			printf("Type a message (max 20)\n");

			readC = read(STDIN_FILENO, msg, msgL + 2 + 5); 

		} while(readC > 20);

			msg[readC-1] = '\0';  // rimpiazzo il newline con carattere di fine stringa

			printf("Input letto: %s\n", msg);
		
		do
		{
			printf("Insert the quantity of receiving clients (max 9)\n");

			fgets(sQt, sizeof sQt, stdin); // se venisse inserito un numero maggiore di 9 non abbiamo un invio incontrollabile in stdin
			
			qt = atoi(sQt);
		} while(qt > 9);
		printf("Quantity of receiving clients: %s \n", sQt);
		
		
		for(int c = 0; c < qt; c++)
		{
			printf("Insert receiving client id\n");
		
			fgets(destPidB, sizeof destPidB, stdin); // al più 10 cifre per pid ('\n' e '\0')
			
			destPid = atoi(destPidB); 

			printf("Read id: %d\n", destPid);
									// dichiarazione di stringa di appoggio che conterrà il comando in questo formato:
									// "<char comando><pid mittente> <pid destinatario> <messaggio>". Dunque esso avrà 
									// lunghezza pari a quella necessaria alla memorizzazione dei due pid, del comando, 
									// due spazi ed il messaggio							

			int fstWritablePos = getPidLength(destPid) + getPidLength(getpid()) + 3; // 3 poichè 2 spazi + char di comando. fstWritablePos è 
												// l'indice del primo spazio dell'array usato dal messaggio	
			char buf[fstWritablePos + readC];
			
			int written = sprintf(buf, "S%d %d", (int) getpid(), destPid); 
		
			*(buf + written) =  ' ';

			memcpy( (buf + written + 1), msg, readC);
		
			write(cToSFd, buf, sizeof buf); 	
		}

}

/*
 * Stampa codice di identificazione di formato <codice id > contenuto in un file di file descriptor fd. 
 */

void printPid(int fd)               
{
	char tmp[1];
	do
	{
		read(fd, tmp, 1);
		printf("%c", *tmp);
	} while(*tmp != ' ');
}

/*******Definizione handler di segnali********/

void msgReceivedHan(int sig)
{
	
	int fd;

	if((fd = openMyPipe()) < 0) // se minore di 0 -> client non connesso
		return;
	
	printf("Message from client id: ");

	
	printPid(fd);				// stampa id client mittente
	printf(" received. Message:\n");
	char buf[20];				//msg di dim 20 come def in send()
	read(fd, buf, sizeof buf);
	printf("%s\n", buf);
	close(fd);
}

void recNotExisHan(int sig)
{
	int fd;

	if((fd = openMyPipe()) < 0)
		return;
	
	printf("Message not sent; receiver ");
	
	printPid(fd); // stampa id del client destinatario non registrato
	
	printf("  unexisting or not registered\n");
	close(fd);
}

/*
 * Disconnette, ripristina l'handler di default e risolleva il signal che lo ha attivato.
 */

void usrInterHan(int sig)
{
	disconnect();
	signal(sig, oldIntHan);
	raise(sig);
}
