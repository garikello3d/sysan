#pragma once

struct Packet {
	uint64_t abs_time;
	int len; // including link level (?)
	uint32_t ip_from;
	int port_from;
	uint32_t ip_to;
	int port_to;
};

struct PacketParser {
	virtual std::vector<Packet> parse() = 0;
};
