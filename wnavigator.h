#ifndef WNAVIGATOR_H
#define WNAVIGATOR_H

#include "filedownloader.h"
#include "downloadprogresslistener.h"
#include "mywebview.h"

class WNavigator : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QString target READ Target WRITE SetTarget)

public:
    WNavigator(MyWebView *view);
    Q_INVOKABLE void UpdateSoftware(QString url);
    Q_INVOKABLE void InitWebshellAPI();

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);

private slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    MyWebView *m_webView;
    QString m_target;
    QString currentTypeMime;
    FileDownloader *data;
    QString Target() const;
    void SetTarget(const QString &target);
};

#endif // WNAVIGATOR_H
