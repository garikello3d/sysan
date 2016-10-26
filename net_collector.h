#pragma once
#include "worker.h"

class NetCollector : public Worker {
public:
	NetCollector(WorkerObserver* const obs, int every_sec, const std::string& out_filename);
	virtual ~NetCollector();

protected:
	virtual bool init();
	virtual void deinit();
	virtual void runUntilStopOrError();
	virtual void flagStop();

private:
	const int m_everySec;
	const std::string m_outFile;
	volatile bool m_exit;
	FILE* m_file;
	int m_counter;
};
