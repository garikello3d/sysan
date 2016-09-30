#pragma once
#include <string>
#include <vector>
#include <set>

// represents /proc/net/tcp or /proc/net/udp
// TODO other protocols (tcp6, udp6, icmp, icmp6 etc)
template <typename AddrType> struct AbstractProcNet {
	int sl;
	AddrType local_addr;
	int local_port;
	AddrType remote_addr;
	int remote_port;
	int state;
	int uid;
	int inode;
};

typedef AbstractProcNet<uint32_t> ProcNet4;
typedef AbstractProcNet<uint8_t[16]> ProcNet6;

void lineSplitter(const std::string& buf, std::vector<std::string>* const items);
void lineParser(const std::string& buf, std::vector<std::string>* const lines);
bool socketParser(const char* buf, bool emu4as6, uint32_t* const ip, int* const port);
uint32_t tryMap6To4(const uint8_t ipv6[32]);

typedef std::vector<ProcNet4> ProcNetList4;
typedef std::vector<ProcNet6> ProcNetList6;
void parseProcNets4(const std::string& buf, const std::set<int>& inodes, ProcNetList4* const list);
void parseProcNets6(const std::string& buf, const std::set<int>& inodes, ProcNetList4* const list4as6, ProcNetList6* const list6);
// TODO void parseProcNets6(const std::string& buf, const std::set<int>& inodes, ProcNetList6* const list);
