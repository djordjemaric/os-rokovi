/*
Poziv programa:
./userInfo userName

Program treba da ispise informacije o korisniku 'userName',
pri cemu se informacije dobijaju iz passwd fajla.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* neophodno zaglavlje prilikom rada sa funkcijama 
 * koje citaju passwd fajl
 * 
 * man 5 passwd za detelje o passwd fajlu
 */
#include <pwd.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
	do { \
		if (!(expr)) \
			osErrorFatal(userMsg); \
	} while (0)


static char* osUsage = "./userInfo userName";

void osErrorFatalImpl(const char* userMsg, const char* fileName, \
						const char* functionName, const int lineNumber);

/* funkcija ispisuje informacije o korisniku iz passwd fajla */
bool osGetUserInfo(const char* userName, FILE* outStream);

int main(int argc, char** argv) {
	
	osAssert(argc == 2, osUsage);
	osAssert(osGetUserInfo(argv[1], stdout), "get user info failed");
	
	exit(EXIT_SUCCESS);
}

/* funkcija ispisuje informacije o korisniku iz passwd fajla */
bool osGetUserInfo(const char* userName, FILE* outStream) {
	
	/* citamo informacije iz passwd fajla o korisniku sa
	 * korisnickim imenom userName
	 * funkcija getpwnam vraca pokazivac na staticki alociranu strukturu
	 * TLPI za detalje
	 * 
	 * BITNO: strukturu nikada ne smete eksplicitno da oslobadjate
	 */ 
	struct passwd* userInfo = getpwnam(userName);
	/* u slucaju greske ili nepostojanja podataka u passwd fajlu o 
	 * korisniku sa imenom userName funkcija getpwnam vraca NULL
	 * i postavlja errno na odgovarajucu vrednost 
	 */
	if (userInfo == NULL)
		return false;
	
	/* ispisujemo informacije koje postoje u passwd  fajlu korisniku */
	fprintf(outStream, "User name: %s\n", userInfo->pw_name);
	fprintf(outStream, "Numerical user id: %jd\n", (intmax_t)userInfo->pw_uid);
	fprintf(outStream, "Numerical group id: %jd\n", (intmax_t)userInfo->pw_gid);
	fprintf(outStream, "Comment: %s\n", userInfo->pw_gecos);
	fprintf(outStream, "Home directory: %s\n", userInfo->pw_dir);
	fprintf(outStream, "Shell: %s\n", userInfo->pw_shell);
	
	return true;
}

void osErrorFatalImpl(const char* userMsg, const char* fileName, \
						const char* functionName, const int lineNumber) {
	
	perror(userMsg);
	
	fprintf(stderr, "File name: %s\nFunction name: %s\nLine number: %d\n", \
					fileName, functionName, lineNumber);
	
	exit(EXIT_FAILURE);
}
