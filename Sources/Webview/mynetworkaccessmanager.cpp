#include "mynetworkaccessmanager.h"
#include "Outils/configmanager.h"
#include <QNetworkReply>
#include <QStandardPaths>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QtWidgets>

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
    m_isFirstAuthentication = true;
    m_NtlmAuthenticationTryCount = 0;
	m_cookieJar = new CookieJar();
    connect(m_cookieJar,SIGNAL(cookieLoaded(QString,QString)),this,SLOT(cookieLoaded(QString,QString)));
	this->setCookieJar(m_cookieJar);
	m_webCache = new QNetworkDiskCache(this);
	ConfigManager &config = ConfigManager::Instance();
    m_webCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+config.GetAppName());
    m_webCache->setMaximumCacheSize(50*1024*1024); // 50Mo
	this->setCache(m_webCache);
	connect(this,SIGNAL(finished(QNetworkReply*)),this,SLOT(getLanguage(QNetworkReply*)));
    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    m_pending_login = "";
}


void MyNetworkAccessManager::onProxyAuthenticationRequired(const QNetworkProxy &prox, QAuthenticator *auth)
{
    qDebug() << "[AUTH] On Proxy Authentication Required Event:";

    // Configuring QAuthenticator for Windows Integrated Authentication with NTLM protocol:
    /*auth->setUser("");

    if (m_NtlmAuthenticationTryCount <= NTLM_AUTH_MAX_TRY_COUNT)
    {
        qDebug() << "[AUTH] > Try NTLM authentication:  " << m_NtlmAuthenticationTryCount << " on " << NTLM_AUTH_MAX_TRY_COUNT;
        m_NtlmAuthenticationTryCount++;
        //return;
    }*/

    qDebug() << "[AUTH] > NO NTLM Authentication. " << (m_isFirstAuthentication ? "First authentication." : "Additional authentication try");
    m_didNtlmAuthenticationTried = true;

    QDialog dialog(NULL,Qt::WindowCloseButtonHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::MSWindowsFixedSizeDialogHint);
    QDialogButtonBox authentication_window(QDialogButtonBox::Ok | QDialogButtonBox::Close);
    QFormLayout layout;
    QLabel infoLabel, UserNameLabel, PasswordLabel, incorrectPassword;
    QLineEdit UserNameEdit, PasswordEdit;
    QString os_language = QLocale::system().uiLanguages().first();

    authentication_window.setGeometry(0, 0, 320, 150);
    UserNameEdit.setFixedWidth(150);
    PasswordEdit.setFixedWidth(150);
    infoLabel.setWordWrap(true);
    incorrectPassword.setWordWrap(true);
    PasswordEdit.setEchoMode(QLineEdit::Password);

    layout.addRow(&infoLabel);
    layout.setVerticalSpacing(20);
    layout.addRow(&UserNameLabel, &UserNameEdit);
    layout.addRow(&PasswordLabel, &PasswordEdit);
    if (m_isFirstAuthentication == false) layout.addRow(&incorrectPassword);

    authentication_window.layout()->setSizeConstraint( QLayout::SetFixedSize );
    layout.addWidget(&authentication_window);

    dialog.setLayout(&layout);

    QString proxyName = prox.hostName() + ":" + QString::number(prox.port());
    os_language.truncate(2);
    if (os_language == FR)
    {
        dialog.setWindowTitle("Authentification requise");
        infoLabel.setText("Le proxy " + proxyName + " demande un nom d'utilisateur et un mot de passe pour établir la connexion.");
        UserNameLabel.setText("Nom d'utilisateur : ");
        PasswordLabel.setText("Mot de passe : ");
        incorrectPassword.setText("<font color='red'>L'authentification a échoué. Veuillez entrer à nouveau vos coordonnées.</font>");
    }
    else
    {
        dialog.setWindowTitle("Authentication required");
        infoLabel.setText("The proxy " + proxyName + " requires a user name and a password.");
        UserNameLabel.setText("User name: ");
        PasswordLabel.setText("Password: ");
        incorrectPassword.setText("<font color='red'>Authentication failed. Please verify your login and your password.</font>");
    }

    QObject::connect(&authentication_window,SIGNAL(accepted()),&dialog,SLOT(accept()));
    QObject::connect(&authentication_window,SIGNAL(rejected()),&dialog,SLOT(reject()));

    int dialogCode = dialog.exec();

    switch(dialogCode)
    {
        case QDialog::Accepted:
            qDebug() << "[AUTH] > Proxy authentication accepted.";
            if (!UserNameEdit.text().isEmpty())
            {
                auth->setUser(UserNameEdit.text());
            }
            else qDebug() << "[AUTH] > Proxy user name is empty.";
            if (!PasswordEdit.text().isEmpty())
            {
                auth->setPassword(PasswordEdit.text());
            }
            else qDebug() << "[AUTH] > Proxy password is empty.";
            m_isFirstAuthentication = false;
        break;
        default:
        case QDialog::Rejected:
            qDebug() << "[AUTH] > Proxy authentication rejected. Quit application.";
            QApplication::quit();
        break;
    }
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
        if (value != "")
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

    //qDebug() << "[TEST] Creating request: " << req.url();

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
                //qDebug() << "Potential login detected: [" << QUrl::fromPercentEncoding(values[1].toUtf8()) << "]";
                m_pending_login = QUrl::fromPercentEncoding(values[1].toUtf8());
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

    if (statusCode == 301)// Permanent redirection detected
    {
        ConfigManager &config = ConfigManager::Instance();
        QString redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        QStringList list = config.GetBaseUrl();
        list.append(redirect);
        config.SetBaseUrl(list);
        qDebug() << "Permanent redirection detected: store the url: " << config.GetBaseUrl();
    }
    else if (statusCode == 302) // redirection detected
    {
        if (m_pending_login != "")
        {
            // Set the login as the last one:
            ConfigManager &config = ConfigManager::Instance();
            config.SetLastLogin(m_pending_login);

            QStringList login_list(config.GetLoginList());
            if (!login_list.contains(m_pending_login))
            {
                qDebug() << "Storing the new login: [" << m_pending_login << "]";
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
        if (langue != "")
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
