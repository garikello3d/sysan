#define BOOST_TEST_MODULE generate 1 module
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "netstat.h"
using namespace std;

struct S {
	S() {
		Slice::App app1, app2, app3, app4;
		
		Slice::App::Connection c1, c2, c3, c4;
		
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
		
		c4.ip_local = 0x40404040;
		c4.port_local = 40000;
		c4.ip_remote = 0xD0D0D0D0;
		c4.port_remote = 4000;
		
		app1.connections.push_back(c1);
		app2.connections.push_back(c2);
		app3.connections.push_back(c3);
		app4.connections.push_back(c4);

		Slice s1;
		s1.apps["a1"] = app1;
		s1.apps["b1"] = app2;
		s1.apps["c1"] = app3;
		s1.apps["d1"] = app4;
		s1.ifaces = { {0x10101010, CARR_WIFI}, {0x20202020, CARR_MOBILE}, {0x30303030, CARR_ETHER} };

		Slice s2;
		s2.apps["a2"] = app1;
		s2.apps["b2"] = app2;
		s2.apps["c2"] = app3;
		s2.apps["d2"] = app4;
		s2.ifaces = { {0x10101010, CARR_WIFI}, {0x20202020, CARR_MOBILE}, {0x30303030, CARR_ETHER} };

		Slice s3;
		s3.apps["a3"] = app1;
		s3.apps["b3"] = app2;
		s3.apps["c3"] = app3;
		s3.apps["d3"] = app4;
		s3.ifaces = { {0x10101010, CARR_WIFI}, {0x20202020, CARR_MOBILE}, {0x30303030, CARR_ETHER} };

		slices[100] = s1;
		slices[200] = s2;
		slices[300] = s3;
	}

	Slices slices;
};

BOOST_AUTO_TEST_CASE(allempty) {
	Packets packets;
	Slices slices;
	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);
	BOOST_CHECK(pstats.empty());
}

BOOST_FIXTURE_TEST_CASE(no_proper_slice_lower, S) {
	Packet p;
	Packets packets;
	
	p.abs_time = 50;
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(no_proper_slice_upper, S) {
	Packet p;
	Packets packets;

	p.abs_time = 350;
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(hit_s1_app1_out, S) {
	Packet p;
	Packets packets;

	p.abs_time = 100;
	p.len = 5000;
	p.ip_from = 0x10101010;
	p.port_from = 10000;
	p.ip_to = 0xA0A0A0A0;
	p.port_to = 1000;
	
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 1);
	const BoundPacket& bp = pstats[0];

	BOOST_CHECK_EQUAL(bp.app_name, "a1");
	BOOST_CHECK_EQUAL(bp.len, 5000);
	BOOST_CHECK_EQUAL(bp.remote_host, 0xA0A0A0A0);
	BOOST_CHECK_EQUAL(bp.remote_port, 1000);
	BOOST_CHECK_EQUAL(bp.direction, DIR_OUT);
	BOOST_CHECK_EQUAL(bp.carrier, CARR_WIFI);
	BOOST_CHECK_EQUAL(bp.slice_time, 100);
}

BOOST_FIXTURE_TEST_CASE(goes_s1_app1_in, S) {
	Packet p;
	Packets packets;

	p.abs_time = 101;
	p.len = 6000;
	p.ip_from = 0xA0A0A0A0;
	p.port_from = 1000;
	p.ip_to = 0x10101010;
	p.port_to = 10000;
	
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 1);
	const BoundPacket& bp = pstats[0];

	BOOST_CHECK_EQUAL(bp.app_name, "a1");
	BOOST_CHECK_EQUAL(bp.len, 6000);
	BOOST_CHECK_EQUAL(bp.remote_host, 0xA0A0A0A0);
	BOOST_CHECK_EQUAL(bp.remote_port, 1000);
	BOOST_CHECK_EQUAL(bp.direction, DIR_IN);
	BOOST_CHECK_EQUAL(bp.carrier, CARR_WIFI);
	BOOST_CHECK_EQUAL(bp.slice_time, 100);
}

BOOST_FIXTURE_TEST_CASE(goes_s2_app3_local, S) {
	Packet p;
	Packets packets;

	p.abs_time = 199;
	p.len = 7000;
	p.ip_from = 0x30303030;
	p.port_from = 30000;
	p.ip_to = 0x30303030;
	p.port_to = 3000;
	
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 1);
	const BoundPacket& bp = pstats[0];

	BOOST_CHECK_EQUAL(bp.app_name, "c2");
	BOOST_CHECK_EQUAL(bp.len, 7000);
	BOOST_CHECK_EQUAL(bp.remote_host, 0x30303030);
	BOOST_CHECK_EQUAL(bp.remote_port, 30000);
	BOOST_CHECK_EQUAL(bp.direction, DIR_LOCAL);
	BOOST_CHECK_EQUAL(bp.carrier, CARR_LOCAL);
	BOOST_CHECK_EQUAL(bp.slice_time, 200);
}

BOOST_FIXTURE_TEST_CASE(goes_s3_app_unknown_out, S) {
	Packet p;
	Packets packets;

	p.abs_time = 256;
	p.len = 8000;
	p.ip_from = 0x10101010;
	p.port_from = 1234;
	p.ip_to = 0xAAAAAAAA;
	p.port_to = 4567;
	
	packets.push_back(p);

	PacketStats pstats;
	generatePacketStats(packets, slices, &pstats);

	BOOST_CHECK_EQUAL(pstats.size(), 1);
	const BoundPacket& bp = pstats[0];

	BOOST_CHECK(bp.app_name.empty());
	BOOST_CHECK_EQUAL(bp.len, 8000);
	BOOST_CHECK_EQUAL(bp.remote_host, 0xAAAAAAAA);
	BOOST_CHECK_EQUAL(bp.remote_port, 4567);
	BOOST_CHECK_EQUAL(bp.direction, DIR_OUT);
	BOOST_CHECK_EQUAL(bp.carrier, CARR_WIFI);
	BOOST_CHECK_EQUAL(bp.slice_time, 300);
}
