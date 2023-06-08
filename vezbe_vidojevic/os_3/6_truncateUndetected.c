/*
Poziv programa:
./truncateUndetected filePath

Program brise sadrzaj fajla, a zatim menja vreme pristupa i 
modifikacije fajla.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* neophodno zaglavlje za upotrebu funkcije utime(...) */
#include <utime.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do { \
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while(0)
	
static char* osUsage = "./truncateUndetected filePath";

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber);

/* funkcija brise sadrzaj fajla i uklana informaciju o pristupu */
void osTruncate(const char* filePath);

int main(int argc, char** argv) {
	
	osAssert(argc == 2, osUsage);
	
	osTruncate(argv[1]);
	
	exit(EXIT_SUCCESS);
}

/* funkcija brise sadrzaj fajla i uklana informaciju o pristupu */
void osTruncate(const char* filePath) {
	
	/* uz pomoc stata citamo informacije o fajlu */
	struct stat fInfo;
	osAssert(stat(filePath, &fInfo) != -1, "stat failed");
	
	/* brisemo sadrzaj fajla tako sto ga otvorimo u modu za pisanje */
	FILE* f = fopen(filePath, "w");
	osAssert(f != NULL, "fopen failed");
	fclose(f);
	
	/* upisujemo podatke o prethodnom vremenu pristupa i modifikacije
	 * u strukturu utimbuf
	 */ 
	struct utimbuf acTime;
	acTime.actime = fInfo.st_atime;
	acTime.modtime = fInfo.st_mtime;
	
	/* uz pomoc funkcije utime(...) menjamo vreme pristupa i 
	 * modifikacije fajla 
	 */ 
	osAssert(utime(filePath, &acTime) != -1, "utime failed");
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
