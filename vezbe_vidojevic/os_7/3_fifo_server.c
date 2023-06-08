/*
./fifo_client pathToFifo

Program prvo kreira fifo fajl na putanji 'pathToFifo' a potom cita brojeve
koje klijent salje preko tog fajla i ispisuje ih na standardni izlaz.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do {\
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while (0)

#define MAX_SIZE (20)	

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber);

static char* osUsage = "./fifo_server pathToFifo";

int main(int argc, char** argv) {
	
	osAssert(argc == 2, osUsage);
	
	/* pravljenje fifo fajla
	 * ovakav poziv bi pukao ukoliko vec postoji fajl na putanji argv[1]
	 * 
	 * BITNO:
	 * samo jedna strana u komunikaciji pravi fifo. druga strana samo 
	 * otvara fifo fajl u suprotnom modu
	 */
	//osAssert(mkfifo(argv[1], 0600) != -1, "mkfifo failed");
	
	/* ako je potrebno da se obradi provera postojanja fifo fajla na putanji 
	 * jedno od mogucih resenja je sledece
	 */
	int result = mkfifo(argv[1], 0600);
	if (result == -1) {
		
		/* ako uzrok pucanja nije postojanja fajla vec nesto drugo
		 * neophodno je prekinuti izvrsavanje
		 */ 
		osAssert(errno == EEXIST, "mkfifo failed");
		
		/* provera da li se radi o fifo fajlu.
		 * ako nije fifo, potrebno je prekinuti izvrsavanje
		 */ 
		struct stat fInfo;
		osAssert(stat(argv[1], &fInfo) != -1, "stat failed");
		osAssert(S_ISFIFO(fInfo.st_mode), "Not a fifo.");
	}
	
	/* otvaranje fifo fajla je blokirajuca operacija
	 * 
	 * Proces ostaje blokiran u open pozivu sve dok neki drugi
	 * proces ne otvori fifo fajl u suprotnom modu 
	 */ 
	int fd = open(argv[1], O_RDONLY);
	osAssert(fd != -1, "open failed");
	 
	char buf[MAX_SIZE];
	int readBytes = 0;
	
	/* dok god je otvoren drugi kraj fifo fajla 
	 * redom se citaju brojevi koje generise klijent 
	 */
	while ((readBytes = read(fd, buf, MAX_SIZE)) > 0) {
		
		int broj;
		sscanf(buf, "%d", &broj);
		
		printf("Received number: '%d'\n", broj);
	}
	
	/* provera da li je read uspesno zavrsen */
	osAssert(readBytes != -1, "read failed");
	
	/* zatvaranje fajla */
	close(fd);
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
