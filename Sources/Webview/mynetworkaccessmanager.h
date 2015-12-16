#ifndef MYNETWORKACCESSMANAGER_H
#define MYNETWORKACCESSMANAGER_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include "cookiejar.h"

class MyNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	static MyNetworkAccessManager *Instance();
	QNetworkReply *createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData);
    CookieJar *getCookieJar();

public slots:
	void clearAll();
    void cookieLoaded(QString name, QString value);

private:
	MyNetworkAccessManager& operator= (const MyNetworkAccessManager&){}
	MyNetworkAccessManager (const MyNetworkAccessManager&){}

	static MyNetworkAccessManager *m_instance;
	MyNetworkAccessManager();
	~MyNetworkAccessManager(){}

	QNetworkDiskCache *m_webCache;
	CookieJar *m_cookieJar;
    QString m_pending_login;

//signals:
//    void cancelRequest();

private slots:
	void getLanguage(QNetworkReply *reply);
    //void handlReplyError(QNetworkReply::NetworkError error);
};

#endif // MYNETWORKACCESSMANAGER_H

