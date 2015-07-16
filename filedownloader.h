#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "downloadprogresslistener.h"
 
class FileDownloader : public QObject{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl Url, DownloadProgressListener *listener, int id);
    virtual ~FileDownloader();
    QByteArray downloadedData() const;
    int getId();
    QString getUrl();
 
signals:
    void downloaded();
 
private slots:
    void fileDownloaded(QNetworkReply* pReply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFailure(QNetworkReply::NetworkError error);
 
private:
    int id;
    QString url;
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
    DownloadProgressListener *listener;

};
 
#endif // FILEDOWNLOADER_H
