#ifndef WEBAPP_H
#define WEBAPP_H

#include "filedownloader.h"
#include "downloadprogresslistener.h"
#include "mywebview.h"

class WebApp : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QStringList baseUrl READ GetBaseUrl WRITE SetBaseUrl)
    Q_PROPERTY(QString icon READ Icon WRITE SetIcon)
    Q_PROPERTY(QString infos READ Infos WRITE SetInfos)
    Q_PROPERTY(QString target READ Target WRITE SetTarget)

public:
    WebApp(MyWebView *view);
    ~WebApp();
    bool IsPageInApplication();
    bool IsPageInApplication(QUrl url);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);


signals:
    void changeIcon(QIcon icon);
    void changeTitle(QString appName);

private:
    QString m_icon;
    QString m_infos;
    QString m_target;
    MyWebView *m_webView;
    FileDownloader *data;
    QString Icon() const;
    void SetIcon(const QString &icon);
    QString Infos() const;
    void SetInfos(const QString &infos);
    QStringList m_baseUrl;
    QStringList GetBaseUrl() const;
    void SetBaseUrl(const QStringList &value);
    QString Target() const;
    void SetTarget(const QString &target);
};

#endif // WEBAPP_H

