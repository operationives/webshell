#include "filedownloader.h"
#include "updateoperations.h"
#include "global.h"

FileDownloader::FileDownloader(QUrl Url){
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
    this, SLOT (fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(Url);
    QNetworkReply *download = m_WebCtrl.get(request);
    connect(download, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
}
 
FileDownloader::~FileDownloader() { }
 
void FileDownloader::fileDownloaded(QNetworkReply* pReply){
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}
 
QByteArray FileDownloader::downloadedData() const{
    return m_DownloadedData;
}

void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal){
    ctrl->evaluateJavaScript(QString("update(%1,%2)").arg(QString::number(bytesReceived),QString::number(bytesTotal)));
}
