#pragma once
#include "slices.h"
#include <stdlib.h>
#include <algorithm>
#include <set>
#include <map>
#include <vector>

class DummySliceParser : public SliceParser {
public:
	DummySliceParser(
		int nr_slices, int time_per_slice, int max_conn_in_slice,
		const std::set<std::string>& apps,
		const std::vector<uint32_t>& local_ips,
		const std::vector<int>& local_ports,
		const std::vector<uint32_t>& remote_ips,
		const std::vector<int>& remote_ports):
		m_nr_slices(nr_slices), m_time_per_slice(time_per_slice), m_max_conn(max_conn_in_slice),
		m_apps(apps),
		m_local_ips(local_ips), m_local_ports(local_ports),
		m_remote_ips(remote_ips), m_remote_ports(remote_ports)
		{
			srandom(0x5678);
		}
	
	virtual parse(Slices* const slices) {
		for (int i = 0; i < m_nr_slices; i++) {
			Slice s;

			const std::set<std::string> active_apps = reducedRandomShuffle(m_apps, random() % m_apps.size());
			for (active_apps::const_iterator a = active_apps.begin(); a != active_apps.end(); ++a) {
				App app;
				
				app.connections.resize(random() % (m_max_conn-1) + 1);
				for (int j = 0; j < s.connections.size(); j++) {
					App::Connection& c = app.connections[j]; // just for short
					c.ip_local = m_local_ips[ random() % m_local_ips.size() ];
					c.port_local = m_local_ports[ random() % m_local_ports.size() ];
					c.ip_remote = m_remote_ips[ random() % m_remote_ips.size() ];
					c.port_remote = m_remote_ports[ random() % m_remote_ports.size() ];
				}

				s.apps.insert(std::make_pair(*a, app));
			}
			
			slices->insert(std::make_pair(i * m_time_per_slice, s));
		}
	}

private:
	static template<class T> std::set<T> reducedRandomShuffle(const std::set<T>& src, int n) {
		std::vector<T> v;
		std::set<T> ret;
		if (n > src.size()) n = src.size();
		for (typename std::set<T>::const_iterator i = src.begin(); i != src.end(); ++i) {
			v.push_back(*i);
		}
		assert(v.size() == src.size());
		std::random_shuffle(v);
		for (int i = 0; i < n; i++) {
			ret.insert(v[i]);
		}
		return ret;
	}

	const int m_nr_slices;
	const int m_time_per_slice;
	const int m_max_conn;
	const std::set<std::string> m_apps;
	const std::vector<uint32_t> m_local_ips;
	const std::vector<int> m_local_ports;
	const std::vector<uint32_t> m_remote_ips;
	const std::vector<int> m_remote_ports;
};
