/*
Poziv programa:
./a.out noOfThreads

Program poziva 'noOfThreads' niti, pri cemu na pocetku
svaka od njih treba da 'spava' dok ne dobije posao.
Program sa standardnog ulaza cita liniju po liniju,
pri cemu svaka linija sadrzi broj elemenata niza, 
a potom i elemente niza. Kada ih main nit ucita,
potrebno je probuditi jednu proizvoljnu nit,
koja ce izracunati vektor koji se dobija 
kvadriranjem elemenata ulaznog niza. Resenje realizovati
koriscenjem uslovnih promenljivih u kombinaciji sa 
mutex-ima.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <pthread.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
    
#define osPthreadCheck(pthreadErr, userMsg) \
	do { \
		int _pthreadErr = pthreadErr; \
		if (_pthreadErr > 0) { \
			errno = _pthreadErr; \
			osAssert(false, userMsg); \
		} \
	} while (0)
    
static const char *osUsage = "./data_based_sync noOfThreads";

#define MAX_SIZE (32)

/* deljene promenljive */
static int g_dataCount = 0; /* promenljiva oznacava operaciju. 0 - nema ulaza, -1 - ugasiti sve niti, >0 broj bajtova na ulazu */
static int g_Data[MAX_SIZE]; /* niz cuva ucitane brojeve */

/* mutex je moguce inicijalizovati i prilikom kreiranja
 * (nema potrebe za pthread_mutex_init(...))
 */ 
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/* uslovna promenljiva kojom se salju signali nitima */
static pthread_cond_t signaler = PTHREAD_COND_INITIALIZER;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

/* funckija implementira nit */
void* osThreadFunction(void* arg) {
	
	/* index niti
	 * 
	 * BITNO:
	 * obratiti paznju na upotrebu tipa intptr_t kojim se garantuje 
	 * pravilno izvrsavanje programa
	 * 
	 * Samo *(int*) nije dobro, jer moze najcesce pravi problem na 
	 * 64bit masinama u zavisnosti od endianness-a
	 */ 
	int idx = (int)(intptr_t)arg; 
	
	/* u beskonacnoj petlji */
	while (true) {
		
		/* nit pokusava da uhvati mutex */
		osPthreadCheck(pthread_mutex_lock(&lock), "mutex lock failed");
		/* dok god nema nista na ulazu */
		while (g_dataCount == 0) {
			/* cekaj na signalnoj promenljivoj
			 * 
			 * BITNO:
			 * cekanje nije aktivno
			 */ 
			osPthreadCheck(pthread_cond_wait(&signaler, &lock), "cond wait failed");
			printf("Thread %d woke up.\n", idx);
		}
		
		/* ako je na ulazu -1, potrebno je prekinuti sve nit */
		if (g_dataCount == -1) {
			/* otkljucava se mutex */
			osPthreadCheck(pthread_mutex_unlock(&lock), "mutex unlock failed");
			/* iskace se iz petlje */
			break;
		}
		
		/* racunamo pokoordinatne proizvode vektora sa samim sobom */
		printf("Calculating in thread %d\n", idx);
		int i = 0;
		for (i = 0; i < g_dataCount; i++) {
			printf("%d ", g_Data[i]*g_Data[i]);
		}
		printf("\n");
		
		/* resutujemo stanje ulaza */
		g_dataCount = 0;
		
		/* otkljucavamo mutex */
		osPthreadCheck(pthread_mutex_unlock(&lock),"mutex unlock failed");
	}
	
	/* stampamo poruku o zavrsetku rad */
	printf("Thread %d terminated\n", idx);
	
	/* prekidamo nit */
	return NULL;
}

int main(int argc, char** argv) {
	
	osAssert(argc == 2, osUsage);
	
	/* alociramo prostor za promenljive koje cuvaju ID niti */
	int noOfThreads = atoi(argv[1]);
	pthread_t* tids = malloc(noOfThreads * sizeof(pthread_t));
	osAssert(tids != NULL, "allocation failed");
	
	/* startujemo niti */
	int i;
	for (i = 0; i < noOfThreads; i++) {
		
		osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunction, (void*)(intptr_t)i), "pthread create failed");
	}
	
	char *line;
	size_t lineLen = 0;
	printf("no of elements | elements\n");
	/* ucitavamo liniju */
	while (getline(&line, &lineLen, stdin) != -1) {
		
		/* pokusavamo da uhvatimo mutex
		 * 
		 * BITNO:
		 * i "main" tred koristi deljenu promenljivu, pa i on mora da
		 * se takmici sa ostalima da bi joj pristupio
		 */ 
		osPthreadCheck(pthread_mutex_lock(&lock), "mutex lock failed");
		
		int bytesRead = 0;
		int totalBytesRead = 0;
		
		/* parsiramo ucitanu liniju */
		sscanf(line, "%d%n", &g_dataCount, &bytesRead);
		totalBytesRead += bytesRead;
		
		/* ako je na ulazu -1*/
		if (g_dataCount == -1) {
			
			/* otkljucavamo mutex */
			osPthreadCheck(pthread_mutex_unlock(&lock), "mutex unlock failed");
			/* signaliziramo svim tredovima */
			osPthreadCheck(pthread_cond_broadcast(&signaler), "cond broadcast");
			/* iskacemo iz petlje */
			break;
		}
		
		/* popunjavamo vektor */
		for (i = 0; i < g_dataCount; i++) {
			
			sscanf(line + totalBytesRead, "%d%n", &g_Data[i], &bytesRead);
			totalBytesRead += bytesRead;
		}
		
		/* otkljucavamo mutex */
		osPthreadCheck(pthread_mutex_unlock(&lock), "mutex unlock failed");
		/* signlaiziramo jednoj niti (bilo kojoj) */
		osPthreadCheck(pthread_cond_signal(&signaler), "cond signal failed");
	}
	
	/* cekamo da se sve niti izvrse */
	for (i = 0; i < noOfThreads; i++) {
		osPthreadCheck(pthread_join(tids[i], NULL), "join failed");
	}
	
	/* oslobadjamo memoriju */
	free(line);
	free(tids);
	
	/* unistavamo mutex i uslovnu promenljivu */
	osPthreadCheck(pthread_mutex_destroy(&lock), "mutex destroy failed");
	osPthreadCheck(pthread_cond_destroy(&signaler), "cond destroy failed");
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
