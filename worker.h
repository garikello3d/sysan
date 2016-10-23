#pragma once
#include <thread>

class WorkerObserver {
public:
	virtual void workerError(const std::string& msg) = 0;
};

class Worker {
public:
	Worker(WorkerObserver* const obs): m_observer(obs), m_thread(NULL) {
	}

	virtual ~Worker() {
		assert(!m_thread);
	}

	bool spawnUntilStopOrError() {
		if (!init())
			return false;
		m_thread = new std::thread(threadFunc, this);
		assert(m_thread);
		return true;
	}

	void stopAndDeinit() {
		if (m_thread) { // i.e. init() was successfull
			flagStop();
			m_thread->join();
			delete m_thread;
			m_thread = NULL;
		}
		deinit();
	}

protected:
	virtual bool init() = 0;
	virtual void deinit() = 0;
	virtual void runUntilStopOrError() = 0;
	virtual void flagStop() = 0;

	void signalError(const std::string& msg) { m_observer->workerError(msg); }

private:
	static void threadFunc(void* p) {
		Worker* w = reinterpret_cast<Worker*>(p);
		w->runUntilStopOrError();
	}
	
	WorkerObserver* const m_observer;
	std::thread* m_thread;
};
