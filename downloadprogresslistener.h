#ifndef DOWNLOADPROGRESSLISTENER_H
#define DOWNLOADPROGRESSLISTENER_H

#include <QtNetwork>

class DownloadProgressListener {
public:
    virtual void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int id);
    virtual void fileDownloaded(int id);
    virtual void downloadFailure(int id);
};
Q_DECLARE_INTERFACE(DownloadProgressListener,"com.webshell.downloadprogresslistener")

#endif // DOWNLOADPROGRESSLISTENER_H

