#include <signal.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/errno.h>
#include <stdarg.h>

#include "net_collector.h"
#include "sniffer.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

void tolog(bool debugOrError, const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
#ifdef __ANDROID__
	__android_log_vprint(debugOrError ? ANDROID_LOG_DEBUG : ANDROID_LOG_ERROR, "SysAnalCollector", fmt, va);
#else
	vprintf(fmt, va);
	putchar('\n');
#endif	
	va_end(va);
}

volatile int exit_flag = 0;
const char* SOCK_NAME = "org.igor.sysanal.socket";
const char* UPLOAD_DIR =  "/storage/sdcard0/Download/";

struct AnyWorkerObserver : public WorkerObserver {
	AnyWorkerObserver() {}
	virtual void workerError(const std::string& msg) {
		tolog(true, "worker error '%s', exiting", msg.c_str());
		exit_flag = 1;
	}
};

enum { SNIFFER_IDX = 0, NETSTAT_IDX = 1, MAX_IDX = 2 };
std::vector<Worker*> workers(MAX_IDX);
AnyWorkerObserver observer;

static void handler(int signum) {
	tolog(true, "Signal %d", signum);
	exit_flag = 1;
}

void install(int signum) {
	struct sigaction sa;
	
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(signum, &sa, NULL) == -1)
		tolog(false, "error installing handler %d", signum);
	else
		tolog(true, "signal %d handler installed", signum);
}

int main(int argc, const char** const argv) {
	tolog(true, "backend start");
	install(SIGINT);
	install(SIGTERM);
	install(SIGKILL);
	install(SIGUSR1);

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		tolog(false, "could not create socket: %s", strerror(errno));
		return 1;
	}

	struct sockaddr_un dst;
	memset(&dst, 0, sizeof(dst));
	dst.sun_family = AF_UNIX;
	strncpy(&dst.sun_path[1], SOCK_NAME, sizeof(dst.sun_path) - 2);
	const int dst_len = sizeof(dst.sun_family) + strlen(SOCK_NAME) + 1;

	if (connect(sock, reinterpret_cast<struct sockaddr*>(&dst), dst_len) < 0) {
		tolog(false, "could not connect to %s: %s",
			&dst.sun_path[1], strerror(errno));
		close(sock);
		return 2;
	}

	workers[SNIFFER_IDX] = new Sniffer(&observer, "any",  std::string(UPLOAD_DIR) +  "dump.anal");
	workers[NETSTAT_IDX] = new NetCollector(&observer, 1, std::string(UPLOAD_DIR) +  "net.anal");

	tolog(true, "starting all workers");
	bool failed = false;
	for (Worker* w : workers) {
		if (!w->spawnUntilStopOrError())
			failed = true;
	}
	tolog(true, "listening to socket events");

	if (!failed) {
		fd_set rfds;
		struct timeval tv;
	
		while (!exit_flag) {
			tolog(true, "(working)");

			FD_ZERO(&rfds);
			FD_SET(sock, &rfds);
			tv.tv_sec = 1;
			tv.tv_usec = 0;
		
			int ret = select(sock + 1, &rfds, NULL, NULL, &tv);
			if (ret == -1) {
				tolog(false, "select error: %s", strerror(errno));
				break;
			}
			else if (ret == 1) {
				if (FD_ISSET(sock, &rfds)) {
					tolog(true, "read event");
					char byte;
					int br = read(sock, &byte, 1);
					if (br == 0) {
						tolog(true, "peer disconnected");
						break;
					}
					else if (br > 9)
						tolog(true, "got %d bytes from peer", br);
					else
						tolog(false, "error reading from peer: %s", strerror(errno));
				}
			}
		}

		close(sock);
		tolog(true, "socket closed");
	}

	tolog(false, "stopping all workers");
	for (Worker* w : workers) {
		w->stopAndDeinit();
		delete w;
	}

	tolog(true, "backend stop");
	return 0;
}
