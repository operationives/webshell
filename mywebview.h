#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include <QWebView>
#include "cookiejar.h"
#include "webshellparameters.h"

class WNavigator;
class NavigatorPlugins;
class WebApp;

class MyWebView : public QWebView
{

	Q_OBJECT

public:
	MyWebView(QWidget *parent = 0);
	~MyWebView();
	bool DispatchJsEvent(const QString & evtType, const QString & evtTarget, const QStringList &keyValues = QStringList());
	CookieJar *m_cookieJar;

signals:
	void changeIcon(QIcon icon);
	void changeTitle(QString appName);
	void close();

private:
	QNetworkAccessManager *m_WebCtrl;
	WNavigator *wnavigator;
	NavigatorPlugins *navigatorplugins;
	WebApp *wapp;
	WebshellParameters *webshellParameters;


private slots:
	void handleRedirect(QUrl url);
	void updateJavaScriptObjects();
	void updateTitle();
};

#endif
