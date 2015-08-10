#include "filedownloader.h"

/**
 * @brief Constructeur permettant d'initialiser le téléchargement
 * @param url		Adresse du téléchargement
 * @param listener	Objet à signaler pour les avancements du téléchargement
 * @param typemime	Identifiant du téléchargement
 */
FileDownloader::FileDownloader(const QString &url, DownloadProgressListener *listener, const QString &typemime)
{
	this->url = QString(url);
	this->typemime = typemime;
	this->listener = listener;
	connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
	this, SLOT (fileDownloaded(QNetworkReply*)));

	QNetworkRequest request(QUrl(this->url));
	download = m_WebCtrl.get(request);
	connect(download, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
	connect(download, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadFailure(QNetworkReply::NetworkError)));
}

/**
 * @brief Si le téléchargement est en cours, on le quitte
 */
FileDownloader::~FileDownloader()
{
	if(download->isRunning())
		download->abort();
}
 
/**
 * @brief Prévient le DownloadProgressListener que le téléchargement est fini
 * @param pReply Réponse contenant les données téléchargées si tout s'est bien passé
 */
void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
	m_DownloadedData = pReply->readAll();
	pReply->deleteLater();
	listener->FileDownloaded(typemime);
}

/**
 * @brief Méthode signalant une erreur de téléchargement au DownloadProgressListener
 * @param error Type d'erreur reçue
 */
void FileDownloader::downloadFailure(QNetworkReply::NetworkError error)
{
	if(error != QNetworkReply::NoError)
		listener->DownloadFailure(typemime);
}
 
/**
 * @brief Méthode d'accès aux données téléchargées
 * @return Octets téléchargés
 */
QByteArray FileDownloader::DownloadedData() const
{
	return m_DownloadedData;
}

/**
 * @brief Indique l'avancement du téléchargement au DownloadProgressListener
 * @param bytesReceived Nombre d'octets téléchargés
 * @param bytesTotal Nombre d'octets au total
 */
void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	listener->DownloadProgress(bytesReceived,bytesTotal,typemime);
}

/**
 * @brief Accesseur du type mime
 * @return typemime
 */
QString FileDownloader::GetMimeType()
{
	return typemime;
}

/**
 * @brief Accesseur de l'url de téléchargement
 * @return url
 */
QString FileDownloader::GetUrl()
{
	return url;
}
