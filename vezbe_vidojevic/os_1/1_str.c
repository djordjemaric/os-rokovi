/*
Primer cestih gresaka na ispitu: neinicijalizovani pokazivac
i string bez terminirajuce nule. 
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	
	char* s;
	s = malloc(50);
	
	char t[50];
	
	t[0] = 'a';
	t[1] = 'b';
	t[2] = 'c';
	//t[3] = 0;
	//t[3] = '\0';
	
	
	//scanf("%s", s);
	
	printf("%s\n", t);
	
	exit(EXIT_SUCCESS);
}
