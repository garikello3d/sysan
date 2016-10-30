#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "query.h"
#include <arpa/inet.h>
using namespace std;

std::string getDomainByIpStr(const char* ip_str) {
	in_addr ina;
	inet_aton(ip_str, &ina);
	return getDomainByIp(ina.s_addr);
}

std::string getDomainByIpStrOnline(const char* ip_str) {
	in_addr ina;
	inet_aton(ip_str, &ina);
	return getDomainByIpOnline(ina.s_addr);
}

std::string getSslSubjectStr(const char* ip_str) {
	in_addr ina;
	inet_aton(ip_str, &ina);
	return getSslSubject(ina.s_addr);
}

BOOST_AUTO_TEST_SUITE(query)

BOOST_AUTO_TEST_CASE(valid_host) {
	BOOST_CHECK_EQUAL(getDomainByIpStr("217.69.139.199"), "ms.mail.ru");
}

BOOST_AUTO_TEST_CASE(invalid_host) {
	BOOST_CHECK_EQUAL(getDomainByIpStr("1.2.3.4"), "");
}

BOOST_AUTO_TEST_CASE(aliases) {
	BOOST_CHECK_EQUAL(getDomainByIpStrOnline("173.194.222.95"),
		"lo-in-f95.1e100.net");
}

BOOST_AUTO_TEST_CASE(sslcert) {
	BOOST_CHECK_EQUAL(getSslSubjectStr("216.58.209.100"),
		"www.google.com");
}

BOOST_AUTO_TEST_SUITE_END()
