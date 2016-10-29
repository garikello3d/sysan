#pragma once
#include <set>
#include <string>
#include <map>
#include <vector>

enum Direction { DIR_IN, DIR_OUT, DIR_LOCAL, DIR_FORW, DIR_UNKNOWN };
enum Carrier { CARR_WIFI, CARR_MOBILE, CARR_ETHER, CARR_LOCAL, CARR_UNKNOWN };

struct Slice {
	struct App {
		App(): pid(0) {}
		
		struct Connection {
			Connection(): ip_local(0), port_local(0), ip_remote(0), port_remote(0) {}
			uint32_t ip_local;
			int port_local;
			uint32_t ip_remote;
			int port_remote;
		};

		struct Server {
			Server(): ip_listen(0), port_listen(0) {}
			uint32_t ip_listen;
			int port_listen;
		};

		typedef std::vector<Connection> Connections;
		Connections connections;

		typedef std::vector<Server> Servers;
		Servers servers;

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
	virtual bool parse(Slices* const slices) = 0;
};
