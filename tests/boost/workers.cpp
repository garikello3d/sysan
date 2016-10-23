#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "worker.h"

using namespace std;

struct JustStoreObserver : public WorkerObserver {
	JustStoreObserver() {}
	virtual void workerError(const std::string& msg) {
		assert(err.empty());
		err.assign(msg);
	}
	string err;
};

class DummyWorker : public Worker {
public:
	DummyWorker(WorkerObserver* const obs, int dur, const char* err, bool init_ok):
		Worker(obs), duration(dur), stopFlag(false), init_good(init_ok), resource(NULL)
	{
		if (err) errMsg.assign(err);
	}

	~DummyWorker() { assert(!resource); }

	const int duration;
	string errMsg;
	volatile bool stopFlag;
	const bool init_good;
	
protected:
	virtual bool init() {
		puts("initialize");
		if (init_good)
			resource = new int(10);
		return init_good;
	}
	
	virtual void deinit() {
		puts("deinitialize");
		if(resource) {
			delete resource;
			resource = NULL;
		}
	}
	
	virtual void runUntilStopOrError() {
		puts("thread is running");
		assert(resource);
		for (int i = 0; i < duration; i++) {
			if (stopFlag) {
				puts("stop flag");
				break;
			}
			if (!errMsg.empty() && i == duration / 2 - 1) {
				puts("some error");
				signalError(errMsg);
				sleep(1);
				break;
			}
			else
				sleep(1);
		}
		puts("thread complete");
	}
	
	virtual void flagStop() { stopFlag = true; }

private:
	int* resource;
};

BOOST_AUTO_TEST_SUITE(workers)

BOOST_AUTO_TEST_CASE(not_run) {
	JustStoreObserver ob;
	Worker* w = new DummyWorker(&ob, 0, NULL, true);
	delete w;
}

BOOST_AUTO_TEST_CASE(terminate_no_error) {
	JustStoreObserver ob;
	Worker* w = new DummyWorker(&ob, 4, NULL, true);
	BOOST_REQUIRE(w->spawnUntilStopOrError());
	sleep(1);
	w->stopAndDeinit();
	delete w;
	BOOST_REQUIRE(ob.err.empty());
}

BOOST_AUTO_TEST_CASE(error_occured) {
	JustStoreObserver ob;
	Worker* w = new DummyWorker(&ob, 4, "fuck", true);
	BOOST_REQUIRE(w->spawnUntilStopOrError());
	sleep(3);
	BOOST_REQUIRE(ob.err == "fuck");
	w->stopAndDeinit();
	delete w;
}

BOOST_AUTO_TEST_CASE(init_error) {
	JustStoreObserver ob;
	Worker* w = new DummyWorker(&ob, 4, "fuck", false);
	BOOST_REQUIRE(!w->spawnUntilStopOrError());
	w->stopAndDeinit();
	delete w;
}

BOOST_AUTO_TEST_SUITE_END();


