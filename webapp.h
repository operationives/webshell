#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WebApp : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QStringList baseUrl READ GetBaseUrl WRITE SetBaseUrl)
    Q_PROPERTY(QString icon READ Icon WRITE SetIcon)

public:
    //A FAIRE: placer baseUrl en Q_PROPERTY, afin d'avoir un set et get adaptés à l'application
//    QList<QString> *baseUrl;
    WebApp(QWebView *view);
    ~WebApp();
    bool IsPageInApplication();
    bool IsPageInApplication(QUrl url);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);


signals:
    void changeIcon(QIcon icon);

private:
    QString m_icon;
    QWebView *view;
    FileDownloader *data;
    QString Icon() const;
    void SetIcon(const QString &icon);
    QStringList m_baseUrl;
    QStringList GetBaseUrl() const;
    void SetBaseUrl(const QStringList &value);
};

#endif // WEBAPP_H

