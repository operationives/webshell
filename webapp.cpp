#include <QApplication>
#include "webapp.h"
#include "filedownloader.h"
#include "global.h"


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
 * @param mime_type		String non significatif
 */
void WebApp::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &mime_type)
{
	Q_UNUSED(bytesReceived);
	Q_UNUSED(bytesTotal);
	Q_UNUSED(mime_type);
}

/**
 * @brief Envoie un signal avec l'icône téléchargé
 * @param mime_type	String non significatif
 */
void WebApp::FileDownloaded(const QString &mime_type)
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
		qWarning() << "Fichier d'installation webapp impossible à ouvrir. Type mime: " << mime_type;
		return;
	}
	file.write(data->DownloadedData());
	file.close();

	QIcon icon(currentFileDirectory);

	emit changeIcon(icon);
}

/**
 * @brief Méthode appelée lors de l'échec d'un téléchargement, envoie un message d'erreur dans la console
 * @param mime_type String non significatif
 */
void WebApp::DownloadFailure(const QString &mime_type)
{
	qWarning() << "Erreur téléchargement icône " << mime_type;
}

/**
 * @brief Accesseur de l'icône
 * @return config->GetIcon()
 */
QString WebApp::Icon() const
{
	return config->GetIcon();
}

/**
 * @brief Change l'icône enregistré dans le gestionnaire de fichier XML et modifie l'icône de la barre de notification
 * @param icon  Url de l'icône de l'application
 */
void WebApp::SetIcon(const QString &icon)
{
	config->SetIcon(icon);
	data = new FileDownloader(icon,qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief Accesseur des informations
 * @return config->GetInfos()
 */
QString WebApp::Infos() const
{
	return config->GetInfos();
}

/**
 * @brief Change les infos enregistrées dans le gestionnaire de fichier XML et modifie le fichier xml de l'application
 * @param infos
 */
void WebApp::SetInfos(const QString &infos)
{
	config->SetInfos(infos);
}

/**
 * @brief Accesseur de baseUrl
 * @return
 */
QStringList WebApp::GetBaseUrl() const
{
	return config->GetBaseUrl();
}

/**
 * @brief Change baseUrl à l'aide du gestionnaire de fichier XML
 * @param value
 */
void WebApp::SetBaseUrl(const QStringList &value)
{
	QStringList newBaseUrl = value;
	config->SetBaseUrl(newBaseUrl);
}

/**
 * @brief Indique si la page courante est dans baseUrl
 * @return Vrai si la page courante est dans baseUrl, faux sinon
 */
bool WebApp::IsPageInApplication()
{
	QString urls = m_webView->url().toString();
	QStringList::iterator i;
	QStringList baseUrl = config->GetBaseUrl();
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
	QString urls = url.toString();
	QStringList::iterator i;
	QStringList baseUrl = config->GetBaseUrl();
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
 * @return config->GetMinWidth()
 */
int WebApp::GetMinWidth()
{
	return config->GetMinWidth();
}

/**
 * @brief Renvoie la hauteur minimale de la fenêtre
 * @return config->GetMinHeight()
 */
int WebApp::GetMinHeight()
{
	return config->GetMinHeight();
}

/**
 * @brief Met à jour la taille minimale de la fenêtre
 * @param minWidth	Nouvelle largeur minimale
 * @param minHeight	Nouvelle hauteur minimale
 */
void WebApp::SetMinSize(int minWidth, int minHeight)
{
	config->SetMinSize(minWidth,minHeight);
}

/**
 * @brief Retourne la largeur par défaut de la fenêtre
 * @return config->GetDefaultWidth()
 */
int WebApp::GetDefaultWidth()
{
	return config->GetDefaultWidth();
}

/**
 * @brief Retourne la hauteur par défaut de la fenêtre
 * @return config->GetDefaultHeight()
 */
int WebApp::GetDefaultHeight()
{
	return config->GetDefaultHeight();
}

/**
 * @brief Met à jour la taille par défaut de la fenêtre
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void WebApp::SetDefaultSize(int defaultWidth, int defaultHeight)
{
	config->SetDefaultSize(defaultWidth,defaultHeight);
}
