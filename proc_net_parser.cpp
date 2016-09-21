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

bool socketParser(const char* buf, uint32_t* const ip, int* const port) {
	const char* colon = strchr(buf, ':');
	if (!colon) return false;
	if (colon - buf != 8) return false;
	return sscanf(buf, "%08X:%04X", ip, port) == 2;
}

void parseProcNets(const std::string& buf, const std::set<int>& inodes, ProcNetList* const list) {
	std::vector<std::string> lines;
	std::vector<std::string> parts;
	lineParser(buf, &lines);
	int inode_index = -1;

	for (std::vector<std::string>::const_iterator L = lines.begin();
		 L != lines.end(); ++L)
	{
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

		ProcNet pn;

		assert(inode_index < (int)parts.size() && inode_index > 0);
		if (sscanf(parts[inode_index].c_str(), "%d", &pn.inode) != 1)
			continue;

		const std::set<int>::const_iterator ii = inodes.find(pn.inode);
		if (ii == inodes.end())
			continue;
	
		if (sscanf(parts[0].c_str(), "%d:", &pn.sl) != 1)
			continue;

		if (!socketParser(parts[1].c_str(), &pn.local_addr, &pn.local_port))
			continue;

		if (!socketParser(parts[2].c_str(), &pn.remote_addr, &pn.remote_port))
			continue;

		if (sscanf(parts[3].c_str(), "%d", &pn.uid) != 1)
			continue;

		list->push_back(pn);
	}
}
