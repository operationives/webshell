#include "updateoperations.h"
#include "global.h"

/**
 * Télécharge et exécute les installeurs en paramètre
 * @param urlList   Liste des liens de téléchargement pour des installeurs à exécuter
 */
void UdpdateOperations::updatePlugin(QStringList urlList){

    loop = new QEventLoop();
    for (QStringList::iterator i = urlList.begin(); i != urlList.end(); ++i){
        updateProcess(*i,0);
    }
    //Callback spécifiant la fin de l'installation
    ctrl->evaluateJavaScript(QString("fin_install()"));
}

/**
 * Télécharge et exécute l'installeur de la webshell
 * @param url   Lien du téléchargement pour la webshell à jour
 */
void UdpdateOperations::updateWebshell(QString url){
    loop = new QEventLoop();
    updateProcess(url,-1);
}

/**
 * Téléchargement et exécution d'un installeur
 * @param url   Lien du téléchargement vers un installeur
 * @param i     Si i==-1, la fonction est appelée par updateWebshell, sinon par updatePlugin
 */
void UdpdateOperations::updateProcess(QString url, int i){
    //La partie suivante permet de télécharger depuis la webshell
    QUrl updateUrl(url);
    data = new FileDownloader(updateUrl);
    connect(data, SIGNAL (downloaded()), this, SLOT (loadUpdate()));

    loop->exec();

    //Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
    QString filename = url;
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
        ctrl->evaluateJavaScript(QString("file_error()"));
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

    //Cas de updateWebshell
    if(i==-1){
        ctrl->evaluateJavaScript(QString("maj_webshell()"));
    }
    //Cas de updatePlugin
    else{
        loop->exec();
        QFile::remove(filedirectory);
    }
}

/**
 * Fonction appelée lors de la fin d'un téléchargement libérant la boucle
 */
void UdpdateOperations::loadUpdate(){
    //Libère loop mis en place après le lancement du téléchargement
    loop->quit();
    qDebug() << "Données reçues" << data->downloadedData().size();
}

/**
 * Fonction appelée lors de la fin d'un processus d'installation libérant la boucle
 * @param exitCode      Code de sortie du processus, indique si l'installation s'est bien déroulée (en temps normal)
 * @param exitStatus    Indique si le processus a crashé ou non
 */
void UdpdateOperations::finishInstall(int exitCode, QProcess::ExitStatus exitStatus){
    if(exitCode!=0 || exitStatus == QProcess::CrashExit){
        ctrl->evaluateJavaScript(QString("erreur()"));
    }
    else{
        ctrl->evaluateJavaScript(QString("success()"));
    }
    //Libère loop mis en place après le lancement de l'installation
    loop->quit();
}
