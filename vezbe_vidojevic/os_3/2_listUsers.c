/*
Program koji ispisuje informacije o svim korisnicima na sistemu.
*/
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <pwd.h>

/* makro pomocu koga mozemo da prevarimo kompajler da ne prijavljuje greske
 * za neiskoriscene promenljive
 */
#define UNUSED(x) ((void)x)
#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do { \
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while (0)
	
static char* osUsage = "./listUsers";
	
void osErrorFatalImpl(const char* userMsg, const char* fileName,\
						const char* functionName, const int lineNumber);
					
/* funkcija lista passwd fajl */
void osListAllUsers(FILE* outStream);	
/* funkcija stampa sadrzaj passwd strukture za trenutnog korisnika */
bool osPrintUserInfo(const struct passwd* userInfo, FILE* outStream);				
						
int main(int argc, char** argv) {
	
	UNUSED(osUsage);
	
	osListAllUsers(stdout);
	
	exit(EXIT_SUCCESS);
}

/* funkcija lista passwd fajl */
void osListAllUsers(FILE* outStream) {
	
	struct passwd* currentUser;
	/* prilikom citanja sadrzaja passwd fajla neophodno je na pocetku
	 * premotati ceo fajl na pocetak pozivom funkcije setpwent()
	 * 
	 * Nema potrebe obradjivati greske za setpwent jer poziv ne puca
	 */
	setpwent();
	
	/* passwd fajl se iterativno cita liniju po liniju pomocu funkcije
	 * getpwent()
	 * 
	 * Funkcija vraca NULL ako dodje do kraja fajla ili ako se dogodi greska.
	 * Za proveru gresaka mora da se koristi errno promenljiva
	 * 
	 * TLPI za detalje
	 */ 
	while((currentUser = getpwent()) != NULL) {
		
		/* stampamo informacije o korisniku */
		osAssert(osPrintUserInfo(currentUser, outStream), "Printing user failed");
	}
	
	/* Nakon sto se zavrsi upotreba passwd fajl MORA da se pravilno
	 * zatvori pozivom funkcije endpwent()
	 */ 
	endpwent();
	
	/* TLPI za detalje */
}

/* funkcija stampa sadrzaj passwd strukture za trenutnog korisnika */
bool osPrintUserInfo(const struct passwd* userInfo, FILE* outStream){
	
	/* stampanje informacija o korisniku */
	fprintf(outStream, "\n##################################\n");
	fprintf(outStream, "User name: %s\n", userInfo->pw_name);
	fprintf(outStream, "Numerical user id: %jd\n", (intmax_t)userInfo->pw_uid);
	fprintf(outStream, "Numeriacl group id: %jd\n", (intmax_t)userInfo->pw_gid);
	fprintf(outStream, "Comment: %s\n", userInfo->pw_gecos);
	fprintf(outStream, "Home directory: %s\n", userInfo->pw_dir);
	fprintf(outStream, "Shell: %s\n", userInfo->pw_shell);
	
	return true;
}

void osErrorFatalImpl(const char* userMsg, const char* fileName,\
						const char* functionName, const int lineNumber) {
							
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", fileName, functionName, lineNumber);
						
	exit(EXIT_FAILURE);	
}
