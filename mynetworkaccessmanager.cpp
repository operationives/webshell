#include "mynetworkaccessmanager.h"
#include "global.h"

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

QNetworkReply *MyNetworkAccessManager::createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
	QNetworkRequest request(req);
	//Avec ce passage, l'application est censée accéder au cache lorsque le réseau n'est pas accessible, mais cela ne fonctionne pas
	if(this->networkAccessible() == QNetworkAccessManager::NotAccessible)
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

void MyNetworkAccessManager::clearCookies()
{
	m_cookieJar->clear();
}
