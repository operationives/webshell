#ifndef DOWNLOADPROGRESSLISTENER_H
#define DOWNLOADPROGRESSLISTENER_H

#include <QtNetwork>

class DownloadProgressListener
{
public:
	virtual void DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &mime_type);
	virtual void FileDownloaded(const QString &mime_type);
	virtual void DownloadFailure(const QString &mime_type);
};
Q_DECLARE_INTERFACE(DownloadProgressListener,"com.webshell.downloadprogresslistener")

#endif // DOWNLOADPROGRESSLISTENER_H

