#pragma once
#include "packets.h"
#include "slices.h"

struct BoundPacket {
	std::string app_name;
	int len;
	uint32_t remote_host;
	int remote_ports;
	Direction direction;
	Carrier carrier;
};

typedef std::vector<BoundPacket> PacketStats;

void generatePacketStats(
	const Packets& packets, const Slices& slices,
	PacketStats* const pstats);
