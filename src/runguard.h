#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QSharedMemory>
#include <QSystemSemaphore>

class RunGuard final
{
private:
	const QString key;
	const QString memLockKey;
	const QString sharedmemKey;

	QSharedMemory sharedMem;
	QSystemSemaphore memLock;

	static QString generateKeyHash(const QString & key, const QString & salt);

public:
	explicit RunGuard(const QString & key);
	~RunGuard();
	Q_DISABLE_COPY(RunGuard)

	bool isAnotherRunning();
	bool tryToRun();
	void release();
};

#endif // RUNGUARD_H
