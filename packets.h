#pragma once
#include <inttypes.h>
#include <vector>

struct Packet {
	uint64_t abs_time;
	int len; // including link level (?)
	uint32_t ip_from;
	int port_from;
	uint32_t ip_to;
	int port_to;
};

typedef std::vector<Packet> Packets;

struct PacketParser {
	virtual void parse(Packets* const packets) = 0;
};
