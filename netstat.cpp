#include "netstat.h"
#include <cassert>

bool checkPacketStrictMatch(const Packet& p, const Slice::App::Connection& c, BoundPacket* const bp) {
	if (
		p.ip_from == c.ip_local && p.port_from == c.port_local &&
		p.ip_to == c.ip_remote && p.port_to == c.port_remote)
	{
		return true;
	}
	else if (
		p.ip_from == c.ip_remote && p.port_from == c.port_remote &&
		p.ip_to == c.ip_local && p.port_to == c.port_local)
	{
		return true;
	}
	else
		return false;
}

void fillPacket(const Packet& p, const Slice::Interfaces& ifaces, BoundPacket* const bp) {
	Carrier carr = CARR_UNKNOWN;
	bool out_matched = false;
	bool in_matched = false;

	Slice::Interfaces::const_iterator ii;

	ii = ifaces.find(p.ip_from);
	if (ii != ifaces.end()) {
		carr = ii->second;
		out_matched = true;
	}

	ii = ifaces.find(p.ip_to);
	if (ii != ifaces.end()) {
		carr = ii->second;
		in_matched = true;
	}

	if (out_matched && !in_matched) {
		bp->remote_host = p.ip_to;
		bp->remote_port = p.port_to;
		bp->direction = DIR_OUT;
		bp->carrier = carr;
	}
	else if (!out_matched && in_matched) {
		bp->remote_host = p.ip_from;
		bp->remote_port = p.port_from;
		bp->direction = DIR_IN;
		bp->carrier = carr;
	}
	else if (out_matched && in_matched) {
		bp->remote_host = p.ip_from; // no matter, equal
		bp->remote_port = p.port_from; // just put here something, don't know any better
		bp->direction = DIR_LOCAL;
		bp->carrier = CARR_LOCAL;
	}
	else {
		bp->remote_host = 0;
		bp->remote_port = 0;
		bp->direction = DIR_FORW;
		bp->carrier = CARR_UNKNOWN;
	}

	bp->len = p.len;
}

bool bindPacket(const Packet& p, const Slice& s, bool check_ports, BoundPacket* const bp) {
	bool found = false;

	if (check_ports) {
		for (Slice::Apps::const_iterator a = s.apps.begin(); a != s.apps.end() && !found; ++a) {
			const std::string& app_name = a->first;
			const Slice::App::Connections& conns = a->second.connections;

			for (Slice::App::Connections::const_iterator c = conns.begin();
				 c != conns.end() && !found; ++c)
			{		
				if (checkPacketStrictMatch(p, *c,  bp)) {
					found = true;
					bp->app_name = app_name;
				}
			}
		}
	}

	fillPacket(p, s.ifaces, bp);

	return found;
}

void generatePacketStats(
	const Packets& packets, const Slices& slices,
	PacketStats* const pstats)
{
	pstats->clear();
	if (slices.empty())
		return;

	for (Packets::const_iterator it = packets.begin(); it != packets.end(); ++it) {
		const Packet& p = *it;

		if (p.abs_time < slices.begin()->first || slices.rbegin()->first < p.abs_time)
			continue;

		std::vector<Slices::const_iterator> candidates;
		Slices::const_iterator next_slice = slices.lower_bound(p.abs_time);
		
		if (next_slice->first == p.abs_time) { // precise hit
			candidates.push_back(next_slice);
		}
		else { // need to choose
			Slices::const_iterator prev_slice = next_slice;
			std::advance(prev_slice, -1);
			
			uint64_t distance_from_prev = p.abs_time - prev_slice->first;
			uint64_t distance_to_next = next_slice->first - p.abs_time;
			assert(distance_from_prev >= 0);
			assert(distance_to_next >= 0);
			assert(distance_to_next + distance_from_prev > 0);

			if (distance_from_prev < distance_to_next) {
				candidates.push_back(prev_slice);
				candidates.push_back(next_slice);
			} else {
				candidates.push_back(next_slice);
				candidates.push_back(prev_slice);
			}
		}
		assert(!candidates.empty());

		bool bound = false;
		BoundPacket bp;
		
		for (std::vector<Slices::const_iterator>::const_iterator c = candidates.begin();
			 c != candidates.end() && !bound; ++c) 
		{
			const Slices::const_iterator& iter = *c;			
			bound = bindPacket(p, iter->second, true, &bp);
			if (bound)
				bp.slice_time = iter->first;
		}

		if (!bound) { // no strict match found on any candidate, perform soft match on first one
			bindPacket(p, candidates[0]->second, false, &bp);
			bp.slice_time = candidates[0]->first;
		}

		pstats->push_back(bp);
	}
}
