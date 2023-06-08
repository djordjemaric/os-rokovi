/*
Primer definisanja makroa u vise redova 
(znak '\' omogucava pisanje u vise redova).
*/

#include <stdio.h>
#include <stdlib.h>

/*
#define proizvod(a,b)\
		int rez=((a)*(b));\
		printf("%d\n",rez);
*/

#define proizvod(a,b)\
	do {\
		int rez=((a)*(b));\
		printf("%d\n",rez);\
	} while (0)

int main(int argc, char** argv) {
	
	int x = 10;
	int y = 5;
	
	//int rez = proizvod(x+y,x-y);
	//int rez = ((x+y)*(x-y)); zbog ovoga moraju da stoje zagrade u makrou
	
	//printf("%d\n", rez);
	
	//bez do while ograde, makro proizvod uzrokuje gresku
	if (x > 5) 
		proizvod(x,y);	
	
	exit(EXIT_SUCCESS);
}
