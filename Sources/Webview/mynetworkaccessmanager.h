#ifndef MYNETWORKACCESSMANAGER_H
#define MYNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include "cookiejar.h"

class MyNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	static MyNetworkAccessManager *Instance();
	QNetworkReply *createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData);

public slots:
	void clearAll();

private:
	MyNetworkAccessManager& operator= (const MyNetworkAccessManager&){}
	MyNetworkAccessManager (const MyNetworkAccessManager&){}

	static MyNetworkAccessManager *m_instance;
	MyNetworkAccessManager();
	~MyNetworkAccessManager(){}

	QNetworkDiskCache *m_webCache;
	CookieJar *m_cookieJar;

private slots:
	void getLanguage(QNetworkReply *reply);
};

#endif // MYNETWORKACCESSMANAGER_H

