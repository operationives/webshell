#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
 
class FileDownloader : public QObject{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl Url);
    virtual ~FileDownloader();
    QByteArray downloadedData() const;
 
signals:
    void downloaded();
 
private slots:
    void fileDownloaded(QNetworkReply* pReply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
 
private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
};
 
#endif // FILEDOWNLOADER_H
