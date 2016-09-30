#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "slice_db.h"
#include <sstream>

using namespace std;

BOOST_AUTO_TEST_SUITE(ser)

BOOST_AUTO_TEST_CASE(conn) {
	stringstream ss;
	{
		cereal::BinaryOutputArchive oarchive(ss);
	
		Slice::App::Connection c;
		c.ip_local = 0x10203040;
		c.port_local = 0x5060;
		c.ip_remote = 0xA0B0C0D0;
		c.port_remote = 0xE0F0;
	
		oarchive(c);
	}
	BOOST_REQUIRE(ss.str().size() >= 16);
}

BOOST_AUTO_TEST_CASE(slice) {
	stringstream ss;
	Slice src1, src2, dst1, dst2;
	{
	
		Slice::App app1, app2;
		
		Slice::App::Connection c1, c2, c3;
		Slice::App::Server s1, s2, s3;
		
		c1.ip_local = 0x10101010;
		c1.port_local = 10000;
		c1.ip_remote = 0xA0A0A0A0;
		c1.port_remote = 1000;
		
		c2.ip_local = 0x20202020;
		c2.port_local = 20000;
		c2.ip_remote = 0xB0B0B0B0;
		c2.port_remote = 2000;
		
		c3.ip_local = 0x30303030;
		c3.port_local = 30000;
		c3.ip_remote = 0x30303030;
		c3.port_remote = 3000;
		
		s1.ip_listen = 0x44444444;
		s1.port_listen = 444;

		s2.ip_listen = 0x55555555;
		s2.port_listen = 555;

		s3.ip_listen = 0x66666666;
		s3.port_listen = 666;	
		
		app1.connections.push_back(c1);
		app1.connections.push_back(c2);
		app1.servers.push_back(s1);
		
		app2.connections.push_back(c3);
		app2.servers.push_back(s2);
		app2.servers.push_back(s3);

		src1.apps["app1"] = app1;
		src1.apps["app2"] = app2;
		src1.ifaces = { {0x123, CARR_WIFI}, {0x456, CARR_MOBILE}, {0x789, CARR_ETHER} };

		src2.apps["app3"] = app2;
		src2.apps["app4"] = app1;
		src2.ifaces = { {0x123, CARR_WIFI}, {0x456, CARR_MOBILE}, {0x789, CARR_ETHER} };

		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(src1);
		oarchive(src2);
	}
	{
		cereal::BinaryInputArchive iarchive(ss);
		iarchive(dst1);
		iarchive(dst2);
	}
	BOOST_REQUIRE(src1 == dst1);
	BOOST_REQUIRE(src2 == dst2);
}

BOOST_AUTO_TEST_SUITE_END() // ser
