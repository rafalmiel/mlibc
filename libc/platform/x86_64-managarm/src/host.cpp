
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#pragma GCC visibility pop

int gethostname(char *buffer, size_t max_length) {
	const char *name = "cradle";
	frigg::infoLogger.log() << "mlibc: Broken gethostbyname() called!" << frigg::EndLog();
	strncpy(buffer, name, max_length);
	return 0;
}

static passwd theEntry;

struct passwd *getpwuid(uid_t) {
	frigg::infoLogger.log() << "mlibc: Broken getpwuid() called!" << frigg::EndLog();
	theEntry.pw_name = "root";
	theEntry.pw_uid = 0;
	theEntry.pw_gid = 0;
	theEntry.pw_dir = "/root";
	theEntry.pw_shell = "/bin/sh";
	return &theEntry;
}

void endpwent(void) {

}

