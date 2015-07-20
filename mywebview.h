#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWidgets>
#include "wnavigator.h"
#include "wnavigatorplugins.h"
#include "webapp.h"

class MyWebView : public QWebView
{

    Q_OBJECT

public:
    MyWebView(QWidget *parent = 0);
    ~MyWebView();

signals:
    void changeIcon(QIcon icon);

private:
    WNavigator *wnavigator;
    WNavigatorPlugins *wnavigatorplugins;
    WebApp *wapp;

private slots:
    void handleRedirect(QUrl url);
};

#endif
