/*
Program demonstrira kreiranje novih procesa. Nakon kreiranja,
roditelj treba da ispise svoj PID i PID svog deteta, a dete
svoj PID i PID svog roditelja. Nakon toga, oba procesa ispisuju istu poruku.
DOdatno, dete poziva exit da bi zavrsilo izvrsavanje, dok roditelj ceka 
da dete zavrsi svoj rad.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/wait.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do { \
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while(0)
	
void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber);

int main(int argc, char** argv) {
	
	pid_t childPid = fork();
	
	osAssert(childPid != -1, "fork failed");
	
	if (childPid > 0) { // parent branch
		
		printf("Hello from parent. My pid is %jd, child pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)childPid);
		
		/* neophodno je sacekati da se child zavrsi, jer se u 
		 * suprotnom seju zombi procesi koji bespotrebno zagusuju 
		 * sistem
		 * 
		 * BITNO:
		 * nakon svakog forka, neophodno je sacekati da se dete zavrsi
		 */ 		
		osAssert(wait(NULL) != -1, "wait failed");
	}
	else { // child branch
		
		//sleep(5);
		printf("Hello from child. My pid is %jd, parent pid is %jd\n", \
				(intmax_t)getpid(), (intmax_t)getppid());
		exit(EXIT_SUCCESS);
	}
	
	printf("Only parent executes this\n");
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
