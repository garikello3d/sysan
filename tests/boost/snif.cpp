#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "sniffer.h"

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

BOOST_AUTO_TEST_SUITE(snif)

BOOST_AUTO_TEST_CASE(no_perm) {
	JustStoreObserver ob;
	Worker* w = new Sniffer(&ob, "eth0", "/tmp/packets.pcap");
	BOOST_REQUIRE(!w->spawnUntilStopOrError());
	w->stopAndDeinit();
	delete w;
	BOOST_REQUIRE(!ob.err.empty());
	printf("pcap error was: %s\n", ob.err.c_str());
}

BOOST_AUTO_TEST_CASE(capture_by_root) {
	JustStoreObserver ob;
	Sniffer* w = new Sniffer(&ob, "any", "/tmp/packets.pcap");
	BOOST_REQUIRE(w->spawnUntilStopOrError());
	sleep(3);
	w->stopAndDeinit();
	delete w;
	BOOST_REQUIRE(ob.err.empty());
	BOOST_CHECK(w->nrPackets() > 0);
}

BOOST_AUTO_TEST_CASE(nothing_by_root) {
	JustStoreObserver ob;
	Sniffer* w = new Sniffer(&ob, "eth6", "/tmp/packets.pcap");
	BOOST_REQUIRE(w->spawnUntilStopOrError());
	sleep(2);
	w->stopAndDeinit();
	delete w;
	BOOST_REQUIRE(ob.err.empty());
	BOOST_CHECK_EQUAL(w->nrPackets(), 0);
}

BOOST_AUTO_TEST_SUITE_END();
