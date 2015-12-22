#ifndef MYWEBPAGE_H
#define MYWEBPAGE_H

#include <QWebPage>

class WebApp;

class MyWebPage : public QWebPage
{

	Q_OBJECT

public:
    MyWebPage(QObject* parent = 0) : QWebPage(parent) {}

protected:
    bool javaScriptConfirm(QWebFrame * frame, const QString & msg);
    void javaScriptAlert(QWebFrame * frame, const QString & msg);

//signals:

private:
    bool acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type);

//private slots:

};

#endif
