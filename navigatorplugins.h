#ifndef NavigatorPlugins_H
#define NavigatorPlugins_H

#include "filedownloader.h"
#include "downloadprogresslistener.h"
#include "semaphore.h"
#include "mywebview.h"

class NavigatorPlugins : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QString target READ Target WRITE SetTarget)

public:
    NavigatorPlugins(MyWebView *view);
    ~NavigatorPlugins();
    Q_INVOKABLE void UpdateSoftware(QString url, QString mime_type);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);

public slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    MyWebView *m_webView;
    QString m_target;
    QString currentTypeMime;
    QHash<QString, FileDownloader *> hash;
    Semaphore *sem;
    QString Target() const;
    void SetTarget(const QString &target);
};

#endif // NavigatorPlugins_H