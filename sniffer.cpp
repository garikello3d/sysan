#include "sniffer.h"
using namespace std;

Sniffer::Sniffer(WorkerObserver* const obs,
	const string& dev_name, const string& out_filename)
	: Worker(obs), m_dev(dev_name),
	  m_pcap(NULL), m_dumper(NULL), m_outFile(out_filename),
	  m_exit(false), m_nrPackets(0)
{
}

Sniffer::~Sniffer() {
}

uint64_t Sniffer::nrPackets() const {
	return m_nrPackets;
}

bool Sniffer::init() {
	char ebuf[PCAP_ERRBUF_SIZE] = {0};
	int status;

	m_pcap = pcap_create(m_dev.c_str(), ebuf);
	if (!m_pcap) {
		signalError(ebuf);
		return false;
	}

	status = pcap_set_snaplen(m_pcap, 65536);
	if (status != 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_set_snaplen failed: %s",
			pcap_statustostr(status));
		signalError(ebuf);
		return false;
	}

	/*status = pcap_set_timeout(m_pcap, PACKET_TIMEOUT_MS);
	if (status != 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_set_timeout failed: %s",
			pcap_statustostr(status));
		signalError(ebuf);
		return false;
	}*/

	status = pcap_activate(m_pcap);
	if (status < 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate error: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		signalError(ebuf);
		return false;
	}
	else if (status > 0) {
		snprintf(ebuf, sizeof(ebuf), "pcap_activate warning: %s/%s",
			pcap_statustostr(status), pcap_geterr(m_pcap));
		signalError(ebuf);
		return false;
	}

	// TODO hope nonblocking mode is off by default?

	m_dumper = pcap_dump_open(m_pcap, m_outFile.c_str());
	if (!m_dumper) {
		snprintf(ebuf, sizeof(ebuf), "pcap_dump_open error: %s",
			pcap_geterr(m_pcap));
		signalError(ebuf);
		return false;
	}

	return true;
}

void Sniffer::deinit() {
	if (m_dumper) {
		pcap_dump_close(m_dumper);
		m_dumper = NULL;
	}
	if (m_pcap) {
		pcap_close(m_pcap);
		m_pcap = NULL;
	}
}

void Sniffer::runUntilStopOrError() {
	while (!m_exit) {
		pair<pcap_dumper_t*, Sniffer*> user_data(m_dumper, this);
		int status = pcap_loop(m_pcap, -1, packetCallback,
			reinterpret_cast<unsigned char*>(&user_data));
		printf("(pcap_loop -> %d)\n", status);
		if (status == -1) {
			char buf[256];
			snprintf(buf, sizeof(buf), "pcap_loop error: %s", pcap_geterr(m_pcap));
			signalError(buf);
			break;
		}
		if (status >= 0)
			printf("(pcap_loop cought %d packets)\n", status);
		else if (status == -2) {
			puts("(pcap_loop terminated its loop)");
			break;
		}
	}
}

void Sniffer::flagStop() {
	m_exit = true;
	pcap_breakloop(m_pcap);
}

// static
void Sniffer::packetCallback(unsigned char *user, const pcap_pkthdr *h, const unsigned char *sp) {
	pair<pcap_dumper_t*, Sniffer*>* user_data =
		reinterpret_cast<pair<pcap_dumper_t*, Sniffer*>*>(user);

	pcap_dump(reinterpret_cast<unsigned char*>(user_data->first), h, sp);

	Sniffer* clazz = reinterpret_cast<Sniffer*>(user_data->second);
	clazz->incPackets();
}

void Sniffer::incPackets() {
	++m_nrPackets;
}
