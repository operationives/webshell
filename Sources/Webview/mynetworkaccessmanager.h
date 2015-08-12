#ifndef MYNETWORKACCESSMANAGER_H
#define MYNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include "cookiejar.h"

class MyNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	MyNetworkAccessManager();
	QNetworkReply *createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData);

public slots:
	void clearAll();

private:
	QNetworkDiskCache *m_webCache;
	CookieJar *m_cookieJar;

private slots:
	void getLanguage(QNetworkReply *reply);
};

#endif // MYNETWORKACCESSMANAGER_H

