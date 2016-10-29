#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "file_packet_parser.h"
#include <arpa/inet.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(parse)

BOOST_AUTO_TEST_CASE(tcp) {
	FilePacketParser p("../../tcp.pcap", 2, 100000);
	Packets packets;
	string err = p.parse(&packets);
	
	BOOST_REQUIRE_EQUAL(packets.size(), 2);
	const Packet p1 = packets[0];
	const Packet p2 = packets[1];
	
	BOOST_REQUIRE_EQUAL(p1.abs_time, 1477748775157770);
	BOOST_CHECK_EQUAL(p1.len, 68);
	BOOST_CHECK_EQUAL(p1.ip_from, ntohl(0xc0a80004));
	BOOST_CHECK_EQUAL(p1.ip_to, ntohl(0x40e9a5c4));
	BOOST_CHECK_EQUAL(p1.port_from, 47698);
	BOOST_CHECK_EQUAL(p1.port_to, 443);
	
	BOOST_REQUIRE_EQUAL(p2.abs_time, 1477748775174340);
	BOOST_CHECK_EQUAL(p2.len, 68);
	BOOST_CHECK_EQUAL(p2.ip_to, ntohl(0xc0a80004));
	BOOST_CHECK_EQUAL(p2.ip_from, ntohl(0x40e9a5c4));
	BOOST_CHECK_EQUAL(p2.port_to, 47698);
	BOOST_CHECK_EQUAL(p2.port_from, 443);
}

BOOST_AUTO_TEST_CASE(udp) {
	FilePacketParser p("../../udp.pcap", 2, 100000);
	Packets packets;
	string err = p.parse(&packets);
	
	BOOST_REQUIRE_EQUAL(packets.size(), 2);
	const Packet p1 = packets[0];
	const Packet p2 = packets[1];
	
	BOOST_REQUIRE_EQUAL(p1.abs_time, 1477750385852900);
	BOOST_CHECK_EQUAL(p1.len, 135);
	BOOST_CHECK_EQUAL(p1.ip_from, ntohl(0xc0a80004));
	BOOST_CHECK_EQUAL(p1.ip_to, ntohl(0x1f9442d6));
	BOOST_CHECK_EQUAL(p1.port_from, 7881);
	BOOST_CHECK_EQUAL(p1.port_to, 6881);
	
	BOOST_REQUIRE_EQUAL(p2.abs_time, 1477750385852990);
	BOOST_CHECK_EQUAL(p2.len, 135);
	BOOST_CHECK_EQUAL(p2.ip_from, ntohl(0xc0a80004));
	BOOST_CHECK_EQUAL(p2.ip_to, ntohl(0xb2148058));
	BOOST_CHECK_EQUAL(p2.port_from, 7881);
	BOOST_CHECK_EQUAL(p2.port_to, 6881);
}

BOOST_AUTO_TEST_CASE(icmp) {
	FilePacketParser p("../../icmp.pcap", 1, 100000);
	Packets packets;
	string err = p.parse(&packets);
	
	BOOST_REQUIRE_EQUAL(packets.size(), 1);
	const Packet p1 = packets[0];
	
	BOOST_REQUIRE_EQUAL(p1.abs_time, 1477751460314250);
	BOOST_CHECK_EQUAL(p1.len, 100);
	BOOST_CHECK_EQUAL(p1.ip_from, ntohl(0xc0a80004));
	BOOST_CHECK_EQUAL(p1.ip_to, ntohl(0xb948e70e));
	BOOST_CHECK_EQUAL(p1.port_from, 0);
	BOOST_CHECK_EQUAL(p1.port_to, 0);
}

BOOST_AUTO_TEST_SUITE_END();
