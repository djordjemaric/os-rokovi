#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
/* funkcija otvara fajl u izabranom modu i upisuje fajl deskriptor u prosledjeni pokazivac
 *
 * argumenti funkcije su putanja do fajla, mod u kome treba otvoriti fajl
 * i pokazivac na fajl deskriptor
 */
bool osFileOpen(const char *filePath, const char *mode, int *fd);

int main(int argc, char **argv) {
    osAssert(2 == argc, "Argument missing");
    
    /*
     * Otvaramo fajl u modu za citanje 
     */
    int fd;
    osAssert(osFileOpen(argv[1], "r", &fd), "File open");
    
    /*
     * Funkcija lseek pomera offset fajla. Pomeranjem offseta na kraj,
	 * kao posledicu dobijamo velicinu fajla
	 *
	 * TLPI za detalje
     */
    off_t fileSize = lseek(fd, 0, SEEK_END);
    osAssert(fileSize >= 0, "Lseek failed");
	/* BTINO: svaki otvorani fajl mora da se zatvori */
    close (fd);
    
	/* stampamo velicinu fajla */
    printf("%jd\n", (intmax_t)fileSize);
    return 0;
}
/* funkcija otvara fajl u izabranom modu i upisuje fajl deskriptor u prosledjeni pokazivac
 *
 * argumenti funkcije su putanja do fajla, mod u kome treba otvoriti fajl
 * i pokazivac na fajl deskriptor
 */
bool osFileOpen(const char *filePath, const char *mode, int *fd) {
    /*
     * Privilegije se postavljaju na rw-r--r--
     */
    static mode_t defaultMode = 0644;
    
    /*
	 * Postupno se ukljucuje flagovi za open funkciju
     */
    int flags = 0;
    switch (mode[0]) {
    case 'r':
		/* u slucaju citanja ukljucuje se:
		 * O_RDONLY - samo citanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na pocetku, ako se ne pise po fajlu
		 * 			sadrzaj ce biti nepromenjen
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_RDONLY;
        break;
    case 'w':
		/* u slucaju citanja ukljucuje se:
		 * O_WRONLY - samo pisanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na pocetku, sadrzaj fajla se brise
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_WRONLY;
        flags |= O_TRUNC;	/* flag kojim se brise sadrzaj fajla */
        flags |= O_CREAT;	/* flag kojim se fajl kreira u slucaju da ne postoji */
        break;
    case 'a':
		/* u nadovezivanja citanja ukljucuje se:
		 * O_WRONLY - samo pisanje
		 * O_RDWR - kombinovano citanje i pisanje, fajl offset na kraju
		 */
        flags |= '+' == mode[1] ? O_RDWR : O_WRONLY;
        flags |= O_APPEND;		/* flag kojim se ukljucuje pisanje ISKLJUCIVO na kraj fajla */
        flags |= O_CREAT;		/* flag kojim se fajl kreira u slucaju da ne postoji */
        break;
    default:
        return false;
    }
    
    /*
     * Open sistemskim pozivom se otvara fajl 
	 *
	 * TLPI za detalje
     */
    *fd = open(filePath, flags, defaultMode);
	/* provera gresaka */
    return *fd >= 0;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
