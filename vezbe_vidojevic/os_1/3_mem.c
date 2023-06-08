#include <stdio.h>
#include <stdlib.h>

#define INIT_SIZE (8)
#define STEP 	(2)

#define checkError(cond,userMsg)\
	do {\
		if (!(cond)) {\
			fprintf(stderr,"%s\n",userMsg);\
			exit(EXIT_FAILURE);\
		}\
	} while (0)

void error(int cond, char* s) ;

int main(int argc, char** argv) {
	
	int* niz;
	int n = 0;
	int capacity = INIT_SIZE;
	
	niz = malloc(capacity*sizeof(int));
	checkError(niz != NULL, "Alokacija nije uspela");
	
	/*
	do {\
		if (!(niz != NULL)) {\
			fprintf(stderr,"%s\n","Alokacija nije uspela");\
			exit(EXIT_FAILURE);\
		}\
	} while (0)
	*/
	
	//error(niz != NULL, "ALokacija nije uspela");
	/*
	if (niz == NULL) {
		fprintf(stderr,"Alokacija nije uspela\n");
		exit(EXIT_FAILURE);		
	}
	*/
	
	while (1) {
		
		int x;
		scanf("%d", &x);
		
		if (x == 0) {
			break;
		}		
		
		niz[n++] = x;
		
		if (n == capacity) {
			
			capacity *= STEP;
			int* noviNiz = realloc(niz, capacity*sizeof(int));
			checkError(noviNiz != NULL, "Realokacije nije uspela");
			
			//error(noviNiz != NULL, "Realokacija nije uspela");
			
			/*
			if (noviNiz == NULL) {
				
				free(niz);
				fprintf(stderr, "Realokacija nije uspela\n");
				exit(EXIT_FAILURE);
			}
			*/
			
			niz = noviNiz;
		}
	}
	
	for (int i = 0; i < n; i++) {
		
		printf("%d ", niz[i]);
	}
	printf("\n");
	
	
	free(niz);
	
	exit(EXIT_SUCCESS);
}

void error(int cond, char* s) {
	
	if (!cond) {
		
		fprintf(stderr,"%s\n", s);
		exit(EXIT_FAILURE);
	}
}
