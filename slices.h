#pragma once
#include <set>
#include <string>
#include <map>

enum Direction { DIR_IN, DIR_OUT, DIR_LOCAL, DIR_FORW, DIR_UNKNOWN };
enum Carrier { CARR_WIFI, CARR_MOBILE, CARR_ETHER, CARR_LOCAL, CARR_UNKNOWN };

struct Slice {
	struct App {
		struct Connection {
			uint32_t ip_local;
			int port_local;
			uint32_t ip_remote;
			int port_remote;
		};

		typedef std::vector<Connection> Connections;
		Connections connections;

		std::set<std::string> files;
		int pid;
	};

	typedef std::map<std::string, App> Apps; // app_name => app info
	Apps apps;

	typedef std::map<uint32_t, Carrier> Interfaces;
	Interfaces ifaces;
};

typedef std::map<uint64_t, Slice> Slices;

struct SliceParser {
	virtual void parse(Slices* const slices) = 0;
};
