#define BOOST_TEST_MODULE generate 1 module
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "netstat.h"
using namespace std;

BOOST_AUTO_TEST_CASE(allempty) {
	Packets packets;
	Slices slices;
	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);
	BOOST_CHECK(pstats.empty());
}
