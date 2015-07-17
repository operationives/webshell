#include <QWebFrame>
#include <QProcess>
#include <QApplication>
#include "wnavigator.h"

WNavigator::WNavigator(QWebView *view){
    this->view = view;
}

/**
 * @brief WNavigator::updateSoftware Met à jour le webshell
 * @param url   Lien de téléchargement
 */
void WNavigator::updateSoftware(QString url){
    //La partie suivante permet de télécharger depuis la webshell
    QUrl updateUrl(url);
    data = new FileDownloader(updateUrl,qobject_cast<DownloadProgressListener *>(this),"");
}

/**
 * @brief WNavigator::downloadProgress Indique à l'application l'avancement du téléchargement
 * @param bytesReceived Nombre d'octets reçus
 * @param bytesTotal    Nombre d'octets au total
 * @param id            Identifiant du FileDownloader
 */
void WNavigator::downloadProgress(qint64 bytesReceived, qint64 bytesTotal, QString mime_type){
    if(!mime_type.isNull()){}
    view->page()->mainFrame()->evaluateJavaScript(QString("update(%1,%2)").arg(QString::number(bytesReceived),QString::number(bytesTotal)));
}

/**
 * @brief WNavigator::fileDownloaded Stocke et exécute l'installeur téléchargé
 * @param id    Identifiant du FileDownloader
 */
void WNavigator::fileDownloaded(QString mime_type){
    if(!mime_type.isNull()){}
    view->page()->mainFrame()->evaluateJavaScript(QString("download_done()"));

    //Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
    QString filename = data->getUrl();
    filename =  filename.right(filename.length() - filename.lastIndexOf("/") - 1);
    QString filedirectory = QString(QApplication::applicationDirPath()+"/");
    filedirectory.append(filename);
    QFile file(filedirectory);

    file.open(QIODevice::WriteOnly);
    file.write(data->downloadedData());
    file.close();

    //Lancement du fichier téléchargé
    //Exécution de fichier dans un chemin précis: ne pas oublier les \" éventuels pour encadrer le chemin
    QString program;
    QString tmp = QString(filedirectory);
    if(filename.endsWith(".msi")){
        tmp.replace("/","\\");
        program = "msiexec.exe /i \""+tmp+"\"";
    }
    else if(filename.endsWith(".exe")){
        tmp.replace("/","\\");
        program = "\""+tmp+"\"";
    }
    else if(filename.endsWith(".pkg") || filename.endsWith(".dmg")){
        //Rien à faire
    }
    else {
        view->page()->mainFrame()->evaluateJavaScript(QString("file_error()"));
    }

    //Lancement du programme. Lorsqu'il finit, finishInstall est appelé
    QProcess *myProcess = new QProcess();
    connect(myProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finishInstall(int, QProcess::ExitStatus)));

    if(filename.endsWith(".pkg") || filename.endsWith(".dmg")){
        myProcess->start("open "+filedirectory);
    }
    else{
        myProcess->start(program);
    }

    view->page()->mainFrame()->evaluateJavaScript(QString("maj_webshell()"));
}

/**
 * @brief WNavigator::downloadFailure Signale l'application de l'échec du téléchargement
 * @param id    Identifiant du FileDownloader
 */
void WNavigator::downloadFailure(QString mime_type){
    if(!mime_type.isNull()){}
    view->page()->mainFrame()->evaluateJavaScript(QString("download_fail()"));

}

/**
 * @brief WNavigator::finishInstall Fonction appelée lors de la fin d'un processus d'installation libérant la boucle
 * @param exitCode      Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus    Indique si le processus a crashé ou non
 */
void WNavigator::finishInstall(int exitCode, QProcess::ExitStatus exitStatus){
    if(exitCode!=0 || exitStatus == QProcess::CrashExit){
        view->page()->mainFrame()->evaluateJavaScript(QString("erreur()"));
    }
    else{
        view->page()->mainFrame()->evaluateJavaScript(QString("success()"));
    }
}
