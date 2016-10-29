#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "file_packet_parser.h"
#include "file_slice_parser.h"
#include "netstat.h"

using namespace std;
enum ReportType { REP_NONE, REP_TEXT, REP_HTML } rep_type = REP_NONE;

string ipToStr(uint32_t ip) {
	if (ip == 0)
		return "?\t";
	char s[32];
	in_addr ina;
	ina.s_addr = ip;
	strncpy(s, inet_ntoa(ina), sizeof(s));
	return string(s);
}

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

void textReport(const PacketStats& ps) {
	
	puts("Identified packets:\n");
	
	printf("Host\t\tPort\tDir\tCarrier\tSize\tApplication\n");
	for (PacketStats::const_iterator it = ps.begin(); it != ps.end(); ++it) {
		printf("%s\t%d\t%s\t%s\t%d\t%s\n",
			ipToStr(it->remote_host).c_str(), it->remote_port,
			directionToStr(it->direction), carrierToStr(it->carrier),
			it->len, it->app_name.c_str());
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

	puts("Generating statistics...");
	PacketStats stat;
	generatePacketStats(packets, slices, &stat);
	puts("Done");

	switch (rep_type) {
		case REP_TEXT:
			textReport(stat);
			break;
		default:
			puts("<not supported>");
	}

	return 0;
}
