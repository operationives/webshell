#include <QWebFrame>
#include <QProcess>
#include <QApplication>
#include "wnavigator.h"

/**
 * @brief WNavigator::WNavigator Constructeur de l'objet WNavigator
 * @param view  WebView sur laquelle effectuer des commandes JavaScript
 */
WNavigator::WNavigator(MyWebView *view)
{
    this->m_webView = view;
    this->m_target = "window";
}

/**
 * @brief WNavigator::UpdateSoftware Met à jour le webshell
 * @param url   Lien de téléchargement
 */
void WNavigator::UpdateSoftware(QString url)
{
    //La partie suivante permet de télécharger depuis la webshell
    QUrl updateUrl(url);
    data = new FileDownloader(updateUrl,qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief Initialization Initialisation des objets JavaScript de l'application
 */
void WNavigator::InitWebshellAPI()
{
    m_webView->page()->mainFrame()->evaluateJavaScript(\
    "navigator.UpdateSoftware = wnavigator.UpdateSoftware");
}

/**
 * @brief WNavigator::DownloadProgress Indique à l'application l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal    Nombre d'octets au total
 * @param id            Identifiant du FileDownloader
 */
void WNavigator::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type)
{
    if(m_webView->DispatchJsEvent("DownloadProgress",m_target,QStringList() << "typemime" << mime_type << "bytesReceived" << QString::number(bytesReceived) << "bytesTotal" << QString::number(bytesTotal))){}
}

/**
 * @brief WNavigator::fileDownloaded Stocke et exécute l'installeur téléchargé
 * @param id    Identifiant du FileDownloader
 */
void WNavigator::FileDownloaded(QString mime_type)
{
    if(m_webView->DispatchJsEvent("DownloadComplete",m_target,QStringList() << "typemime" << mime_type)){}

    currentTypeMime = mime_type;
    //Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
    QString filename = data->GetUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    file.open(QIODevice::WriteOnly);
    file.write(data->DownloadedData());
    file.close();

    //Lancement du fichier téléchargé
    //Exécution de fichier dans un chemin précis: ne pas oublier les \" éventuels pour encadrer le chemin
    QString program;
    QString tmp = QString(filedirectory);
    if(filename.endsWith(".msi"))
    {
        tmp.replace("/","\\");
        program = "msiexec.exe /i \""+tmp+"\"";
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
        if(m_webView->DispatchJsEvent("InstallError",m_target,QStringList() << "typemime" << mime_type)){}
    }

    //Lancement du programme. Lorsqu'il finit, finishInstall est appelé
    QProcess *myProcess = new QProcess();
    connect(myProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finishInstall(int, QProcess::ExitStatus)));

    if(filename.endsWith(".pkg") || filename.endsWith(".dmg"))
    {
        myProcess->start("open "+filedirectory);
    }
    else
    {
        myProcess->start(program);
    }

    if(m_webView->DispatchJsEvent("NeedRestart",m_target,QStringList() << "typemime" << mime_type)){}
}

/**
 * @brief WNavigator::DownloadFailure Signale l'application de l'échec du téléchargement
 * @param id    Identifiant du FileDownloader
 */
void WNavigator::DownloadFailure(QString mime_type)
{
    if(m_webView->DispatchJsEvent("DownloadFailure",m_target,QStringList() << "typemime" << mime_type)){}
}

/**
 * @brief WNavigator::finishInstall Fonction appelée lors de la fin d'un processus d'installation libérant la boucle
 * @param exitCode      Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus    Indique si le processus a crashé ou non
 */
void WNavigator::finishInstall(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode!=0 || exitStatus == QProcess::CrashExit)
    {
        if(m_webView->DispatchJsEvent("InstallError",m_target,QStringList() << "typemime" << currentTypeMime)){}
    }
    else
    {
        if(m_webView->DispatchJsEvent("InstallSuccess",m_target,QStringList() << "typemime" << currentTypeMime)){}
    }
}

/**
 * @brief WNavigator::Target Retourne la cible des événements
 * @return m_target
 */
QString WNavigator::Target() const
{
    return m_target;
}

/**
 * @brief WNavigator::SetTarget met à jour la cible des événements
 * @param target    Nouvelle cible des événements
 */
void WNavigator::SetTarget(const QString &target)
{
    m_target = target;
}
