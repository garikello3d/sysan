#pragma once
#include <set>
#include <string>
#include <map>

enum Direction { IN, OUT, FORWARD };
enum Carrier { WIFI, MOBILE, ETHER, LOCAL, UNKNOWN };

struct Slice {
	struct App {
		struct Connection {
			uint32_t ip_local;
			int port_local;
			uint32_t ip_remote;
			int port_remote;
		} connections;

		std::set<std::string> files;
		int pid;
	};

	typedef std::map<std::string, App> Apps;
	Apps apps;
	std::map<uint32_t, Carrier> ifaces;
};

typedef std::map<uint64_t, Slice> Slices;

struct SliceParser {
	virtual void parse(Slices* const slices) = 0;
};
