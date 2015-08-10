#include <QWebFrame>
#include <QProcess>
#include <QApplication>
#include "wnavigator.h"
#include "Outils/configmanager.h"

/**
 * @brief Constructeur de l'objet WNavigator
 * @param view	WebView sur laquelle effectuer des commandes JavaScript
 */
WNavigator::WNavigator(MyWebView *view, WebshellParameters *webshellParameters)
{
	this->m_webView = view;
	this->m_parameters = webshellParameters;
	m_parameters->setProperty("target","window");
}

/**
 * @brief Met à jour le webshell
 * @param url	Lien de téléchargement
 */
void WNavigator::UpdateSoftware(const QString &url)
{
	//La partie suivante permet de télécharger depuis la webshell
	data = new FileDownloader(url,qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief Initialisation des objets JavaScript de l'application
 */
void WNavigator::InitWebshellAPI()
{
	m_webView->page()->mainFrame()->evaluateJavaScript(\
	"navigator.UpdateSoftware = wnavigator.UpdateSoftware;\
	navigator.webshellParameters = webshellParameters;\
	navigator.Close = wnavigator.Close;\
	navigator.GetWebshellVersion = wnavigator.GetWebshellVersion;");
}

/**
 * @brief Ferme le webshell
 */
void WNavigator::Close()
{
	emit close();
}

/**
 * @brief Renvoie le numéro de version du webshell
 */
QString WNavigator::GetWebshellVersion()
{
	return qApp->applicationVersion();
}

/**
 * @brief Indique à l'application l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal	Nombre d'octets au total
 * @param id			Identifiant du FileDownloader
 */
void WNavigator::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, const QString &mime_type)
{
	if(m_webView->DispatchJsEvent("DownloadProgress",m_parameters->property("target").toString(),QStringList() << "typemime" << mime_type << "bytesReceived" << QString::number(bytesReceived) << "bytesTotal" << QString::number(bytesTotal))){}
}

/**
 * @brief Stocke et exécute l'installeur téléchargé
 * @param id	Identifiant du FileDownloader
 */
void WNavigator::FileDownloaded(const QString &mime_type)
{
	if(m_webView->DispatchJsEvent("DownloadComplete",m_parameters->property("target").toString(),QStringList() << "typemime" << mime_type)){}

	currentTypeMime = mime_type;
	//Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
	QString filename = data->GetUrl();
	filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
	currentFileDirectory = QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/");
	currentFileDirectory.append(filename);
	QFile file(currentFileDirectory);

	if(!file.open(QIODevice::WriteOnly))
	{
		qWarning() << "Fichier d'installation navigator impossible à ouvrir. Type mime: " << mime_type;
		return;
	}
	file.write(data->DownloadedData());
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
		if(m_webView->DispatchJsEvent("InstallError",m_parameters->property("target").toString(),QStringList() << "typemime" << mime_type)){}
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

	ConfigManager &config = ConfigManager::Instance();
	config.SetInstallationFileToRemove(currentFileDirectory);
	if(m_webView->DispatchJsEvent("NeedRestart",m_parameters->property("target").toString(),QStringList() << "typemime" << mime_type)){}
}

/**
 * @brief Signale l'application de l'échec du téléchargement
 * @param id	Identifiant du FileDownloader
 */
void WNavigator::DownloadFailure(const QString &mime_type)
{
	if(m_webView->DispatchJsEvent("DownloadFailure",m_parameters->property("target").toString(),QStringList() << "typemime" << mime_type)){}
}

/**
 * @brief Fonction appelée lors de la fin du processus d'installation\n
 * Comme l'application se ferme dans le cadre de cette installation, elle n'est théoriquement jamais appelée
 * @param exitCode		Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus	Indique si le processus a crashé ou non
 */
void WNavigator::finishInstall(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitCode!=0 || exitStatus == QProcess::CrashExit)
	{
		if(m_webView->DispatchJsEvent("InstallError",m_parameters->property("target").toString(),QStringList() << "typemime" << currentTypeMime)){}
	}
	else
	{
		if(m_webView->DispatchJsEvent("InstallSuccess",m_parameters->property("target").toString(),QStringList() << "typemime" << currentTypeMime)){}
	}
	QFile::remove(currentFileDirectory);
}
