#ifndef WEBAPP_H
#define WEBAPP_H

#include "Outils/filedownloader.h"
#include "downloadprogresslistener.h"
#include "mywebview.h"

class WebApp : public QObject, public DownloadProgressListener
{

	Q_OBJECT
	Q_INTERFACES(DownloadProgressListener)
	Q_PROPERTY(QStringList baseUrl READ GetBaseUrl WRITE SetBaseUrl)
	Q_PROPERTY(QString icon READ Icon WRITE SetIcon)
	Q_PROPERTY(QString infos READ Infos WRITE SetInfos)
    Q_PROPERTY(QString display_name READ Infos WRITE SetDisplayName)
    Q_PROPERTY(QString target READ Target WRITE SetTarget)

public:
	WebApp(MyWebView *view);
	~WebApp();
	bool IsPageInApplication();
	Q_INVOKABLE bool IsPageInApplication(QUrl url);
	Q_INVOKABLE int GetMinWidth();
	Q_INVOKABLE int GetMinHeight();
	Q_INVOKABLE int GetDefaultWidth();
	Q_INVOKABLE int GetDefaultHeight();
	Q_INVOKABLE void SetMinSize(int minWidth, int minHeight);
	Q_INVOKABLE void SetDefaultSize(int defaultWidth, int defaultHeight);

	void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &typemime);
	void FileDownloaded(const QString &typemime);
	void DownloadFailure(const QString &typemime);


signals:
	void changeIcon(const QIcon &icon);

private:
	QString m_target;
	QString currentFileDirectory;
	MyWebView *m_webView;
	FileDownloader *data;
	QString Icon() const;
	void SetIcon(const QString &icon);
	QString Infos() const;
    void SetInfos(const QString &infos);
    QString DisplayName() const;
    void SetDisplayName(const QString &displayName);
	QStringList GetBaseUrl() const;
	void SetBaseUrl(const QStringList &value);
	QString Target() const;
	void SetTarget(const QString &target);
};

#endif // WEBAPP_H

