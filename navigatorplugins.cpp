#include <QWebFrame>
#include <QProcess>
#include <QApplication>
#include "navigatorplugins.h"


/**
 * @brief NavigatorPlugins::NavigatorPlugins Constructeur de l'objet NavigatorPlugins
 * @param view  WebView sur laquelle effectuer des commandes JavaScript
 */
NavigatorPlugins::NavigatorPlugins(QWebView *view)
{
    this->view = view;
    sem = new Semaphore();
}


NavigatorPlugins::~NavigatorPlugins()
{
    delete sem;
}

/**
 * @brief NavigatorPlugins::UpdateSoftware Met à jour le webshell
 * @param url   Lien de téléchargement
 */
void NavigatorPlugins::UpdateSoftware(QString url, QString mime_type)
{
    //La partie suivante permet de télécharger depuis la webshell
    QUrl updateUrl(url);
    hash[mime_type] = new FileDownloader(updateUrl,qobject_cast<DownloadProgressListener *>(this),mime_type);
}

/**
 * @brief NavigatorPlugins::DownloadProgress Indique à l'application l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal    Nombre d'octets au total
 * @param id            Identifiant du FileDownloader
 */
void NavigatorPlugins::DownloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type)
{
    if(!mime_type.isNull()){}
    view->page()->mainFrame()->evaluateJavaScript(QString("update(%1,%2)").arg(QString::number(bytesReceived),QString::number(bytesTotal)));
}

/**
 * @brief NavigatorPlugins::fileDownloaded Stocke et exécute l'installeur téléchargé
 * @param id    Identifiant du FileDownloader
 */
void NavigatorPlugins::FileDownloaded(QString mime_type)
{
    view->page()->mainFrame()->evaluateJavaScript(QString("download_done()"));

    sem->Acquire();
    //Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
    QString filename = hash.value(mime_type)->GetUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    file.open(QIODevice::WriteOnly);
    file.write(hash.value(mime_type)->DownloadedData());
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
        view->page()->mainFrame()->evaluateJavaScript(QString("file_error()"));
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

    view->page()->mainFrame()->evaluateJavaScript(QString("maj_webshell()"));
}

/**
 * @brief NavigatorPlugins::DownloadFailure Signale l'application de l'échec du téléchargement
 * @param id    Identifiant du FileDownloader
 */
void NavigatorPlugins::DownloadFailure(QString mime_type)
{
    if(!mime_type.isNull()){}
    view->page()->mainFrame()->evaluateJavaScript(QString("download_fail()"));

}

/**
 * @brief NavigatorPlugins::finishInstall Fonction appelée lors de la fin d'un processus d'installation libérant la boucle
 * @param exitCode      Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus    Indique si le processus a crashé ou non
 */
void NavigatorPlugins::finishInstall(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode!=0 || exitStatus == QProcess::CrashExit)
    {
        view->page()->mainFrame()->evaluateJavaScript(QString("erreur()"));
    }
    else
    {
        view->page()->mainFrame()->evaluateJavaScript(QString("success()"));
    }

    sem->Release();
}

