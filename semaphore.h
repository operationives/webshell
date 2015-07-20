#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <QEventLoop>
#include <QMutex>

class Semaphore : public QObject
{
    Q_OBJECT

public:
    Semaphore(int free = 1);
    ~Semaphore();
    void Acquire();
    void Release();

private:
    int free;
    QList<QEventLoop *> *stack;
    QMutex *mutex;
};


#endif // SEMAPHORE_H

