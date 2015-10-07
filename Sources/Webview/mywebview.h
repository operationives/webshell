#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include <QWebView>
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
	void LoadInternalPage(QString page);
    void SetUpdatingStatus(bool updatingStatus){isUpdating = updatingStatus;};
    bool IsUpdating() {return isUpdating;};

signals:
	void changeIcon(const QIcon &icon);
	void changeTitle(const QString &appName);
	void close();

private:
	QTimer *timer;
	WNavigator *wnavigator;
	NavigatorPlugins *navigatorplugins;
	WebApp *wapp;
	WebshellParameters *webshellParameters;
	bool connectionLost;
    bool isUpdating;

    QString IsCurrentUrlWithAnchor(QString url);


private slots:
	void handleRedirect(const QUrl &url);
	void updateTitle();
    void updateLogin();
	void updateJavaScriptObjects();
	void updateConnectivity();
};

#endif
