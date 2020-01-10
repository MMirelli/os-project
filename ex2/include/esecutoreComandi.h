#define len 1000  // è la massima lunghezza di un comando, data le modeste dimensioni di un usuale comando di shell
		  // non si effettuano controlli in fase di lettura da STDIN

char ** genCommandStr(char buf[len]);

void childExec(char ** cmd, int cmdNum);

void freeMem(char *** cmdBlock, int cmdNum);
