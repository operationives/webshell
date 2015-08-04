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
	void clearCookies();

private:
	QNetworkDiskCache *m_webCache;
	CookieJar *m_cookieJar;
};

#endif // MYNETWORKACCESSMANAGER_H

