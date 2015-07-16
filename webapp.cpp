#include <QApplication>
#include "webapp.h"
#include "filedownloader.h"

//Régler: affectation de icon ne renvoie pas vers seticon
WebApp::WebApp(QWebView *view)
{
    this->view = view;
    baseUrl = new QList<QUrl>();
    this->setIcon(QUrl("http://i.stack.imgur.com/ILTQq.png"));
}

WebApp::~WebApp()
{
    delete baseUrl;
}

void WebApp::downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int id)
{

}

void WebApp::fileDownloaded(int id)
{
    QString filename = data->getUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    qDebug() << filedirectory;

    file.open(QIODevice::WriteOnly);
    file.write(data->downloadedData());
    file.close();

    QIcon icon(filedirectory);

    emit ChangeIcon(icon);
}

void WebApp::downloadFailure(int id)
{

}

/**
 * @brief WebApp::setIcon   Change la propriété icon et modifie l'icône de la barre de notification
 * @param icon  Url de l'icône de l'application
 */
void WebApp::setIcon(QUrl icon)
{
    this->icon = icon;
    data = new FileDownloader(icon,qobject_cast<DownloadProgressListener *>(this),-1);
}

QUrl WebApp::getIcon()
{
    return icon;
}


bool WebApp::ispageInApplication()
{
    QString urls = view->url().toString();
    QList<QUrl>::iterator i;
    bool res = false;
    for (i = baseUrl->begin(); i != baseUrl->end(); ++i){
        if(urls.startsWith((*i).toString())){
            res = true;
            break;
        }
    }
    return res;
}

bool WebApp::ispageInApplication(QUrl url)
{
    QString urls = url.toString();
    QList<QUrl>::iterator i;
    bool res = false;
    for (i = baseUrl->begin(); i != baseUrl->end(); ++i){
        if(urls.startsWith((*i).toString())){
            res = true;
            break;
        }
    }
    return res;

}
