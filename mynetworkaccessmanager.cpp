#include "mynetworkaccessmanager.h"
#include "global.h"

/**
 * @brief Crée les outils permettant d'utiliser les cookies et en théorie le cache
 */
MyNetworkAccessManager::MyNetworkAccessManager()
	:QNetworkAccessManager()
{
	m_cookieJar = new CookieJar();
	this->setCookieJar(m_cookieJar);
	m_webCache = new QNetworkDiskCache(this);
	m_webCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/cache");
	m_webCache->setMaximumCacheSize(10*1024*1024); // 10Mo
	this->setCache(m_webCache);
}

/**
 * @brief Gestion de requête http
 * @param op			paramètre de base
 * @param req			Requête initiale
 * @param outgoingData	Paramètre de base
 * @return QNetworkAccessManager::createRequest(op, request, outgoingData)
 */
QNetworkReply *MyNetworkAccessManager::createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
	QNetworkRequest request(req);
	//Avec ce passage, l'application est censée accéder au cache lorsque le réseau n'est pas accessible, mais cela ne fonctionne pas
	if(this->networkAccessible() == QNetworkAccessManager::NotAccessible)
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

/**
 * @brief Supprime les cookies
 */
void MyNetworkAccessManager::clearCookies()
{
	m_cookieJar->clear();
}
