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

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
    
static const char *os_Usage = "./2 pathToFifo";
    
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);


int main(int argc, char** argv) {
	
	osAssert(argc == 2, os_Usage);
	
	/* otvorimo fifo sa fopen
	 * najlakse je, jer time izbegavamo gimnasticenje sa 
	 * parsiranjem stringa, a i u tekstu ne pise da je zabranjena
	 * upotreba stdio :)
	 */ 
	FILE* f = fopen(argv[1], "r");
	
	osAssert(f != NULL, "fopen failed");
	
	float sum = 0;
	float number = 0;
	
	/* dok uspesno konvertujemo vrednosti */
	while (fscanf(f, "%f", &number) == 1) {
		
		/* dodajemo procitanu vrednost na sumu */
		sum += number;
	}
	
	/* ako nismo stigli do kraja fajla znaci da smo naisli na
	 * nesto sto nije broj 
	 */ 
	errno = EINVAL;
	osAssert(feof(f), "Not a number");
	
	/* stampamo rezultat */
	printf("%f\n", sum);
	
	/* zatvaramo fajl */
	fclose(f);
	
	exit(EXIT_SUCCESS);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
