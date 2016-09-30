#include "utility.h"

static inline const char* carrier2str(Carrier c) {
	switch (c) {
		case CARR_WIFI: return "wifi";
		case CARR_MOBILE: return "mobile";
		case CARR_ETHER: return "ethernet";
		case CARR_LOCAL: return "loopback";
		default: return "unknown";
	}
}

std::string dumpInterfaces(const Slice::Interfaces ifaces) {
	std::string ret("Available interfaces: ");
	for (Slice::Interfaces::const_iterator i = ifaces.begin(); i != ifaces.end(); ++i) {
		char ip[32];
		in_addr ina;
		
		ina.s_addr = i->first;
		strncpy(ip, inet_ntoa(ina), sizeof(ip));

		char tmp[256];
		snprintf(tmp, sizeof(tmp), "%s(%s) ", ip, carrier2str(i->second));

		ret.append(tmp);
	}
	return ret;
}

void enumerateAppsAndConns() {
	Slice s;
	collectSlice(&s);
	printf("%d apps:\n", (int)s.apps.size());
	for (Slice::Apps::const_iterator aa = s.apps.begin();
		 aa != s.apps.end(); ++aa)
	{
		const Slice::App& a = aa->second;
		if (a.connections.empty() && a.servers.empty())
			continue;

		printf("\t%-32s / %d:\n", aa->first.c_str(), aa->second.pid);

		for (Slice::App::Connections::const_iterator cc = a.connections.begin();
			 cc != a.connections.end(); ++cc)
		{
			char ip1[32], ip2[32];
			in_addr ina;
			
			ina.s_addr = cc->ip_local;
			strncpy(ip1, inet_ntoa(ina), sizeof(ip1));

			ina.s_addr = cc->ip_remote;
			strncpy(ip2, inet_ntoa(ina), sizeof(ip2));
					
			printf("\t\t%s:%d -> %s:%d\n", ip1, cc->port_local, ip2, cc->port_remote);
		}

		for (Slice::App::Servers::const_iterator ss = a.servers.begin();
			 ss != a.servers.end(); ++ss)
		{
			char ip[32];
			in_addr ina;
			
			ina.s_addr = ss->ip_listen;
			strncpy(ip, inet_ntoa(ina), sizeof(ip));

			printf("\t\t%s:%d\n", ip, ss->port_listen);
		}
	}
	puts(dumpInterfaces(s.ifaces).c_str());
}
