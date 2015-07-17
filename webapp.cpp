#include <QApplication>
#include "webapp.h"
#include "filedownloader.h"

//Régler: affectation de icon ne renvoie pas vers seticon
WebApp::WebApp(QWebView *view)
{
    this->view = view;
    baseUrl = new QList<QUrl>();
    //this->setProperty("icon","http://i.stack.imgur.com/ILTQq.png");
}

WebApp::~WebApp()
{
    delete baseUrl;
}

void WebApp::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type)
{

}

void WebApp::FileDownloaded(QString mime_type)
{
    QString filename = data->GetUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    qDebug() << filedirectory;

    file.open(QIODevice::WriteOnly);
    file.write(data->DownloadedData());
    file.close();
    //delete data;

    QIcon icon(filedirectory);

    emit changeIcon(icon);
}

void WebApp::DownloadFailure(QString mime_type)
{

}

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

bool WebApp::IsPageInApplication()
{
    QString urls = view->url().toString();
    QList<QUrl>::iterator i;
    bool res = false;
    for (i = baseUrl->begin(); i != baseUrl->end(); ++i)
    {
        if(urls.startsWith((*i).toString()))
        {
            res = true;
            break;
        }
    }
    return res;
}

bool WebApp::IsPageInApplication(QUrl url)
{
    QString urls = url.toString();
    QList<QUrl>::iterator i;
    bool res = false;
    for (i = baseUrl->begin(); i != baseUrl->end(); ++i)
    {
        if(urls.startsWith((*i).toString()))
        {
            res = true;
            break;
        }
    }
    return res;

}
