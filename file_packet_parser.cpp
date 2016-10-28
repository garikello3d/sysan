#include "file_packet_parser.h"
using namespace std;

FilePacketParser::FilePacketParser(
	const std::string& file, int limit, int precision_to_sec)
	: m_file(file), m_limit(limit), m_precision(precision_to_sec),
	  m_pcap(NULL)
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
	int status;

	m_pcap = pcap_open_offline(m_file.c_str(), ebuf);
	if (!m_pcap) {
		return string(ebuf);
	}

	status = pcap_set_snaplen(m_pcap, 65536);
	if (status != 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_set_snaplen failed: %s",
			pcap_statustostr(status));
		return string(ebuf);
	}

	status = pcap_activate(m_pcap);
	if (status < 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate error: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		return string(ebuf);
	}
	else if (status > 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate warning: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		return string(ebuf);
	}

	return string();
}

void FilePacketParser::deinitPcap() {
	if (m_pcap) {
		pcap_close(m_pcap);
		m_pcap = NULL;
	}
}

string FilePacketParser::startPseudoCapture(Packets* const packets) {
	CaptureContext ctx(packets, pcap_datalink(m_pcap), m_precision);

	int status = pcap_loop(m_pcap, -1, packetCallback,
		reinterpret_cast<unsigned char*>(&ctx));

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
	CaptureContext* ctx = reinterpret_cast<CaptureContext*>(user);

	Packet p;
	const uint64_t microsec = (uint64_t)h->ts.tv_sec * 1000000 + (uint64_t)h->ts.tv_usec;
	p.abs_time = microsec % (1000000 / ctx->precision);
	p.len = h->len;

	// NOTE sp starts from link-level header
	if (parseIpAndAbove(ctx->datalink_type, sp, h->caplen, &p))
		ctx->packets->push_back(p);
}

// static
bool FilePacketParser::parseIpAndAbove(
	int link_type, const uint8_t* data, size_t size, Packet* const packet)
{
	return true;
}
