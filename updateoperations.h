#ifndef UPDATEOPERATIONS_H
#define UPDATEOPERATIONS_H

#include <QWebFrame>
#include <QProcess>
#include <QWebView>
#include <QtNetwork>
#include "filedownloader.h"

class UdpdateOperations : public QObject{

    Q_OBJECT

public:
    Q_INVOKABLE void updatePlugin(QStringList urlList);
    Q_INVOKABLE void updateWebshell(QString url);
    QEventLoop *loop;

private slots:
    void loadUpdate();
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    FileDownloader *data;
};

#endif // UPDATEOPERATIONS_H
