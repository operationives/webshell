#ifndef DOWNLOADPROGRESSLISTENER_H
#define DOWNLOADPROGRESSLISTENER_H

#include <QtNetwork>

class DownloadProgressListener {
public:
    virtual void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type);
    virtual void fileDownloaded(QString mime_type);
    virtual void downloadFailure(QString mime_type);
};
Q_DECLARE_INTERFACE(DownloadProgressListener,"com.webshell.downloadprogresslistener")

#endif // DOWNLOADPROGRESSLISTENER_H

