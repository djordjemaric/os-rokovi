/*
Poziv programa:
./fifo_client pathToFifo

Program prvo otvara fifo fajl na putanji 'pathToFifo' a potom salje slucajno
generisane brojeve preko tog fajla sve dok korisnik unosi "yes" sa standardnog ulaza.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <time.h>
#include <strings.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do {\
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while (0)

#define MAX_SIZE (20)

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber);

static char* osUsage = "./fifo_client pathToFifo";

int main(int argc, char** argv) {
	
	osAssert(argc == 2, osUsage);
	
	/* otvaranje fifo fajla u suprotnom modu
	 * u odnosu na fifo_server
	 */ 
	int fd = open(argv[1], O_WRONLY);
	osAssert(fd != -1, "open failed");
	
	/* postavljanje sita za generisanje slucajnih brojeva */
	srand(time(NULL));
	char localBuf[MAX_SIZE];
	
	/* sve dok korisnik unosi yes 
	 * salje se novi slucajni broj serveru
	 */ 
	do {
		int broj = rand();
		sprintf(localBuf, "%d", broj);
		printf("Sent number: %d\n", broj);
		
		osAssert(write(fd, localBuf, MAX_SIZE) != -1, "write failed");
		printf("Continue sending: \n");
		scanf("%s", localBuf);
	} while(!strcasecmp("yes", localBuf));
	
	/* BITNO:
	 * zatvaranjem jedne strane fifo fajla
	 * automatski se zatvara i druga strana 
	 */
	close(fd);
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
