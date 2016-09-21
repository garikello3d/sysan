#pragma once
#include <string>
#include <vector>
#include <set>

// represents /proc/net/tcp or /proc/net/udp
// TODO other protocols (tcp6, udp6, icmp, icmp6 etc)
struct ProcNet {
	int sl;
	uint32_t local_addr;
	int local_port;
	uint32_t remote_addr;
	int remote_port;
	int state;
	int uid;
	int inode;
};

void lineSplitter(const std::string& buf, std::vector<std::string>* const items);
void lineParser(const std::string& buf, std::vector<std::string>* const lines);
bool socketParser(const char* buf, uint32_t* const ip, int* const port);

typedef std::vector<ProcNet> ProcNetList;
void parseProcNets(const std::string& buf, const std::set<int>& inodes, ProcNetList* const list);
