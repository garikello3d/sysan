#include "collector.h"
#include "slices.h"
#include "proc_net_parser.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

bool readFile(const char* name, int max_size, std::string* const contents) {
	FILE* f = fopen(name, "r");
	if (!f) return false;
	contents->resize(max_size);
	int bytes_read = fread(&((*contents)[0]), 1, max_size, f);
	if (bytes_read < 0) {
		contents->clear();
		fclose(f);
		return false;
	}
	else {
		contents->resize(bytes_read);
		fclose(f);
		return true;
	}
}

std::set<int> getSocketInodesInCurrentDir() {
	std::set<int> i;
	
	DIR* dir = opendir(".");
	if (!dir)
		return std::set<int>();
	
	while (struct dirent* dent = readdir(dir)) {
		if (errno != 0) {
			closedir(dir);
			return std::set<int>();
		}

		if (dent->d_type != DT_LNK)
			continue;

		char buf[256];
		ssize_t ssz = readlink(dent->d_name, buf, sizeof(buf));
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

void collectSlice(Slice* const s) {
	DIR* proc = opendir("/proc");
	if (!proc)
		return;

	struct dirent* dent;

	while ((dent = readdir(proc))) {
		if (errno != 0) {
			closedir(proc);
			return;
		}
		if (dent->d_type != DT_DIR)
			continue; // not a directory

		chdir("/proc");
		Slice::App app;
		if (sscanf(dent->d_name, "%d", &app.pid) != 1)
			continue; // not a PID number
		if (chdir(dent->d_name) != 0)
			continue;

		std::string app_name;
		if (!readFile("cmdline", 256, &app_name))
			continue;

		std::string net_tcp; // TODO handle UDP also
		if (!readFile("net/tcp", 16384, &net_tcp))
			continue;

		if (chdir("fd") != 0)
			continue;
		const std::set<int> inodes = getSocketInodesInCurrentDir();

		ProcNetList conn_list;
		parseProcNets(net_tcp, inodes, &conn_list);

		for (ProcNetList::const_iterator it = conn_list.begin();
			 it != conn_list.end(); ++it)
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
}
