#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
#include <QtWidgets>

class MyWebView : public QWebView{

    Q_OBJECT

public:
    MyWebView(QWidget *parent = 0);

private:
    QString baseUrl;

private slots:
    void handleRedirect(QUrl url);
};

#endif
