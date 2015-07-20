#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "mainwindow.h"

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
    app.setApplicationName(QString("fr.dev.djanah.webshell"));
    app.setApplicationVersion(QString("1.0"));

//    QUrl launch = QUrl("http://djanah.dev.ives.fr");
//    QUrl launch = QUrl("http://djanah.dev.ives.fr/VideoLiveAPI/inst_plugin.php?retour=http%253A//djanah.dev.ives.fr/client/menu.php%253Fl%253Dfr%2526PHPSESSID%253Db2ui2vusdc0nug99befbkjmjv1");
    QUrl launch = QUrl("file:///"+QApplication::applicationDirPath()+"/"+"index.html");
    if(argc==2)
    {
        QString str;
        str = argv[1];
        //On remplace webshell:// par http:// et webshells:// par https://
        if(str.startsWith("webshell://") || str.startsWith("webshells://"))
        {
            str.replace(0,8,"http");
        }
        QUrl url = QUrl(str);
        //Si les conditions de validation de l'url en paramètre sont remplies, on remplace l'url de démarrage
        if(url.isValid() && !str.endsWith("//"))
        {
            launch = url;
        }
    }


    //Permet de placer dans un fichier .log ce qui est affiché dans la console
    qInstallMessageHandler(myMessageOutput);

    //Si l'url n'a pas été choisie à partir des arguments, on prend celle mise au départ
    //Sinon, on prend l'url spécifiée plus tôt
    MainWindow *mw = new MainWindow(launch);

    //Affichage de la page de démarrage
    mw->show();


    return app.exec();
}
