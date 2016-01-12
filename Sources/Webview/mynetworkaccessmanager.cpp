#include "mynetworkaccessmanager.h"
#include "Outils/configmanager.h"
#include <QNetworkReply>
#include <QStandardPaths>


MyNetworkAccessManager* MyNetworkAccessManager::m_instance=NULL;

MyNetworkAccessManager* MyNetworkAccessManager::Instance()
{
	if(!m_instance)
	{
		m_instance = new MyNetworkAccessManager();
	}
	return m_instance;
}

/**
 * @brief Crée les outils permettant d'utiliser les cookies et en théorie le cache
 */
MyNetworkAccessManager::MyNetworkAccessManager()
	:QNetworkAccessManager()
{
	m_cookieJar = new CookieJar();
    connect(m_cookieJar,SIGNAL(cookieLoaded(QString,QString)),this,SLOT(cookieLoaded(QString,QString)));
	this->setCookieJar(m_cookieJar);
	m_webCache = new QNetworkDiskCache(this);
	ConfigManager &config = ConfigManager::Instance();
    m_webCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+config.GetAppName());
    m_webCache->setMaximumCacheSize(50*1024*1024); // 50Mo
	this->setCache(m_webCache);
	connect(this,SIGNAL(finished(QNetworkReply*)),this,SLOT(getLanguage(QNetworkReply*)));
    m_pending_login = "";
}

/**
 * @brief Intercepte les cookies chargés depuis le disque
 * synchronise la langue entre l'xml de config et le cookie
 */
void MyNetworkAccessManager::cookieLoaded(QString name, QString value)
{
    if (name == "langue")
    {
        ConfigManager &config = ConfigManager::Instance();
        config.SetLanguage(value);
    }
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

    if (outgoingData)
    {


        // Get user data:
        char buffer[256];
        outgoingData -> peek(buffer,256);

        QString data(buffer);
        // Buffer parsing:
        QStringList parameters;
        parameters = data.split("&");

        for (int i = 0; i < parameters.size(); ++i)
        {
            QString parameter = parameters.at(i);
            QStringList values;
            values = parameter.split("=");
            if (values.at(0) == QString("email"))
            {
                qDebug() << "Potential login detected: [" << values[1] << "]";
                m_pending_login = values[1];
            }
        }
    }

	//Avec ce passage, l'application est censée accéder au cache lorsque le réseau n'est pas accessible, mais cela ne fonctionne pas
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    //if(this->networkAccessible() == QNetworkAccessManager::NotAccessible)
    //	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);
	//Avec ce header, on peut si il n'y a pas de cookie défini pour la langue accéder à une langue spécifique pour la page demandée
	ConfigManager &config = ConfigManager::Instance();
    request.setRawHeader("Accept-Language", config.GetLanguage().toLatin1());

    /*QNetworkReply *reply = QNetworkAccessManager::createRequest(op, request, outgoingData);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(handlReplyError(QNetworkReply::NetworkError)));
    if (outgoingData)
        if(this->networkAccessible() == QNetworkAccessManager::NotAccessible)
        {
            qDebug() << "Network is not accessible. Abort the post request";
            //connect(this, SIGNAL(cancelRequest()), reply, SLOT(abort()));
            //emit cancelRequest();
            reply->abort();
            //emit reply->error(QNetworkReply::TemporaryNetworkFailureError);
        }
    return reply;*/

    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

/**
 * @brief Supprime les cookies et le cache
 */
void MyNetworkAccessManager::clearAll()
{
	m_cookieJar->clear();
	m_webCache->clear();
}

/**
 * @brief Si le champ cookie est présent dans la réponse et qu'elle contient le paramètre langue, on le stocke dans le fichier de config
 * @param reply	Réponse du serveur pouvant indiquer la langue dans les cookies
 */
void MyNetworkAccessManager::getLanguage(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 302) // redirection detected
    {
        if (m_pending_login != "")
        {
            // Set the login as the last one:
            ConfigManager &config = ConfigManager::Instance();
            config.SetLastLogin(m_pending_login);

            QStringList login_list(config.GetLoginList());
            if (!login_list.contains(m_pending_login))
            {
                qDebug() << "Storage of the new login: [" << m_pending_login << "]";
                login_list.append(m_pending_login);
                config.SetLoginList(login_list);
            }
            m_pending_login = "";
        }
    }

	if(reply->hasRawHeader("Set-Cookie"))
	{
        QString langue(reply->rawHeader("Set-Cookie"));
		if(!langue.contains("langue="))
			return;
		int index = langue.indexOf("langue=");
		langue.replace(0,index+7,"");
		langue.truncate(2);
        ConfigManager &config = ConfigManager::Instance();
		config.SetLanguage(langue);
	}
}

/**
 * @brief renvoie le cookie jar
 */
CookieJar* MyNetworkAccessManager::getCookieJar()
{
    return m_cookieJar;
}


/*void MyNetworkAccessManager::handlReplyError(QNetworkReply::NetworkError error)
{
    switch(error)
    {
     case QNetworkReply::NoError:
        qDebug() << "No error";
        break;
     default:
        qDebug() << "Reply error";
        break;
    }
}*/
