#include "filedownloader.h"
#include "global.h"

/**
 * @brief FileDownloader::FileDownloader Constructeur permettant d'initialiser le téléchargement
 * @param Url Lien à télécharger
 */
FileDownloader::FileDownloader(QUrl Url){
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
    this, SLOT (fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(Url);
    QNetworkReply *download = m_WebCtrl.get(request);
    connect(download, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
}
 
/**
 * @brief FileDownloader::~FileDownloader Destructeur de FileDownloader
 */
FileDownloader::~FileDownloader() { }
 
/**
 * @brief FileDownloader::fileDownloaded Méthode de fin de téléchargement
 * @param pReply Réponse contenant les données téléchargées si tout s'est bien passé
 */
void FileDownloader::fileDownloaded(QNetworkReply* pReply){
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}
 
/**
 * @brief FileDownloader::downloadedData Méthode d'accès aux données téléchargées
 * @return Octets téléchargés
 */
QByteArray FileDownloader::downloadedData() const{
    return m_DownloadedData;
}

/**
 * @brief FileDownloader::downloadProgress Indique l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets téléchargés
 * @param bytesTotal Nombre d'octets au total
 */
void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal){
    ctrl->evaluateJavaScript(QString("update(%1,%2)").arg(QString::number(bytesReceived),QString::number(bytesTotal)));
}
