#include "../include/util.h"

/*
 * Calcola il numero di caratteri necessari a rappresentare l'intero in ingresso. 
 */

int getPidLength(int pid)
{
	int rst = 0;
	while(pid > 0)
	{
		rst++;
		pid /= 10;
	}
	return rst;
}
