#include "sniffer.h"
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

const int DURATION = 60;

int main() {
	puts("Started");
	JustStoreObserver ob;
	Sniffer* w = new Sniffer(&ob, "any", "/data/any.pcap");
	if (w->spawnUntilStopOrError()) {
		printf("Spawned capture thread for %d seconds\n", DURATION);
		sleep(DURATION);
	}
	else
		puts("Error initializing capture");
	w->stopAndDeinit();
	delete w;
	printf("Captured %d packets\n", (int)w->nrPackets());
	printf("Errors was: '%s'\n", ob.err.c_str());
	return 0;
}
