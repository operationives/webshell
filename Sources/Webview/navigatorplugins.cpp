#include <QWebFrame>
#include <QProcess>
#include <QApplication>
#include "navigatorplugins.h"


/**
 * @brief Constructeur de l'objet NavigatorPlugins
 * @param view  WebView sur laquelle effectuer des commandes JavaScript
 */
NavigatorPlugins::NavigatorPlugins(MyWebView *view)
{
	this->m_webView = view;
	this->m_target = "window";
	//Le sémaphore est utile afin de mettre en pause les threads cherchant à exécuter
	//un installeur téléchargé alors qu'un autre est déjà en cours d'exécution
	sem = new Semaphore();
}


NavigatorPlugins::~NavigatorPlugins()
{
	delete sem;
}

/**
 * @brief Met à jour le webshell
 * @param url   Lien de téléchargement
 */
void NavigatorPlugins::UpdateSoftware(QString url, QString typemime)
{
	//La partie suivante permet de télécharger depuis la webshell
	fileDownloaderHash[typemime] = new FileDownloader(url,qobject_cast<DownloadProgressListener *>(this),typemime);
}

/**
 * @brief Indique à l'application l'avancement du téléchargement par un événement JavaScript
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal	Nombre d'octets au total
 * @param id			Identifiant du FileDownloader
 */
void NavigatorPlugins::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &typemime)
{
	if(m_webView->DispatchJsEvent("DownloadProgress",m_target,QStringList() << "typemime" << typemime << "bytesReceived" << QString::number(bytesReceived) << "bytesTotal" << QString::number(bytesTotal))){}
}

/**
 * @brief Stocke et exécute l'installeur téléchargé
 * @param id	Identifiant du FileDownloader
 */
void NavigatorPlugins::FileDownloaded(const QString &typemime)
{
	if(m_webView->DispatchJsEvent("DownloadComplete",m_target,QStringList() << "typemime" << typemime)){}

	if(!sem->Acquire())
		return;
	currentTypeMime = typemime;
	//Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
	QString filename = fileDownloaderHash.value(typemime)->GetUrl();
	filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
	currentFileDirectory = QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/");
	currentFileDirectory.append(filename);
	QFile file(currentFileDirectory);

	if(!file.open(QIODevice::WriteOnly))
	{
		qWarning() << "Fichier d'installation navigatorPlugins impossible à ouvrir. Type mime: " << typemime;
		return;
	}

	file.write(fileDownloaderHash.value(typemime)->DownloadedData());
	file.close();

	//Lancement du fichier téléchargé
	//Exécution de fichier dans un chemin précis: ne pas oublier les \" éventuels pour encadrer le chemin
	QString program;
	QString tmp = QString(currentFileDirectory);
	if(filename.endsWith(".msi"))
	{
		tmp.replace("/","\\");
		program = "msiexec.exe /i \""+tmp+"\" /qb";
	}
	else if(filename.endsWith(".exe"))
	{
		tmp.replace("/","\\");
		program = "\""+tmp+"\"";
	}
	else if(filename.endsWith(".pkg") || filename.endsWith(".dmg"))
	{
		//Rien à faire
	}
	else
	{
		if(m_webView->DispatchJsEvent("InstallError",m_target,QStringList() << "typemime" << typemime)){}
	}

	//Lancement du programme. Lorsqu'il finit, finishInstall est appelé
	QProcess *myProcess = new QProcess();
	connect(myProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finishInstall(int, QProcess::ExitStatus)));

	if(filename.endsWith(".pkg") || filename.endsWith(".dmg"))
	{
		myProcess->start("open "+currentFileDirectory);
	}
	else
	{
		myProcess->start(program);
	}
}

/**
 * @brief Signale l'application de l'échec du téléchargement
 * @param id	Identifiant du FileDownloader
 */
void NavigatorPlugins::DownloadFailure(const QString &typemime)
{
	if(m_webView->DispatchJsEvent("DownloadFailure",m_target,QStringList() << "typemime" << typemime)){}
}

/**
 * @brief Fonction appelée lors de la fin d'un processus d'installation\n
 * Efface le fichier d'installation utilisé\n
 * Prévient l'application si l'installation s'est correctement déroulée ou non
 * @param exitCode		Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus	Indique si le processus a crashé ou non
 */
void NavigatorPlugins::finishInstall(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitCode!=0 || exitStatus == QProcess::CrashExit)
	{
		if(m_webView->DispatchJsEvent("InstallError",m_target,QStringList() << "typemime" << currentTypeMime)){}
	}
	else
	{
		if(m_webView->DispatchJsEvent("InstallSuccess",m_target,QStringList() << "typemime" << currentTypeMime)){}
	}
	QFile::remove(currentFileDirectory);
	sem->Release();
}

/**
 * @brief Retourne la cible des événements
 * @return m_target
 */
QString NavigatorPlugins::Target() const
{
	return m_target;
}

/**
 * @brief Met à jour la cible des événements
 * @param target	Nouvelle cible des événements
 */
void NavigatorPlugins::SetTarget(const QString &target)
{
	m_target = target;
}
