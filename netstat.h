#pragma once
#include "packets.h"
#include "slices.h"

struct BoundPacket {
	BoundPacket(): len(0), remote_port(0), direction(DIR_UNKNOWN), carrier(CARR_UNKNOWN),
				   slice_time(0) {}

	std::string app_name;
	int len;
	uint32_t remote_host;
	int remote_port;
	Direction direction;
	Carrier carrier;
	uint64_t slice_time;
};

struct Remote {
	Remote(): host(0), port(0) {}
    uint32_t host;
	int port;
};

struct RemoteInfo {
	RemoteInfo(): is_ssl(false) {}
	std::string domain;
	std::string whois;
	bool is_ssl;
	std::string ssl;
};

inline bool operator < (const Remote& r1, const Remote& r2) {
	uint64_t r1_ = (((uint64_t)r1.host) << 32) + (uint64_t)r1.port;
	uint64_t r2_ = (((uint64_t)r2.host) << 32) + (uint64_t)r2.port;
	return r1_ < r2_;
}

struct Traffic {
	Traffic(): downloaded(0), uploaded(0), transit(0), internal(0), unknown(0) {}
	int downloaded; // DIR_IN
	int uploaded;   // DIR_OUT
	int transit;    // DIR_FORW
	int internal;   // DIR_LOCAL
	int unknown;    // DIR_UNKNOWN
};

struct AppActivity {
	std::map<Remote, Traffic> remote_traffic;
	std::map<Carrier, Traffic> carrier_traffic;
};

typedef std::vector<BoundPacket> PacketStats;
typedef std::map<std::string, AppActivity> AppsActivity;
typedef std::map<uint32_t, RemoteInfo> RemotesInfo;

void generatePacketStats(
	const Packets& packets, const Slices& slices,
	PacketStats* const pstats);

void generateAppStats(const PacketStats& ps,
	AppsActivity* const ai, RemotesInfo* const ri);

