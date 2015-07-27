#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <QEventLoop>
#include <QMutex>

class Semaphore : public QObject
{
	Q_OBJECT

public:
	Semaphore(int nbRessources = 1);
	~Semaphore();
	void Acquire();
	void Release();

private:
	int nbRessources;
	QList<QEventLoop *> *stack;
	QMutex *mutex;
};


#endif // SEMAPHORE_H

