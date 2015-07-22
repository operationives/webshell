#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include <QWebView>

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
    void changeIcon(QIcon icon);

private:
    WNavigator *wnavigator;
    NavigatorPlugins *navigatorplugins;
    WebApp *wapp;
    bool firstPage;


private slots:
    void handleRedirect(QUrl url);
};

#endif
