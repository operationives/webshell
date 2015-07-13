#ifndef WNAVIGATOR_H
#define WNAVIGATOR_H

#include <QWebFrame>
#include <QProcess>
#include <QWebView>
#include <QtNetwork>
#include "filedownloader.h"

class WNavigator : public QObject{

    Q_OBJECT

public:
    WNavigator();
    Q_INVOKABLE void updatePlugin(QStringList urlList);
    Q_INVOKABLE void updateWebshell(QString url);
    Q_INVOKABLE void toto();

private slots:
    void loadUpdate();
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    FileDownloader *data;
    void updateProcess(QString url, int i);
    QEventLoop *loop;
};

#endif // WNAVIGATOR_H
