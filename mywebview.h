#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include <QWebView>
#include "mynetworkaccessmanager.h"
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

signals:
	void changeIcon(const QIcon &icon);
	void changeTitle(const QString &appName);
	void close();

private:
	QTimer *timer;
	MyNetworkAccessManager *m_WebCtrl;
	WNavigator *wnavigator;
	NavigatorPlugins *navigatorplugins;
	WebApp *wapp;
	WebshellParameters *webshellParameters;


private slots:
	void handleRedirect(const QUrl &url);
	void updateTitle();
	void updateJavaScriptObjects();
	void updateConnectivity();
};

#endif
