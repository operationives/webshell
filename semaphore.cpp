#include "semaphore.h"
#include <QDebug>

/**
 * @brief Semaphore::Semaphore Création du sémaphore
 * @param free  Disponibilité du sémaphore
 */
Semaphore::Semaphore(int free)
    : QObject()
{
    //Si free>0, le sémaphore est disponible
    this->free = free;
    //Pile d'objets servant à mettre en attente les threads
    this->stack = new QList<QEventLoop *>();
    //Mutex permettant d'assurer l'exclusion mutuelle
    this->mutex = new QMutex();
}

/**
 * @brief Semaphore::~Semaphore
 */
Semaphore::~Semaphore()
{
    delete mutex;
    delete stack;
    this->~QObject();
}

/**
 * @brief Semaphore::Acquire Si le sémaphore est disponible, il laisse la main, sinon il place l'exécution en attente
 */
void Semaphore::Acquire()
{
    mutex->lock();
    if(free>0)
    {
        //Le sémaphore est disponible: on décrémente la disponibilité et on continue le flot d'exécution
        free--;
        mutex->unlock();
    }
    else
    {
        qDebug() << "On empile la stack";
        qDebug() << free;
        //Sémaphore indisponible: mise en attente du thread jusqu'à ce qu'il soit libéré par un futur Release
        QEventLoop *loop = new QEventLoop();
        stack->append(loop);
        mutex->unlock();

        loop->exec();
        qDebug() << "On quitte acquire";

        mutex->lock();
        free--;
        mutex->unlock();
    }
}

/**
 * @brief Semaphore::Release Augmente la disponibilité et si nécessaire libère le premier thread à avoir été mis en attente
 */
void Semaphore::Release()
{
    mutex->lock();
    free++;
    if(free>0)
    {
        if(!stack->isEmpty())
        {
            qDebug() << "On dépile la stack";
            qDebug() << free;
            //Si le sémaphore est disponible et qu'un thread est en attente, on le libère puis on détruit la boucle
            QEventLoop *loop = stack->first();
            if(loop->isRunning())
                qDebug() << "La boucle tourne";
            loop->quit();
            stack->removeFirst();
            delete loop;
        }
    }
    mutex->unlock();
}
