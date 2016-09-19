#pragma once
#include "packets.h"
#include "slices.h"

struct BoundPacket {
	BoundPacket(): len(0), remote_port(0), direction(DIR_UNKNOWN), carrier(CARR_UNKNOWN) {}

	std::string app_name;
	int len;
	uint32_t remote_host;
	int remote_port;
	Direction direction;
	Carrier carrier;
};

typedef std::vector<BoundPacket> PacketStats;

void generatePacketStats(
	const Packets& packets, const Slices& slices,
	PacketStats* const pstats);
