#ifndef WNAVIGATOR_H
#define WNAVIGATOR_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WNavigator : public QObject, public DownloadProgressListener{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)

public:
    WNavigator(QWebView *view);
    Q_INVOKABLE void updateSoftware(QString url);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int id);
    void fileDownloaded(int id);
    void downloadFailure(int id);

private slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWebView *view;
    FileDownloader *data;
};

#endif // WNAVIGATOR_H
