#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WebApp : public QObject, public DownloadProgressListener{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)

public:
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
    QString m_icon;
    QWebView *view;
    FileDownloader *data;
    QString icon() const;
    void setIcon(const QString &icon);
};

#endif // WEBAPP_H

