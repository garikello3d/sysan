#include "query.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
using namespace std;

string ipToStr(uint32_t ip) {
	if (ip == 0)
		return "?\t";
	char s[32];
	in_addr ina;
	ina.s_addr = ip;
	strncpy(s, inet_ntoa(ina), sizeof(s));
	return string(s);
}

string getDomainByIp(uint32_t ip) {
	in_addr ina;
	ina.s_addr = ip;
	hostent* he = gethostbyaddr(reinterpret_cast<void*>(&ina), sizeof(ina), AF_INET);
	if (!he)
		return std::string();
	return he->h_name;
}

string getDomainByIpCmd(const std::string& cmd) {
	FILE* f = popen(cmd.c_str(), "r");
	if (!f) return string();
	char* line = NULL;
	size_t len = 0;
	ssize_t rd;
	char tail[1024] = {0};
	bool found = false;
	static const char MARK[] = "domain name pointer";
	while ((rd = getline(&line, &len, f)) != -1) {
		const char* t = strstr(line, MARK);
		if (t) {
			int n = snprintf(tail, sizeof(tail), "%s", t + sizeof(MARK));
			tail[n-1] = 0; // chop the trailing \n
			if (tail[n-2] == '.')
				tail[n-2] = 0; // chop the annoying dot
			found = true;
			break;
		}
	}
	pclose(f);
	if (found) {
		string ret(tail);
		free(line);
		return ret;
	}
	else {
		free(line);
		return string();
	}
}

string getDomainByIpOnline(uint32_t ip) {
	char cmd1[1024], cmd2[1024];
	const char* ip_str = ipToStr(ip).c_str();
	snprintf(cmd1, sizeof(cmd1), "host -t ptr %s", ip_str);
	snprintf(cmd2, sizeof(cmd2), "host -t ptr %s 8.8.8.8", ip_str);
	const string name1 = getDomainByIpCmd(cmd1);
	const string name2 = getDomainByIpCmd(cmd2);
	if (name1.empty() || name2.empty())
		return string();
	if (name2.size() > name1.size())
		return name2;
	else
		return name1;
}

string getSslSubject(uint32_t ip) {
	const char* ip_str = ipToStr(ip).c_str();
	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "openssl s_client -connect %s:443 2>&1 1>/dev/null </dev/null", ip_str);
	FILE* f = popen(cmd, "r");
	if (!f) return string();
	char* line = NULL;
	size_t len = 0;
	ssize_t rd;
	char CN[64] = {0};
	bool found = false;
	static const char MARK_DEPTH[] = "depth=0";
	static const char MARK_CN[] = "CN = ";
	while ((rd = getline(&line, &len, f)) != -1) {
		const char* t = strstr(line, MARK_DEPTH);
		if (t == line) {
			const char* cn = strstr(line, MARK_CN);
			if (cn) {
				int n = snprintf(CN, sizeof(CN), "%s", cn + sizeof(MARK_CN) - 1);
				CN[n - 1] = 0;
				found = true;
				break;
			}
		}
	}
	pclose(f);
	if (found) {
		string ret(CN);
		free(line);
		return ret;
	}
	else {
		free(line);
		return string();
	}
}
