#include "updateoperations.h"
#include "global.h"

void UdpdateOperations::updatePlugin(QStringList urlList){

    loop = new QEventLoop();
    for (QStringList::iterator i = urlList.begin(); i != urlList.end(); ++i){
        //La partie suivante permet de télécharger depuis la webshell
        QUrl updateUrl(*i);
        data = new FileDownloader(updateUrl);
        connect(data, SIGNAL (downloaded()), this, SLOT (loadUpdate()));

        loop->exec();

        //Stockage des données téléchargées dans le fichier filename placé dans le répertoire filedirectory
        QString filename = *i;
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
            //Remplacer par une callback
            qDebug() << "Extension de fichier non répertoriée";
        }

        //Lancement du programme. Lorsqu'il finit, finishInstall est appelé
        QProcess *myProcess = new QProcess();
        connect(myProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finishInstall(int, QProcess::ExitStatus)));

        if(filename.endsWith(".pkg") || filename.endsWith(".dmg")){
            myProcess->startDetached(QLatin1String("open"),QStringList() << filedirectory);
        }
        else{
            myProcess->start(program);
        }

        //Test de lancement du processus
//        if (myProcess->waitForStarted(1000) == false)
//        qDebug() << "Error starting external program";

//        else
//        qDebug() << "external program running";

        loop->exec();
        QFile::remove(filedirectory);
    }
    //Callback spécifiant la fin de l'installation
    ctrl->evaluateJavaScript(QString("fin_install()"));
}

void UdpdateOperations::updateWebshell(QString url){
    loop = new QEventLoop();
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
        //Remplacer par une callback
        qDebug() << "Extension de fichier non répertoriée";
    }

    //Lancement du programme. Lorsqu'il finit, finishInstall est appelé
    QProcess *myProcess = new QProcess();
    connect(myProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finishInstall(int, QProcess::ExitStatus)));

    if(filename.endsWith(".pkg") || filename.endsWith(".dmg")){
        myProcess->startDetached(QLatin1String("open"),QStringList() << filedirectory);
    }
    else{
        myProcess->start(program);
    }

    ctrl->evaluateJavaScript(QString("maj_webshell()"));
}

void UdpdateOperations::loadUpdate(){
    //Libère loop mis en place après le lancement du téléchargement
    loop->quit();
    qDebug() << "Données reçues" << data->downloadedData().size();
}

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
