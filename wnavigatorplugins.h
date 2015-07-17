#ifndef WNAVIGATORPLUGINS_H
#define WNAVIGATORPLUGINS_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WNavigatorPlugins : public QObject, public DownloadProgressListener{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)

public:
    WNavigatorPlugins(QWebView *view);
    Q_INVOKABLE void updateSoftware(QString url, QString mime_type);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void fileDownloaded(QString mime_type);
    void downloadFailure(QString mime_type);

public slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWebView *view;
    QHash<QString, FileDownloader *> hash;
    QSemaphore *sem;
};

#endif // WNAVIGATORPLUGINS_H

