#pragma once
#include "worker.h"
#include <pcap.h>

class Sniffer : public Worker {
public:
	Sniffer(WorkerObserver* const obs,
		const std::string& dev_name, const std::string& out_filename);

	virtual ~Sniffer();

	uint64_t nrPackets() const;

protected:
	virtual bool init();
	virtual void deinit();
	virtual void runUntilStopOrError();
	virtual void flagStop();

private:
	static void packetCallback(unsigned char *user, const pcap_pkthdr *h, const unsigned char *sp);
	void incPackets();
	
	const std::string m_dev;
	pcap_t* m_pcap;
	pcap_dumper_t* m_dumper;
	const std::string m_outFile;
	volatile bool m_exit;
	uint64_t m_nrPackets;

	//const int PACKET_TIMEOUT_MS = 1000; // 1 sec
};
