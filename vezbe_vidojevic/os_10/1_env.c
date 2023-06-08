/*
Program demonstrira rad sa promenljivama okruzenja.
Program se poziva u formatima:
./a.out + name value
./a.out - name
./a.out name

U slucaju da je argv[1] znak +, potrebno je dodati promenljivu sa imenom 'name'
u okruzenje, pri cemu je odgovarajuca vrednost 'value'. U slucaju da je argv[1] znak -
potrebno je izbaciti promenljivu sa imenom 'name' iz okruzenja. Ako nije zadat ni znak +
ni znak -, potrebno je ispisati vrednost promenljive sa imenom 'name'.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define check_error(cond,userMsg)\
	do {\
		if (!(cond)) { \
			perror(userMsg);\
			exit(EXIT_FAILURE);\
		}\
	} while(0)
	
extern char** environ;

void print_env(void) {
	
	char **ep;
	
	for (ep = environ; *ep != NULL; ep++) {
		
		printf("%s\n", *ep);
	}
}
	
int main(int argc, char** argv) {
	
	//print_env();
	
	check_error(argc >= 2, "...");
	
	if (argv[1][0] == '+') {
		
		check_error(setenv(argv[2], argv[3], 0) != -1, "...");
		
		char* value = getenv(argv[2]);
		check_error(value != NULL, "...");
		
		printf("%s\n",value);
	}
	else if (argv[1][0] == '-') {
		
		check_error(unsetenv(argv[2]) != -1, "...");
		
		char* value = getenv(argv[2]);
		
		printf("%s\n", value == NULL ? "nema" : "ima");
		
	}
	else {
		
		char* value = getenv(argv[1]);
		check_error(value != NULL, "....");
		
		printf("%s\n", value);
	}
	
	exit(EXIT_SUCCESS);
}
