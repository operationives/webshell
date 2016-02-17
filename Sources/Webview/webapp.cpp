#include <QApplication>
#include "webapp.h"
#include "Outils/filedownloader.h"
#include "Outils/configmanager.h"


/**
 * @brief Constructeur de l'objet WebApp
 * @param view	Page sur laquelle effectuer des commandes JavaScript
 */
WebApp::WebApp(MyWebView *view)
{
	this->m_webView = view;
	this->m_target = "window";
}

/**
 * @brief Si un icône est sauvegardé dans les données, il est supprimé
 */
WebApp::~WebApp()
{
	if(!currentFileDirectory.isNull())
		QFile::remove(currentFileDirectory);
}

/**
 * @brief Méthode de progression de téléchargement, actuellement rien à faire pour un téléchargement si petit
 * @param bytesReceived	Nombre d'octets reçus
 * @param bytesTotal	Nombre d'octets total
 * @param typemime		String non significatif
 */
void WebApp::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &typemime)
{
	Q_UNUSED(bytesReceived);
	Q_UNUSED(bytesTotal);
	Q_UNUSED(typemime);
}

/**
 * @brief Envoie un signal avec l'icône téléchargé
 * @param typemime	String non significatif
 */
void WebApp::FileDownloaded(const QString &typemime)
{
	QString filename = data->GetUrl();
	filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
	if(!currentFileDirectory.isNull())
		QFile::remove(currentFileDirectory);
	currentFileDirectory = QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/");
	currentFileDirectory.append(filename);
	QFile file(currentFileDirectory);

	if(!file.open(QIODevice::WriteOnly))
	{
		qWarning() << "Fichier d'installation webapp impossible à ouvrir. Type mime: " << typemime;
		return;
	}
	file.write(data->DownloadedData());
	file.close();

	QIcon icon(currentFileDirectory);

	emit changeIcon(icon);
}

/**
 * @brief Méthode appelée lors de l'échec d'un téléchargement, envoie un message d'erreur dans la console
 * @param typemime String non significatif
 */
void WebApp::DownloadFailure(const QString &typemime)
{
	qWarning() << "Erreur téléchargement icône " << typemime;
}

/**
 * @brief Accesseur de l'icône
 * @return config.GetIcon()
 */
QString WebApp::Icon() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetIcon();
}

/**
 * @brief Change l'icône enregistré dans le gestionnaire de fichier XML et modifie l'icône de la barre de notification
 * @param icon  Url de l'icône de l'application
 */
void WebApp::SetIcon(const QString &icon)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetIcon(icon);
	data = new FileDownloader(icon,qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief Accesseur des informations
 * @return config.GetInfos()
 */
QString WebApp::Infos() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetInfos();
}

/**
 * @brief Change les infos enregistrées dans le gestionnaire de fichier XML et modifie le fichier xml de l'application
 * @param infos
 */
void WebApp::SetInfos(const QString &infos)
{
    ConfigManager &config = ConfigManager::Instance();
    config.SetInfos(infos);
}

/**
 * @brief Accesseur du nom à afficher
 * @return config.GetDisplayName()
 */
QString WebApp::DisplayName() const
{
    ConfigManager &config = ConfigManager::Instance();
    return config.GetDisplayName();
}

/**
 * @brief Change le nom du service enregistré dans le gestionnaire de fichier XML et modifie le fichier xml de l'application
 * @param infos
 */
void WebApp::SetDisplayName(const QString &displayName)
{
    ConfigManager &config = ConfigManager::Instance();
    config.SetDisplayName(displayName);
}

/**
 * @brief Accesseur de baseUrl
 * @return
 */
QStringList WebApp::GetBaseUrl() const
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetBaseUrl();
}

/**
 * @brief Change baseUrl à l'aide du gestionnaire de fichier XML
 * @param value
 */
void WebApp::SetBaseUrl(const QStringList &value)
{
	ConfigManager &config = ConfigManager::Instance();
	QStringList newBaseUrl = value;
	config.SetBaseUrl(newBaseUrl);
}

/**
 * @brief Indique si la page courante est dans baseUrl
 * @return Vrai si la page courante est dans baseUrl, faux sinon
 */
bool WebApp::IsPageInApplication()
{
	ConfigManager &config = ConfigManager::Instance();
	QString urls = m_webView->url().toString();
	QStringList::iterator i;
	QStringList baseUrl = config.GetBaseUrl();
	bool res = false;
	for (i = baseUrl.begin(); i != baseUrl.end(); ++i)
	{
		if(urls.startsWith(*i))
		{
			res = true;
			break;
		}
	}
	return res;
}

/**
 * @brief Indique si l'url en paramètre est dans baseUrl
 * @param url	url dont on veut tester l'appartenance à baseUrl
 * @return Vrai si url est dans baseUrl, faux sinon
 */
bool WebApp::IsPageInApplication(QUrl url)
{
	ConfigManager &config = ConfigManager::Instance();
	QString urls = url.toString();
	QStringList::iterator i;
	QStringList baseUrl = config.GetBaseUrl();
	bool res = false;
	for (i = baseUrl.begin(); i != baseUrl.end(); ++i)
	{
		if(urls.startsWith(*i))
		{
			res = true;
			break;
		}
	}
	return res;

}

/**
 * @brief Retourne la cible des événements
 * @return m_target
 */
QString WebApp::Target() const
{
	return m_target;
}

/**
 * @brief Met à jour la cible des événements
 * @param target	Nouvelle cible des événements
 */
void WebApp::SetTarget(const QString &target)
{
	m_target = target;
}

/**
 * @brief Renvoie la largeur minimale de la fenêtre
 * @return config.GetMinWidth()
 */
int WebApp::GetMinWidth()
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetMinWidth();
}

/**
 * @brief Renvoie la hauteur minimale de la fenêtre
 * @return config.GetMinHeight()
 */
int WebApp::GetMinHeight()
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetMinHeight();
}

/**
 * @brief Met à jour la taille minimale de la fenêtre
 * @param minWidth	Nouvelle largeur minimale
 * @param minHeight	Nouvelle hauteur minimale
 */
void WebApp::SetMinSize(int minWidth, int minHeight)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetMinSize(minWidth,minHeight);
}

/**
 * @brief Retourne la largeur par défaut de la fenêtre
 * @return config.GetDefaultWidth()
 */
int WebApp::GetDefaultWidth()
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetDefaultWidth();
}

/**
 * @brief Retourne la hauteur par défaut de la fenêtre
 * @return config.GetDefaultHeight()
 */
int WebApp::GetDefaultHeight()
{
	ConfigManager &config = ConfigManager::Instance();
	return config.GetDefaultHeight();
}

/**
 * @brief Met à jour la taille par défaut de la fenêtre
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void WebApp::SetDefaultSize(int defaultWidth, int defaultHeight)
{
	ConfigManager &config = ConfigManager::Instance();
	config.SetDefaultSize(defaultWidth,defaultHeight);
}
