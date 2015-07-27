#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "downloadprogresslistener.h"
 
class FileDownloader : public QObject
{
	Q_OBJECT
public:
	explicit FileDownloader(QUrl Url, DownloadProgressListener *listener, QString mime_type);
	virtual ~FileDownloader();
	QByteArray DownloadedData() const;
	QString GetMimeType();
	QString GetUrl();
 
private slots:
	void fileDownloaded(QNetworkReply* pReply);
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFailure(QNetworkReply::NetworkError error);
 
private:
	QString mime_type;
	QString url;
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
	DownloadProgressListener *listener;

};
 
#endif // FILEDOWNLOADER_H
