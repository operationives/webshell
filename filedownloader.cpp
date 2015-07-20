#include "filedownloader.h"

/**
 * @brief FileDownloader::FileDownloader Constructeur permettant d'initialiser le téléchargement
 * @param Url Lien à télécharger
 */
FileDownloader::FileDownloader(QUrl Url, DownloadProgressListener *listener, QString mime_type)
{
    this->url = Url.toString();
    this->mime_type = mime_type;
    this->listener = listener;
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
    this, SLOT (fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(Url);
    QNetworkReply *download = m_WebCtrl.get(request);
    connect(download, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(download, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadFailure(QNetworkReply::NetworkError)));
}
 
/**
 * @brief FileDownloader::~FileDownloader Destructeur de FileDownloader
 */
FileDownloader::~FileDownloader()
{
}
 
/**
 * @brief FileDownloader::fileDownloaded Méthode de fin de téléchargement
 * @param pReply Réponse contenant les données téléchargées si tout s'est bien passé
 */
void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    listener->FileDownloaded(mime_type);
}

/**
 * @brief FileDownloader::downloadFailure Méthode signalant une erreur de téléchargement
 * @param error Type d'erreur reçue
 */
void FileDownloader::downloadFailure(QNetworkReply::NetworkError error)
{
    if(error != QNetworkReply::NoError)
        listener->DownloadFailure(mime_type);
}
 
/**
 * @brief FileDownloader::downloadedData Méthode d'accès aux données téléchargées
 * @return Octets téléchargés
 */
QByteArray FileDownloader::DownloadedData() const
{
    return m_DownloadedData;
}

/**
 * @brief FileDownloader::downloadProgress Indique l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets téléchargés
 * @param bytesTotal Nombre d'octets au total
 */
void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    listener->DownloadProgress(bytesReceived,bytesTotal,mime_type);
}

/**
 * @brief FileDownloader::GetMimeType Accesseur de mime_type
 * @return mime_type
 */
QString FileDownloader::GetMimeType()
{
    return mime_type;
}

/**
 * @brief FileDownloader::GetUrl Accesseur de l'url de téléchargement
 * @return url
 */
QString FileDownloader::GetUrl()
{
    return url;
}
