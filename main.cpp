#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "mainwindow.h"
#include "global.h"

//A faire: gestion de la taille maximale du fichier .log
/**
 * @brief myMessageOutput Gestion des sorties consoles à placer dans les logs
 * @param type          Catégorie de message
 * @param logcontext    Contexte (vide?)
 * @param msg           Contenu du message
 */
void myMessageOutput(QtMsgType type, const QMessageLogContext & logcontext,const QString & msg)
{
    QFile file(QApplication::applicationDirPath()+"/"+qAppName()+".log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    file.write(QString("[").toLatin1()+QDateTime::currentDateTime().toString().toLatin1()+QString("] ").toLatin1());
    std::string text = msg.toLatin1().constData();
    std::cout << text << "\n";
    std::cout.flush();
    switch (type)
    {
    case QtDebugMsg:
        file.write(QString("Debug: ").toLatin1()+msg.toLatin1()+QString(" LogContext: ").toLatin1()+QString(logcontext.function).toLatin1()+QString("\r\n").toLatin1());
        break;
    case QtWarningMsg:
        file.write(QString("Warning: ").toLatin1()+msg.toLatin1()+QString(" LogContext: ").toLatin1()+QString(logcontext.function).toLatin1()+QString("\r\n").toLatin1());
        break;
    case QtCriticalMsg:
        file.write(QString("Critical: ").toLatin1()+msg.toLatin1()+QString(" LogContext: ").toLatin1()+QString(logcontext.function).toLatin1()+QString("\r\n").toLatin1());
        break;
    case QtFatalMsg:
        file.write(QString("Fatal: ").toLatin1()+msg.toLatin1()+QString(" LogContext: ").toLatin1()+QString(logcontext.function).toLatin1()+QString("\r\n").toLatin1());
        abort();
    default:
        file.write(QString("Other: ").toLatin1()+msg.toLatin1()+QString(" LogContext: ").toLatin1()+QString(logcontext.function).toLatin1()+QString("\r\n").toLatin1());
        break;
    }
}

/**
 * @brief main Méthode appelée lors de l'exécution du webshell
 * @param argc  Nombre d'arguments
 * @param argv  Arguments
 * @return app.exec()
 */
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    //Permet de placer dans un fichier .log ce qui est affiché dans la console
    qInstallMessageHandler(myMessageOutput);

    QCommandLineParser parser;
    QCommandLineOption configOption(QStringList() << "f" << "file", "Chemin d'accès au fichier de configuration <confFile>.", "confFile");
    QCommandLineOption urlOption(QStringList() << "u" << "url", "Write generated data into <url>.", "file:///"+QApplication::applicationDirPath()+"/"+"index.html");

    parser.addOption(configOption);

    parser.addOption(urlOption);

    parser.process(app);

    if(parser.isSet(configOption))
    {
        config = new ConfigManager(parser.value(configOption));
    }
    else
    {
        config = new ConfigManager(QApplication::applicationDirPath()+"/appli.xml");
        config->SetBaseUrl(new QStringList());
        config->SetIcon("");
        config->SetInfos("");
        config->SetCloseButtonBehaviour(false);
    }

    QString launch = config->GetLaunchUrl();
    if(parser.isSet(urlOption))
    {
        QString str = parser.value(urlOption);
        //On remplace webshell:// par http:// et webshells:// par https://
        if(str.startsWith("webshell://") || str.startsWith("webshells://"))
        {
            str.replace(0,8,"http");
        }
        //On remplace webshellf:// par file:///
        if(str.startsWith("webshellf://"))
        {
            str.replace(0,12,"file:///");
        }
        QUrl url = QUrl(str);
        //Si les conditions de validation de l'url en paramètre sont remplies, on remplace l'url de démarrage
        if(url.isValid() && !str.endsWith("//"))
        {
            launch = url.url();
        }
    }

    if(launch!=config->GetLaunchUrl())
        config->SetLaunchUrl(launch);

    QString appName;
    if(!config->GetBaseUrl()->isEmpty())
        appName = config->GetBaseUrl()->first();
    else
    {
        config->SetBaseUrl(&(QStringList() << launch));
        appName = launch;
    }

    //On récupère dans appName la partie placée entre ://(/) et le premier / suivant
    int index = appName.indexOf("//");
    if(appName.at(index+2) == '/')
        index = index+3;
    else
        index = index+2;
    appName.remove(0,index);
    index = appName.indexOf("/");
    appName.truncate(index);
    app.setApplicationName(appName);

    app.setApplicationVersion(config->GetVersion());

    //Si l'url n'a pas été choisie à partir des arguments, on prend celle mise au départ
    //Sinon, on prend l'url spécifiée plus tôt
    MainWindow *mw = new MainWindow();

    //Affichage de la page de démarrage
    mw->show();


    return app.exec();
}
