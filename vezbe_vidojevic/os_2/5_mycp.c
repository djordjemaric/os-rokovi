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
/* funkcija kopira sadrzaj fajla na putanji filePathSrc u fajl na putanji
 * filePathDest
 */
bool osCopyFilePath(const char *filePathDest, const char *filePathSrc);
/* funkcija kopira sadrzaj fajla sa fajl deskriptorom fdSrc u fajl sa fajl
 * deskriptorom fdDest
 */
bool osCopyFileFd(const int fdDest, const int fdSrc);

int main(int argc, char **argv) {
    osAssert(3 == argc, "Argument missing, usage: ./cpfile source destination");
    osAssert(osCopyFilePath(argv[2], argv[1]), "File write to console");
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

/* funkcija kopira sadrzaj fajla na putanji filePathSrc u fajl na putanji
 * filePathDest
 */
bool osCopyFilePath(const char *filePathDest, const char *filePathSrc) {
    /*
     * DEklarisemo promenljive za fajl deskriptore
     */
    int fdDest, fdSrc;
	/* otvaramo fajlove u odgovorajucim modovima i proveravamo greske */
    if (!osFileOpen(filePathDest, "w", &fdDest) || !osFileOpen(filePathSrc, "r", &fdSrc))
        return false;
    
    /*
     * sadrzaj fajla sa deskriptorom fdSrc se kopira u fajl da deskriptorom fdDest
     */
    bool result = osCopyFileFd(fdDest, fdSrc);
	/* cuva se errno koji su postavili sistemski pozivi u funkciji osCopyFilePath */
    int saveErrno = errno;
	/* zatvaraju se deskriptori, sto za posledicu ima postavljanje errno na 0,
	 * tj. brisanje eventualnih gresaka koje su se desile u read i write sistemskim pozivima 
	 */
    close(fdDest);
    close(fdSrc);
    
    /*
     * Vracanje errno vrednosti koje su postavili read i write sistemski pozivi
     */
    errno = saveErrno;
	/* provera gresaka */
    return result;
}
/* funkcija kopira sadrzaj fajla sa fajl deskriptorom fdSrc u fajl sa fajl
 * deskriptorom fdDest
 */
bool osCopyFileFd(const int fdDest, const int fdSrc) {
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
    while ((readBytes = read(fdSrc, memBuf, memBufSize)) > 0) {
		/* ono sto smo procitali sa jednog fd-a stampamo na drugi fd
		 *
		 * BITNO:
		 * uvek mozete da upisete u fajl najvise onoliko bajtova koliko ste procitali. Dakle, argument funkcije write
		 * moze da bude samo readBytes, nikako memBufSize
 		 */
        if(write(fdDest, memBuf, readBytes) < 0) {
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
