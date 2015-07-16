#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WebApp : public QObject, public DownloadProgressListener{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QUrl icon READ getIcon WRITE setIcon)

public:
    QUrl icon;
    QList<QUrl> *baseUrl;
    WebApp(QWebView *view);
    ~WebApp();
    bool ispageInApplication();
    bool ispageInApplication(QUrl url);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int id);
    void fileDownloaded(int id);
    void downloadFailure(int id);

signals:
    void ChangeIcon(QIcon icon);

private:
    QWebView *view;
    FileDownloader *data;
    void setIcon(QUrl icon);
    QUrl getIcon();
};

#endif // WEBAPP_H

