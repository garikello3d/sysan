#include "slice_db.h"
#include <signal.h>
#include <unistd.h>
#include <android/log.h>

//void handler(int sig, int code, struct sigcontext *scp, char *addr);

static void handler(int signum) {
	__android_log_print(ANDROID_LOG_DEBUG, "Collector","signal %d", signum);
}

void install(int signum) {
	struct sigaction sa;
	
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(signum, &sa, NULL) == -1)
		__android_log_print(ANDROID_LOG_DEBUG, "Collector","error installing %d", signum);
	else
		__android_log_print(ANDROID_LOG_DEBUG, "Collector","%d handler installed", signum);
}

int main() {
	install(SIGINT);
	install(SIGTERM);
	install(SIGTERM);
	install(SIGKILL);

	sleep(3);
	
	return 0;
}
