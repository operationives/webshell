#include "mynetworkaccessmanager.h"
#include "global.h"
#include <QNetworkReply>

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
	connect(this,SIGNAL(finished(QNetworkReply*)),this,SLOT(getLanguage(QNetworkReply*)));
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
	//Avec ce header, on peut si il n'y a pas de cookie défini pour la langue accéder à une langue spécifique pour la page demandée
	request.setRawHeader("Accept-Language", config->GetLanguage().toLatin1());
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

/**
 * @brief Supprime les cookies
 */
void MyNetworkAccessManager::clearCookies()
{
	m_cookieJar->clear();
}

/**
 * @brief Si le champ cookie est présent dans la réponse et qu'elle contient le paramètre langue, on le stocke dans le fichier de config
 * @param reply	Réponse du serveur pouvant indiquer la langue dans les cookies
 */
void MyNetworkAccessManager::getLanguage(QNetworkReply *reply)
{
	if(reply->hasRawHeader("Set-Cookie"))
	{
		QString langue(reply->rawHeader("Set-Cookie"));
		if(!langue.contains("langue="))
			return;
		int index = langue.indexOf("langue=");
		langue.replace(0,index+7,"");
		langue.truncate(2);
		config->SetLanguage(langue);
	}
}