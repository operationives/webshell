#include <QApplication>
#include "webapp.h"
#include "filedownloader.h"
#include "global.h"


/**
 * @brief WebApp::WebApp Constructeur de l'objet WebApp
 * @param view  Page sur laquelle effectuer des commandes JavaScript
 */
WebApp::WebApp(QWebView *view)
{
    this->view = view;
    m_baseUrl = *config->GetBaseUrl();
}

/**
 * @brief WebApp::~WebApp Destructeur de l'objet WebApp
 */
WebApp::~WebApp()
{
}

/**
 * @brief WebApp::DownloadProgress Méthode de progression de téléchargement, actuellement rien à faire pour un téléchargement si petit
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal    Nombre d'octets total
 * @param mime_type     String non significatif
 */
void WebApp::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type)
{

}

/**
 * @brief WebApp::FileDownloaded Envoie un signal avec l'icône téléchargé
 * @param mime_type String non significatif
 */
void WebApp::FileDownloaded(QString mime_type)
{
    QString filename = data->GetUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    file.open(QIODevice::WriteOnly);
    file.write(data->DownloadedData());
    file.close();
    //delete data;

    QIcon icon(filedirectory);

    emit changeIcon(icon);
}

/**
 * @brief WebApp::DownloadFailure Méthode appelée lors de l'échec d'un téléchargement
 * @param mime_type String non significatif
 */
void WebApp::DownloadFailure(QString mime_type)
{

}

/**
 * @brief WebApp::Icon Accesseur de l'icône
 * @return m_icon
 */
QString WebApp::Icon() const
{
    return m_icon;
}

/**
 * @brief WebApp::setIcon   Change la propriété icon et modifie l'icône de la barre de notification
 * @param icon  Url de l'icône de l'application
 */
void WebApp::SetIcon(const QString &icon)
{
    m_icon = icon;
    data = new FileDownloader(QUrl(icon),qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief WebApp::GetBaseUrl Accesseur de baseUrl
 * @return
 */
QStringList WebApp::GetBaseUrl() const
{
    return m_baseUrl;
}

/**
 * @brief WebApp::SetBaseUrl Change baseUrl dans le fichier de config et dans l'application
 * @param value
 */
void WebApp::SetBaseUrl(const QStringList &value)
{
    m_baseUrl = value;
    config->SetBaseUrl(&m_baseUrl);
}

/**
 * @brief WebApp::IsPageInApplication Indique si la page courante est dans baseUrl
 * @return Vrai si la page courante est dans baseUrl, faux sinon
 */
bool WebApp::IsPageInApplication()
{
    QString urls = view->url().toString();
    QStringList::iterator i;
    bool res = false;
    for (i = m_baseUrl.begin(); i != m_baseUrl.end(); ++i)
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
 * @brief WebApp::IsPageInApplication Indique si url est dans baseUrl
 * @param url   url dont on veut tester l'appartenance à baseUrl
 * @return      Vrai si url est dans baseUrl, faux sinon
 */
bool WebApp::IsPageInApplication(QUrl url)
{
    QString urls = url.toString();
    QStringList::iterator i;
    bool res = false;
    for (i = m_baseUrl.begin(); i != m_baseUrl.end(); ++i)
    {
        if(urls.startsWith(*i))
        {
            res = true;
            break;
        }
    }
    return res;

}