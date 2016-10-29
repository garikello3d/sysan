#pragma once
#include "packets.h"
#include <stdlib.h>
#include <pcap.h>

class FilePacketParser : public PacketParser {
public:
	FilePacketParser(const std::string& file, int limit, int precision_to_sec);
	virtual std::string parse(Packets* const packets);

protected:
	std::string initPcap();
	void deinitPcap();
	std::string startPseudoCapture(Packets* const packets);

private:
	static void packetCallback(unsigned char *user, const pcap_pkthdr *h, const unsigned char *sp);
	static bool parseLinkAndAbove(const uint8_t* data, size_t size, Packet* const packet);
	static bool parseIpAndAbove(const uint8_t* data, size_t size, Packet* const packet);
	static bool parseTransport(uint8_t proto, const uint8_t* data, size_t size, Packet* const packet);

	int datalinkType() const;
	int precision() const;
	int limit() const;
	pcap_t* pcap() const;
	bool incAndTestLimit();

	const std::string m_file;
	const int m_limit;
	const int m_precision;
	pcap_t* m_pcap;
	int m_datalink;
	int m_nrCaptured;
};
