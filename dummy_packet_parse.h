#include "packets.h"
#include <stdlib.h>

class DummyPacketParser : public PacketParser {
public:
	DummyPacketParser(
		int num, 
		const std::vector<uint32_t>& from_ips,
		const std::vector<int>& from_ports,
		const std::vector<uint32_t>& to_ips,
		const std::vector<int>& to_ports,
		int max_len):
		m_num(num),
		m_from_ips(from_ips), m_from_ports(from_ports),
		m_to_ips(to_ips), m_to_ports(to_ports),
		m_max_len(max_len)
		{
			srandom(0x1234);
		}
	
	virtual std::vector<Packet> parse() {
		std::vector<Packet> ret;
		for (int i = 0; i < m_num; i++) {
			Packet p;
			p.len = (random() % (m_max_len - 1)) + 1;
			p.ip_from = m_from_ips[ random() % m_from_ips.size() ];
			p.port_from = m_from_ports[ random() % m_from_ports.size() ];
			p.ip_to = m_to_ips[ random() % m_to_ips.size() ];
			p.port_to = m_to_ports[ random() % m_to_ports.size() ];
		}
	}

private:
	const int m_num;
	const std::vector<uint32_t> m_from_ips;
	const std::vector<int> m_from_ports;
	const std::vector<uint32_t> m_to_ips;
	const std::vector<int> m_to_ports;
	int m_max_len;
};
