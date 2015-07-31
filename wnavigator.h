#ifndef WNAVIGATOR_H
#define WNAVIGATOR_H

#include "filedownloader.h"
#include "downloadprogresslistener.h"
#include "mywebview.h"
#include "webshellparameters.h"

class WNavigator : public QObject, public DownloadProgressListener
{

	Q_OBJECT
	Q_INTERFACES(DownloadProgressListener)

public:
	WNavigator(MyWebView *view, WebshellParameters *webshellParameters);
	Q_INVOKABLE void UpdateSoftware(const QString &url);
	Q_INVOKABLE void InitWebshellAPI();
	Q_INVOKABLE void Close();

	void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &mime_type);
	void FileDownloaded(const QString &mime_type);
	void DownloadFailure(const QString &mime_type);

signals:
	void close();

private slots:
	void finishInstall(int exitCode, QProcess::ExitStatus exitStatus);

private:
	MyWebView *m_webView;
	WebshellParameters *m_parameters;
	QString currentTypeMime;
	FileDownloader *data;
};

#endif // WNAVIGATOR_H
