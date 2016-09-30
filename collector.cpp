#include "collector.h"
#include "slices.h"
#include "proc_net_parser.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <cstring>

bool readFile(const std::string& name, std::string* const contents) {
	FILE* f = fopen(name.c_str(), "r");
	if (!f) return false;

	char buf[1024];
	contents->reserve(sizeof(buf));
	bool ok = true;
	
	for (;;) {
		int bytes_read = fread(buf, 1, sizeof(buf), f);
		if (bytes_read > 0)
			contents->append(buf, bytes_read);
		else if (bytes_read == 0)
			break;
		else { // < 0
			ok = false;
			break;
		}
	}
	fclose(f);

	if (!ok)
		contents->clear();

	return ok;
}

std::set<int> getSocketInodes(const std::string& fd_path) {
	std::set<int> i;
	
	DIR* dir = opendir(fd_path.c_str());
	if (!dir)
		return std::set<int>();
	
	while (struct dirent* dent = readdir(dir)) {
		if (!dent && errno != 0) {
			closedir(dir);
			return std::set<int>();
		}

		if (dent->d_type != DT_LNK)
			continue;

		char buf[256] = {0};
		ssize_t ssz = readlink((std::string(fd_path) + "/" + dent->d_name).c_str(), buf, sizeof(buf));
		if (ssz <= 0)
			continue;

		int inode = 0;
		if (sscanf(buf, "socket:[%d]", &inode) != 1)
			continue;

		i.insert(inode);
	}

	closedir(dir);
	return i;
}

bool getSystemInterfaces(Slice::Interfaces* const ifaces) {
	struct ifaddrs *ifaddr, *ifa;
	int n;

	if (getifaddrs(&ifaddr) == -1)
		return false;

	ifaces->clear();

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
			continue;
		
		if (ifa->ifa_addr->sa_family == AF_INET) {
			const char* ifname = ifa->ifa_name;
			Carrier car = CARR_UNKNOWN;

			if (strstr(ifname, "eth"))
				car = CARR_ETHER;
			else if (strstr(ifname, "lo"))
				car = CARR_LOCAL;
			else if (strstr(ifname, "wlan"))
				car = CARR_WIFI;
			else if (strstr(ifname, "rmnet"))
				car = CARR_MOBILE;

			const in_addr_t saddr = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
			ifaces->insert(std::make_pair(saddr, car));
		}
	}

	freeifaddrs(ifaddr);

	return true;
}

void collectSlice(Slice* const s) {
	DIR* proc = opendir("/proc");
	if (!proc)
		return;

	struct dirent* dent;

	while ((dent = readdir(proc))) {
		if (!dent && errno != 0) {
			closedir(proc);
			return;
		}
		if (dent->d_type != DT_DIR)
			continue; // not a directory

		Slice::App app;
		if (sscanf(dent->d_name, "%d", &app.pid) != 1)
			continue; // not a PID number

		std::string base_dir = std::string("/proc/") + dent->d_name + "/";
		std::string app_name;
		if (!readFile(base_dir + "cmdline", &app_name))
			continue;

		//printf("app %s:\n", app_name.c_str());

		std::string net_tcp4, net_tcp6; // TODO handle UDP also
		if (!readFile(base_dir + "net/tcp", &net_tcp4))
			continue;
		if (!readFile(base_dir + "net/tcp6", &net_tcp6))
			continue;

		const std::set<int> inodes = getSocketInodes(base_dir + "fd");
		// printf(" %d inodes: \n", (int)inodes.size());
		//for (std::set<int>::const_iterator ino = inodes.begin(); ino != inodes.end(); ++ino)
		//	printf("%d ", *ino);
		//putchar('\n');

		ProcNetList4 conn_list4, conn_list4_as_6;
		ProcNetList6 conn_list6;
		parseProcNets4(net_tcp4, inodes, &conn_list4);
		parseProcNets6(net_tcp6, inodes, &conn_list4_as_6, &conn_list6);

		conn_list4.insert(conn_list4.end(), conn_list4_as_6.begin(), conn_list4_as_6.end());
		// TODO use IPv6 list also
		//printf(" %d conns\n", (int)conn_list.size());

		for (ProcNetList4::const_iterator it = conn_list4.begin();
			 it != conn_list4.end(); ++it)
		{
			if (it->remote_port != 0 && it->remote_addr != 0) {
				Slice::App::Connection c;
				c.ip_local = it->local_addr;
				c.port_local = it->local_port;
				c.ip_remote = it->remote_addr;
				c.port_remote = it->remote_port;
				app.connections.push_back(c);
			} else {
				Slice::App::Server s;
				s.ip_listen = it->local_addr;
				s.port_listen = it->local_port;
				app.servers.push_back(s);
			}
		}

		s->apps.insert(std::make_pair(app_name, app));
	}

	closedir(proc);

	if (!getSystemInterfaces(&s->ifaces))
		s->apps.clear();
}
