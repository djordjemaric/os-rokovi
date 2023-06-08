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

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
/* funkcija otvara fajl u izabranom modu i upisuje fajl deskriptor u prosledjeni pokazivac
 *
 * argumenti funkcije su putanja do fajla, mod u kome treba otvoriti fajl
 * i pokazivac na fajl deskriptor
 */
bool osFileOpen(const char *filePath, const char *mode, int *fd);
/* funkcija kopira sadrzaj fajla sa fajl deskriptorom fd na stdout*/
bool osCopyToTerminal(const int fd);

int main(int argc, char **argv) {
    osAssert(2 == argc, "Argument missing");
    
    int fd;
    osAssert(osFileOpen(argv[1], "r", &fd), "File create");
    osAssert(osCopyToTerminal(fd), "File write to console");
    close(fd);
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
/* funkcija kopira sadrzaj fajla sa fajl deskriptorom fd na stdout*/
bool osCopyToTerminal(int fd) {
    /*
     * Alocira se bafer za citanje fajla
     */
    static const uint32_t memBufSize = 1U << 13; // 8KB
    char *memBuf = malloc(memBufSize);
	/* BITNO: obrada gresaka je neophodna prilikom svakog alociranja */
    if (NULL == memBuf)
        return false;
    
    /* promenljiva koja cuva broj ucitanih bajtova */
    int32_t readBytes;
	/* sve dok mozemo da ucitamo nesto iz fajla */
    while ((readBytes = read(fd, memBuf, memBufSize)) > 0) {
		/* ono sto smo procitali stampamo na stdout
		 *
		 * BITNO: kada se koriste stdout, stdin i stderr u kombinaciji sa read i write, uvek morate da koristite
		 * 		sistemski definisane fajl deskriptore nad kojima su otvoreni tokovi
		 *
		 * stdout -> STDOUT_FILENO
		 * stderr -> STDERR_FILENO
		 * stdin  -> STDIN_FILENO
		 *
		 * Nikada ne smete da koristite brojeve umesto sistemski definisanih konstanti, jer to zavisi od implementacije
		 *
		 * BITNO 2:
		 * uvek mozete da upisete u fajl najvise onoliko bajtova koliko ste procitali. Dakle, argument funkcije write
		 * moze da bude samo readBytes, nikako memBufSize
 		 */
        if(write(STDOUT_FILENO, memBuf, readBytes) < 0) {
            free(memBuf);
            return false;
        }
    }
	/* BITNO: sve sto se alocira, mora i da se oslobodi */
    free(memBuf);
	/* provera gresaka */
    return 0 == readBytes;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
