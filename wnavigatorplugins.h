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
    Q_INVOKABLE void updateSoftware(QString url);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int id);
    void fileDownloaded(int id);
    void downloadFailure(int id);

private slots:
    void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QWebView *view;
    FileDownloader *data;
    static int cpt;
};

#endif // WNAVIGATORPLUGINS_H

