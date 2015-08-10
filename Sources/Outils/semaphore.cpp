#include "semaphore.h"

/**
 * @brief Création du sémaphore
 * @param nbRessources  Disponibilité du sémaphore
 */
Semaphore::Semaphore(int nbRessources)
	: QObject()
{
	//Si nbRessources>0, le sémaphore est disponible
	this->nbRessources = nbRessources;
	//Pile d'objets servant à mettre en attente les threads
	this->stack = new QList<QEventLoop *>();
	//Mutex permettant d'assurer l'exclusion mutuelle
	this->mutex = new QMutex();
	//Cela permet de savoir si l'acquire s'est effectué avant ou après la destruction de l'objet
	this->acquireSuccess = true;
}

/**
 * @brief libère les threads en attente en leur indiquant qu'ils ne peuvent pas aller plus loin
 */
Semaphore::~Semaphore()
{
	mutex->lock();
	this->acquireSuccess = false;
	int i;
	for(i = 0; i!= stack->length();++i)
	{
		QEventLoop *loop = stack->last();
		loop->quit();
		stack->removeLast();
		delete loop;
	}
	mutex->unlock();
	delete stack;
	delete mutex;
}

/**
 * @brief Si le sémaphore est disponible, il laisse la main, sinon il place l'exécution en attente\n
 * Lorsque le destructeur libère la boucle, le résultat envoyé est faux, sinon Acquire renvoie vrai
 */
bool Semaphore::Acquire()
{
	mutex->lock();
	if(nbRessources>0)
	{
		//Le sémaphore est disponible: on décrémente la disponibilité et on continue le flot d'exécution
		nbRessources--;
		mutex->unlock();
	}
	else
	{
		//Sémaphore indisponible: mise en attente du thread jusqu'à ce qu'il soit libéré par un futur Release
		QEventLoop *loop = new QEventLoop();
		stack->append(loop);
		mutex->unlock();

		loop->exec();

		mutex->lock();
		nbRessources--;
		mutex->unlock();
	}
	return this->acquireSuccess;
}

/**
 * @brief Augmente la disponibilité et si nécessaire libère le premier thread à avoir été mis en attente
 */
void Semaphore::Release()
{
	mutex->lock();
	nbRessources++;
	if(nbRessources>0)
	{
		if(!stack->isEmpty())
		{
			//Si le sémaphore est disponible et qu'un thread est en attente, on le libère puis on détruit la boucle
			QEventLoop *loop = stack->last();
			loop->quit();
			stack->removeLast();
			delete loop;
		}
	}
	mutex->unlock();
}
