#include "netstat.h"

void generatePacketStats(
	const Packets& packets, const Slices& slices,
	PacketStats* const pstats)
{
	pstats->clear();

	for (Packets::const_iterator it = packets.begin(); it != packets.end(); ++it) {
		const Packet& p = *it;

		
	}
}
