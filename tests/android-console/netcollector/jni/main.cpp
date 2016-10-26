#include "net_collector.h"
#include <unistd.h>

using namespace std;

struct JustStoreObserver : public WorkerObserver {
	JustStoreObserver() {}
	virtual void workerError(const std::string& msg) {
		assert(err.empty());
		printf("workerError: %s\n", msg.c_str());
		err.assign(msg);
	}
	string err;
};

const int DURATION = 100;
const int EVERY = 1;

int main() {
	puts("Started");
	JustStoreObserver ob;
	NetCollector* n = new NetCollector(&ob, EVERY, "/storage/sdcard1/Download/netstat");
	if (n->spawnUntilStopOrError()) {
		printf("Spawned netstat thread for %d seconds\n", DURATION);
		sleep(DURATION);
	}
	else
		puts("Error initializing netstat");
	n->stopAndDeinit();
	delete n;
	printf("Errors was: '%s'\n", ob.err.c_str());
	return 0;
}
