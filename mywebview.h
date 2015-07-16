#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
#include <QtWidgets>
#include "wnavigator.h"
#include "webapp.h"

class MyWebView : public QWebView
{

    Q_OBJECT

public:
    MyWebView(QWidget *parent = 0);
    ~MyWebView();

signals:
    void ChangeIcon(QIcon icon);

private:
    WNavigator *wnavigator;
    WebApp *wapp;

private slots:
    void handleRedirect(QUrl url);
};

#endif
