#ifndef WNAVIGATOR_H
#define WNAVIGATOR_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WNavigator : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)

public:
    WNavigator(QWebView *view);
    Q_INVOKABLE void UpdateSoftware(QString url);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);

private slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWebView *view;
    FileDownloader *data;
};

#endif // WNAVIGATOR_H
