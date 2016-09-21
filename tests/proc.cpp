#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "proc_net_parser.h"
using namespace std;

BOOST_AUTO_TEST_SUITE(proc)

BOOST_AUTO_TEST_SUITE(lineparser)

BOOST_AUTO_TEST_CASE(empty) {
	vector<string> lines;
	lineParser(std::string(), &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 0);
}

BOOST_AUTO_TEST_CASE(newline_only) {
	vector<string> lines;
	lineParser("\n", &lines);
	BOOST_REQUIRE(lines.empty());
}

BOOST_AUTO_TEST_CASE(char_and_newline) {
	vector<string> lines;
	lineParser("1\n", &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 1);
	BOOST_CHECK_EQUAL(lines[0], "1");
}

BOOST_AUTO_TEST_CASE(char_only) {
	vector<string> lines;
	lineParser("1", &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 1);
	BOOST_CHECK_EQUAL(lines[0], "1");
}

BOOST_AUTO_TEST_CASE(two_lines_newline) {
	vector<string> lines;
	lineParser("11\n222\n", &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 2);
	BOOST_CHECK_EQUAL(lines[0], "11");
	BOOST_CHECK_EQUAL(lines[1], "222");
}

BOOST_AUTO_TEST_CASE(two_lines_only) {
	vector<string> lines;
	lineParser("11\n222", &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 2);
	BOOST_CHECK_EQUAL(lines[0], "11");
	BOOST_CHECK_EQUAL(lines[1], "222");
}

BOOST_AUTO_TEST_CASE(extra_newlines) {
	vector<string> lines;
	lineParser("11\n222\n\n3333\n\n", &lines);
	BOOST_REQUIRE_EQUAL(lines.size(), 3);
	BOOST_CHECK_EQUAL(lines[0], "11");
	BOOST_CHECK_EQUAL(lines[1], "222");
	BOOST_CHECK_EQUAL(lines[2], "3333");
}

BOOST_AUTO_TEST_SUITE_END() // lineparser

BOOST_AUTO_TEST_SUITE(linesplitter)

BOOST_AUTO_TEST_CASE(empty) {
	vector<string> v;
	lineSplitter("", &v);
	BOOST_REQUIRE_EQUAL(v.size(), 0);
}

BOOST_AUTO_TEST_CASE(one_char) {
	vector<string> v;
	lineSplitter("x", &v);
	BOOST_REQUIRE_EQUAL(v.size(), 1);
	BOOST_CHECK_EQUAL(v[0], "x");
}

BOOST_AUTO_TEST_CASE(two_words) {
	vector<string> v;
	lineSplitter("123 4567", &v);
	BOOST_REQUIRE_EQUAL(v.size(), 2);
	BOOST_CHECK_EQUAL(v[0], "123");
	BOOST_CHECK_EQUAL(v[1], "4567");
}

BOOST_AUTO_TEST_CASE(extra_spaces) {
	vector<string> v;
	lineSplitter(" 123   4567   A \t\tC\t  \tD   ", &v);
	BOOST_REQUIRE_EQUAL(v.size(), 5);
	BOOST_CHECK_EQUAL(v[0], "123");
	BOOST_CHECK_EQUAL(v[1], "4567");
	BOOST_CHECK_EQUAL(v[2], "A");
	BOOST_CHECK_EQUAL(v[3], "C");
	BOOST_CHECK_EQUAL(v[4], "D");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(socketparser)

BOOST_AUTO_TEST_CASE(empty) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(!socketParser("", &ip, &port));
}

BOOST_AUTO_TEST_CASE(no_colon) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(!socketParser("1232A", &ip, &port));
}

BOOST_AUTO_TEST_CASE(bad_ip_len) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(!socketParser("1234567:1234", &ip, &port));
}

BOOST_AUTO_TEST_CASE(bad_both_len) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(!socketParser("1234567:123", &ip, &port));
}

BOOST_AUTO_TEST_CASE(bad_syms) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(!socketParser("123456K8:0123", &ip, &port));
}

BOOST_AUTO_TEST_CASE(good) {
	uint32_t ip = 0; int port = 0;
	BOOST_REQUIRE(socketParser("1234567A:123A", &ip, &port));
	BOOST_CHECK_EQUAL(ip, 0x1234567A);
	BOOST_CHECK_EQUAL(port, 0x123A);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(netparser)

BOOST_AUTO_TEST_CASE(real_proc_net_tcp) {
	string tcp = "\
  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode\n                                                   \
   0: 3500007F:0035 00000000:0000 0A 00000000:00000000 00:00000000 00000000   195        0 11912 1 ffff8800d60c0000 99 0 0 10 0\n                    \
   1: 0100007F:0277 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 13489 1 ffff8802100f0000 99 0 0 10 0\n                    \
   2: 00000000:14EB 00000000:0000 0A 00000000:00000000 00:00000000 00000000   195        0 11914 1 ffff8800d60c07c0 99 0 0 10 0\n                    \
   3: 0600A8C0:C068 33A79A95:01BB 01 00000000:00000000 00:00000000 00000000  1000        0 15042 1 ffff8800d3bd0000 25 3 30 10 -1\n                  \
   4: 0600A8C0:C4A6 D129685F:C8D5 06 00000000:00000000 03:00000BF3 00000000     0        0 0 3 ffff8800c6ef0348\n \
";
	
	ProcNetList list;
	set<int> inodes { 13489, 15042 };
	parseProcNets(tcp, inodes, &list);

	BOOST_REQUIRE_EQUAL(list.size(), 2);
	BOOST_CHECK_EQUAL(list[0].sl, 1);
	BOOST_CHECK_EQUAL(list[0].local_addr, 0x0100007F);
	BOOST_CHECK_EQUAL(list[0].local_port, 0x0277);
	BOOST_CHECK_EQUAL(list[0].remote_addr, 0);
	BOOST_CHECK_EQUAL(list[0].remote_port, 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
