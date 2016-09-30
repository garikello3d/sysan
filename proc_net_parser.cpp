#include "proc_net_parser.h"
#include <cstring>
#include <algorithm>
#include <cassert>

void lineParser(const std::string& buf, std::vector<std::string>* const lines) {
	lines->clear();
	const char* p = buf.data(), *prev_p = p;
	while (p && p < buf.data() + buf.size()) {
		p = strchr(prev_p, '\n');
		if (p && p > prev_p) {
			lines->push_back(std::string(prev_p, p));
		}
		if (prev_p && !p) {
			if (buf.data() + buf.size() - prev_p > 0) // don't create empty elements
				lines->push_back(std::string(prev_p, buf.data() + buf.size()));
			break;
		}
		++p;
		prev_p = p;
	}
}

void lineSplitter(const std::string& buf, std::vector<std::string>* const items) {
	items->clear();
	const char* p = buf.data();
	std::string item;
	while (p < buf.data() + buf.size()) {
		if (*p != ' ' && *p != '\n' && *p != '\t')
			item += *p;
		else {
			if (!item.empty())
				items->push_back(item);
			item.clear();
		}
		++p;
	}
	if (!item.empty())
		items->push_back(item);
}

bool socketParser(const char* buf, bool emu4as6, uint32_t* const ip, int* const port) {
	const char* colon = strchr(buf, ':');
	if (!colon) return false;
	if (!emu4as6) {
		if (colon - buf != 8) return false;
		return sscanf(buf, "%08X:%04X", ip, port) == 2;
	}
	else {
		if (colon - buf != 32) return false;
		uint8_t ipv6[32];
		if (sscanf(buf, "%32c:%04X", ipv6, port) != 2)
			return false;
		uint32_t ipv4 = tryMap6To4(ipv6);
		if (ipv4) {
			*ip = ipv4;
			return true;
		}
		else
			return false;
	}
}

uint32_t tryMap6To4(const uint8_t ipv6[32]) {
	if (memcmp(ipv6, "0000000000000000FFFF0000", 24))
		return 0;
	uint32_t ret = 0;
	char tail[9] = {0}; // need to leave extra \0 to make sscanf happy
	memcpy(tail, &ipv6[24], 8);
	if (sscanf(tail, "%08X", &ret) != 1)
		return false;
	else
		return ret;
}

void parseProcNets4_internal(const std::string& buf, const std::set<int>& inodes, bool emu4as6, ProcNetList4* const list) {
	std::vector<std::string> lines;
	std::vector<std::string> parts;
	lineParser(buf, &lines);
	int inode_index = -1;

	for (std::vector<std::string>::const_iterator L = lines.begin();
		 L != lines.end(); ++L)
	{
		//printf(">>> line \"%s\"\n", L->c_str());
		lineSplitter(*L, &parts);
		if (parts.size() < 10)
			continue;
		if (inode_index == -1) { // this is header line
			std::vector<std::string>::iterator pi = std::find(parts.begin(), parts.end(), "inode");
			if (pi == parts.end())
				return; // no inode column, do nothing
			inode_index = std::distance(parts.begin(), pi) - 2;
			continue;
		}

		ProcNet4 pn;

		assert(inode_index < (int)parts.size() && inode_index > 0);
		if (sscanf(parts[inode_index].c_str(), "%d", &pn.inode) != 1)
			continue;

		//printf("-- search inode %d...", pn.inode);
		const std::set<int>::const_iterator ii = inodes.find(pn.inode);
		if (ii == inodes.end())
			continue;
	
		if (sscanf(parts[0].c_str(), "%d:", &pn.sl) != 1)
			continue;

		if (!socketParser(parts[1].c_str(), emu4as6, &pn.local_addr, &pn.local_port))
			continue;

		if (!socketParser(parts[2].c_str(), emu4as6, &pn.remote_addr, &pn.remote_port))
			continue;

		if (sscanf(parts[3].c_str(), "%d", &pn.uid) != 1)
			continue;

		list->push_back(pn);
	}
}

void parseProcNets4(const std::string& buf, const std::set<int>& inodes, ProcNetList4* const list) {
}

void parseProcNets6(const std::string& buf, const std::set<int>& inodes, ProcNetList4* const list4as6, ProcNetList6* const list6) {
}
