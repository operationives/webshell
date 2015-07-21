#ifndef NavigatorPlugins_H
#define NavigatorPlugins_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"
#include "semaphore.h"

class NavigatorPlugins : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)

public:
    NavigatorPlugins(QWebView *view);
    ~NavigatorPlugins();
    Q_INVOKABLE void UpdateSoftware(QString url, QString mime_type);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);

public slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWebView *view;
    QHash<QString, FileDownloader *> hash;
    Semaphore *sem;
};

#endif // NavigatorPlugins_H
