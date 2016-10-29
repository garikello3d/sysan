#include "file_packet_parser.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
using namespace std;

FilePacketParser::FilePacketParser(
	const std::string& file, int limit, int precision_to_sec)
	: m_file(file), m_limit(limit), m_precision(precision_to_sec),
	  m_pcap(NULL), m_datalink(0), m_nrCaptured(0)
{
}

string FilePacketParser::parse(Packets* const packets) {
	string err = initPcap();
	if (err.empty()) {
		packets->clear();
		err = startPseudoCapture(packets);
	}
	deinitPcap();
	return err;
}

string FilePacketParser::initPcap() {
	char ebuf[PCAP_ERRBUF_SIZE] = {0};
	//int status;

	m_pcap = pcap_open_offline(m_file.c_str(), ebuf);
	if (!m_pcap) {
		return string(ebuf);
	}

	/*status = pcap_set_snaplen(m_pcap, 65536);
	if (status != 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_set_snaplen failed: %s",
			pcap_statustostr(status));
		return string(ebuf);
		}*/

	/*status = pcap_activate(m_pcap);
	if (status < 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate error: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		return string(ebuf);
	}
	else if (status > 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate warning: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		return string(ebuf);
		}*/

	m_datalink = pcap_datalink(m_pcap);

	return string();
}

void FilePacketParser::deinitPcap() {
	if (m_pcap) {
		pcap_close(m_pcap);
		m_pcap = NULL;
	}
}

string FilePacketParser::startPseudoCapture(Packets* const packets) {
	pair<FilePacketParser*, Packets* const> user_data(this, packets);

	int status = pcap_loop(m_pcap, -1, packetCallback,
		reinterpret_cast<unsigned char*>(&user_data));

	if (status == -1) {
		char buf[256];
		snprintf(buf, sizeof(buf), "pcap_loop error: %s", pcap_geterr(m_pcap));
		return string(buf);
	}

	return string();
}

// static
void FilePacketParser::packetCallback(
	unsigned char *user, const pcap_pkthdr *h, const unsigned char *sp)
{
	pair<FilePacketParser*, Packets* const>* user_data =
		reinterpret_cast<pair<FilePacketParser*, Packets* const>* >(user);
	FilePacketParser* clazz = user_data->first;
	Packets* packets = user_data->second;

	if (!clazz->incAndTestLimit()) {
		pcap_breakloop(clazz->pcap());
		return;
	}

	if (clazz->datalinkType() != DLT_LINUX_SLL)
		return;

	Packet p;
	const uint64_t microsec = (uint64_t)h->ts.tv_sec * 1000000 + (uint64_t)h->ts.tv_usec;
	const int factor = 1000000 / clazz->precision();
	
	p.abs_time = (microsec / factor) * factor;
	p.len = h->len;
	p.ip_from = 0;
	p.port_from = 0;
	p.ip_to = 0;
	p.port_to = 0;

	// NOTE sp starts from link-level header
	if (parseLinkAndAbove(sp, h->caplen, &p))
		packets->push_back(p);
}

// static
bool FilePacketParser::parseLinkAndAbove(
	const uint8_t* data, size_t size, Packet* const packet)
{
	if (size < 16) return false;
	if (data[14] == 0x08 && data[15] == 0x00) // IPv4
		return parseIpAndAbove(data + 16, size - 16, packet);
	else
		return true;
}

// static
bool FilePacketParser::parseIpAndAbove(
	const uint8_t* data, size_t size, Packet* const packet)
{
	if (size < sizeof(ip))
		return false;
	const ip* iph = reinterpret_cast<const ip*>(data);
	if (iph->ip_v != IPVERSION)
		return false; // shouldn't be here, stream maleformed!?
	if (iph->ip_off & IP_MF)
		return true; // we don't handle fragments yet
	packet->ip_from = iph->ip_src.s_addr;
	packet->ip_to = iph->ip_dst.s_addr;
	return parseTransport(iph->ip_p, data + sizeof(ip), size - sizeof(ip), packet);
}

// static
bool FilePacketParser::parseTransport(
	uint8_t proto, const uint8_t* data, size_t size, Packet* const packet)
{
	if(proto == 6) {
		if (size < sizeof(tcphdr))
			return false;
		const tcphdr* tcph = reinterpret_cast<const tcphdr*>(data);
		packet->port_from = ntohs(tcph->source);
		packet->port_to = ntohs(tcph->dest);
	}
	else if (proto == 17) {
		if (size < sizeof(udphdr))
			return false;
		const udphdr* udph = reinterpret_cast<const udphdr*>(data);
		packet->port_from = ntohs(udph->source);
		packet->port_to = ntohs(udph->dest);
	}
	return true;
}

int FilePacketParser::datalinkType() const {
	return m_datalink;
}

int FilePacketParser::precision() const {
	return m_precision;
}

int FilePacketParser::limit() const {
	return m_limit;
}

pcap_t* FilePacketParser::pcap() const {
	return m_pcap;
}

bool FilePacketParser::incAndTestLimit() {
	return ++m_nrCaptured <= m_limit;
}
