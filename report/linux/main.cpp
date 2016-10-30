#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "file_packet_parser.h"
#include "file_slice_parser.h"
#include "netstat.h"
#include "query.h"

using namespace std;
enum ReportType { REP_NONE, REP_TEXT, REP_HTML } rep_type = REP_NONE;

const char* carrierToStr(Carrier c) {
	switch (c) {
		case CARR_WIFI: return "wifi";
		case CARR_MOBILE: return "mobile";
		case CARR_ETHER: return "ethernet";
		case CARR_LOCAL: return "loopback";
		default: return "?";
	}
}

const char* directionToStr(Direction d) {
	switch (d) {
		case DIR_IN: return "IN";
		case DIR_OUT: return "OUT";
		case DIR_LOCAL: return "LOCAL";
		case DIR_FORW: return "FORW";
		default: return "?";
	}
}

void textReport(const PacketStats& ps, const AppsActivity& apps, const RemotesInfo& remotes) {
	puts("Identified packets:\n");
	printf("Host\t\tPort\tDir\tCarrier\tSize\tApplication\n");
	for (PacketStats::const_iterator it = ps.begin(); it != ps.end(); ++it) {
		printf("%s\t%d\t%s\t%s\t%d\t%s\n",
			ipToStr(it->remote_host).c_str(), it->remote_port,
			directionToStr(it->direction), carrierToStr(it->carrier),
			it->len, it->app_name.c_str());
	}

	puts("Application activity:\n");
	for (AppsActivity::const_iterator ai = apps.begin(); ai != apps.end(); ++ai) {
		const AppActivity& a = ai->second;
		
		printf("%s\n\tRemote Traffic:\n\t\t%-16s\t%-48s\t%-5s\t%-10s\t%-10sSSL CN\n",
			ai->first.empty() ? "<unknown>" : ai->first.c_str(),
			"IP", "Hostname", "Port", "Downl", "Upl");
		for (std::map<Remote, Traffic>::const_iterator rem = a.remote_traffic.begin();
			 rem != a.remote_traffic.end(); ++rem)
		{
			const uint32_t host_ip = rem->first.host;
			const Traffic& traf = rem->second;
			RemotesInfo::const_iterator rit = remotes.find(host_ip);
			assert(rit != remotes.end());
			const RemoteInfo& ri = rit->second;
			printf("\t\t%-16s\t%-48s\t%-5d\t%-10d\t%-10d%s\n",
				ipToStr(host_ip).c_str(), ri.domain.c_str(),
				rem->first.port, traf.downloaded, traf.uploaded, ri.ssl.c_str());
		}

		printf("\tInterface Usage:\n\t\t%-16s\t%-5s\t%-5s\n", "Interface", "Downl", "Upl");
		for (std::map<Carrier, Traffic>::const_iterator car = a.carrier_traffic.begin();
			 car != a.carrier_traffic.end(); ++car)
		{
			const char* if_name = carrierToStr(car->first);
			const Traffic& traf = car->second;
			printf("\t\t%-16s\t%-5d\t%-5d\n", if_name, traf.downloaded, traf.uploaded);
		}
	}
}

int main(int argc, const char** const argv) {
	if (argc != 4) {
		printf("Usage: %s <packet_file> <network_file> <text|html>\n", argv[0]);
		return 1;
	}

	if (!strcmp(argv[3], "text"))
		rep_type = REP_TEXT;
	else if (!strcmp(argv[3], "html"))
		rep_type = REP_HTML;
	else {
		printf("Invalid report type %s\n", argv[3]);
		return 1;
	}

	FilePacketParser packet_parser(argv[1], 0, 1000000);
	FileSliceParser slice_parser(argv[2]);

	puts("Parsing packet file...");
	Packets packets;
	string err = packet_parser.parse(&packets);
	if (!err.empty()) {
		printf("Error parsing packet file %s: %s\n", argv[1], err.c_str());
		return 2;
	}
	puts("Done");

	puts("Parsing network file...");
	Slices slices;
	if (!slice_parser.parse(&slices)) {
		printf("Error parsing network file %s\n", argv[2]);
		return 3;
	}
	puts("Done");

	PacketStats stat;
	AppsActivity apps;
	RemotesInfo remotes;
	
	puts("Generating statistics ...");
	generatePacketStats(packets, slices, &stat);
	puts("Done");

	puts("Grouping statistics and resolving ...");
	generateAppStats(stat, &apps, &remotes);
	puts("Done");

	switch (rep_type) {
		case REP_TEXT:
			textReport(stat, apps, remotes);
			break;
		default:
			puts("<not supported>");
	}

	return 0;
}
