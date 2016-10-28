#pragma once
#include <inttypes.h>
#include <vector>
#include <string>

struct Packet {
	uint64_t abs_time; // in microseconds
	int len; // whole packet length
	uint32_t ip_from;
	int port_from;
	uint32_t ip_to;
	int port_to;
};

typedef std::vector<Packet> Packets;

struct PacketParser {
	virtual std::string parse(Packets* const packets) = 0;
};
