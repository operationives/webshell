#ifndef DOWNLOADPROGRESSLISTENER_H
#define DOWNLOADPROGRESSLISTENER_H

#include <QtNetwork>

class DownloadProgressListener
{
public:
	virtual void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &typemime)
	{
		Q_UNUSED(bytesReceived);
		Q_UNUSED(bytesTotal);
		Q_UNUSED(typemime);
	}
	virtual void FileDownloaded(const QString &typemime)
	{
		Q_UNUSED(typemime);
	}
	virtual void DownloadFailure(const QString &typemime)
	{
		Q_UNUSED(typemime);
	}
};
Q_DECLARE_INTERFACE(DownloadProgressListener,"com.webshell.downloadprogresslistener")

#endif // DOWNLOADPROGRESSLISTENER_H

