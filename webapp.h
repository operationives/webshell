#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebView>
#include "filedownloader.h"
#include "downloadprogresslistener.h"

class WebApp : public QObject, public DownloadProgressListener
{

    Q_OBJECT
    Q_INTERFACES(DownloadProgressListener)
    Q_PROPERTY(QString icon READ Icon WRITE SetIcon)
//    Q_PROPERTY(QList<QString> *baseUrl READ GetBaseUrl WRITE SetBaseUrl)

public:
    //A FAIRE: placer baseUrl en Q_PROPERTY, afin d'avoir un set et get adaptés à l'application
    QList<QString> *baseUrl;
    WebApp(QWebView *view);
    ~WebApp();
    bool IsPageInApplication();
    bool IsPageInApplication(QUrl url);

    void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    void FileDownloaded(QString mime_type);
    void DownloadFailure(QString mime_type);


signals:
    void changeIcon(QIcon icon);

private slots:
    void updateBaseUrl();

private:
    QString m_icon;
    QWebView *view;
    FileDownloader *data;
    QString Icon() const;
    void SetIcon(const QString &icon);
//    QList<QString> *m_baseUrl;
//    QList<QString> *GetBaseUrl() const;
//    void SetBaseUrl(QList<QString> *value);
};

#endif // WEBAPP_H

