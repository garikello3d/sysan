#include "net_collector.h"
#include "collector.h"
#include "slice_db.h"
#include <sstream>
#include <unistd.h>
#include <sys/errno.h>
using namespace std;

NetCollector::NetCollector(
	WorkerObserver* const obs, int every_sec, const std::string& out_filename)
	: Worker(obs), m_everySec(every_sec), m_outFile(out_filename), m_exit(false), m_file(NULL)
{
}

NetCollector::~NetCollector() {
}

bool NetCollector::init() {
	FILE* f = fopen(m_outFile.c_str(), "w");
	if (!f)
		return false;
	m_file = f;
	m_counter = 0;
	return true;
}

void NetCollector::deinit() {
	if (m_file)
		fclose(m_file);
}

void NetCollector::runUntilStopOrError() {
	while (!m_exit) {
		++m_counter;

		stringstream ss;
		Slice slice;
		collectSlice(&slice);
		cereal::BinaryOutputArchive oarchive(ss);

		// TODO/FIXME add time!
		oarchive(slice);
		const string& buf = ss.str();

		size_t wr = fwrite(buf.data(), buf.size(), 1, m_file);
		if (wr != 1) {
			char buf[256];
			snprintf(buf, sizeof(buf), "slice write %d error: %s",
				m_counter, strerror(errno));
			signalError(buf);
			break;
		}
		fflush(m_file);

		sleep(m_everySec);
	}
}

void NetCollector::flagStop() {
	m_exit = true;
}
