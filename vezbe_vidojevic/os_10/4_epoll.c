/*
Program demonstrira nacin za multipleksiranje fajlova putem 
interfejsa epoll. Kao argumenti komandne linije,
zadaju se putanje do fifo fajlova. 
Potrebno je sacekati na podatke u tim fajlovima, 
pri cemu se ne zna unapred kada ce i na kom fajlu pristici podaci. 
Poruke koje stizu potrebno je ispisati na standardni izlaz. Program
se zaustavlja kada programi koji su otvorili fifo fajlove za pisanje 
zatvore svoj kraj.
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <sys/epoll.h>
#include <string.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
    
static const char *os_Usage = "./epoll pathToFile1 pathToFile2 pathToFIle3...";
    
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

#define MAX_SIZE 	(256)
#define MAX_EVENTS 	(8)
#define WAIT_TIME	(-1)

int main(int argc, char** argv) {
	
	osAssert(argc >= 2, os_Usage);
	
	/* kreiramo epoll_instancu */
	unsigned int numFifos = argc-1;
	int epollFd = epoll_create(numFifos);
	osAssert(epollFd != -1, "epoll_create failed");
	
	struct epoll_event currentEvent;
	int i = 0;
	/* punimo instancu dogadjajima koje zelimo da monitorisemo */
	for (i = 0; i < numFifos; i++) {
		/* brisemo sadrzaj strukture */
		memset(&currentEvent, 0, sizeof(struct epoll_event));
		/* postavljamo operaciju koju monitorisemo */
		currentEvent.events = EPOLLIN;
		/* pamtimo fajl deskriptor fajla
		 * 
		 * BITNO:
		 * fajl deskriptor mora biti otvoren u neblokirajucem rezimu (O_NONBLOCK),
		 * inace ne postoji potreba za multipleksiranjem
		 */ 
		currentEvent.data.fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		osAssert(currentEvent.data.fd != -1, "open failed");
		
		/* u epoll instancu upisujemo informacije sadrzane u strukturi 
		 * current event 
		 */ 
		osAssert(epoll_ctl(epollFd, EPOLL_CTL_ADD, currentEvent.data.fd, &currentEvent) != -1, "epoll_ctl failed");
	}
	
	struct epoll_event readyFds[MAX_EVENTS];
	/* dok god imamo neki fifo u epoll instanci */
	while (numFifos) {
		int numReadyEvents;
		/* cekamo da se desi neka aktivnost */
		osAssert((numReadyEvents = epoll_wait(epollFd, readyFds, MAX_EVENTS, WAIT_TIME)) != -1, "epoll_wait failed") ;
		
		char buf[MAX_SIZE];
		/* prolazimo kroz niz PROBUDJENIH deskriptora, dakle ne kroz sve
		 * deskriptore 
		 */
		for (i = 0; i < numReadyEvents; i++) {
			
			/* ako je ulaz uzrok budjenja */
			if (readyFds[i].events & EPOLLIN) {
				
				int bytesRead;
				/* citamo sadrzaj poruke */
				osAssert((bytesRead = read(readyFds[i].data.fd, buf, MAX_SIZE)) != -1, "read failed");
				/* terminisemo bafer */
				buf[bytesRead] = 0;
				/* stampamo poruku */
				printf("FIFO %d sent %s\n",i, buf);
			}
			/* ako je uzrok budjenja neka greska ili zatvaranje druge strane fifo-a*/
			else if (readyFds[i].events & (EPOLLHUP | EPOLLERR)) {
				
				/* stampamo poruku */
				printf("FIFO %d closed.\n", i);
				/* zatvaramo fajl deskriptor, cime ga i izbacujemo iz 
				 * epoll instance
				 */ 
				close(readyFds[i].data.fd);
				/* umanjujemo brojac */
				numFifos--;
			}
		}
	}
	
	/* zatvaramo epoll instancu */
	close(epollFd);
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
