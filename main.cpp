#include <QString>
#include "global.h"


void myMessageOutput(QtMsgType type, const QMessageLogContext & logcontext,const QString & msg)  {
    QFile file(QApplication::applicationDirPath()+"/"+qAppName()+".log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    file.write("["+QDateTime::currentDateTime().toString()+"] ");
    switch (type) {
    case QtDebugMsg:
        file.write("Debug: "+msg+" LogContext: "+logcontext.function+", "+logcontext.line+"\r\n");
        break;
    case QtWarningMsg:
        file.write("Warning: "+msg+" LogContext: "+logcontext.function+", "+logcontext.line+"\r\n");
        break;
    default:
        file.write("Other: "+msg+" LogContext: "+logcontext.function+", "+logcontext.line+"\r\n");
        break;
    }
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName(QString("fr.dev.djanah.webshell"));
    app.setApplicationVersion(QString("1.0"));

//    QUrl launch = QUrl("http://djanah.dev.ives.fr");
//    QUrl launch = QUrl("http://djanah.dev.ives.fr/VideoLiveAPI/inst_plugin.php?retour=http%253A//djanah.dev.ives.fr/client/menu.php%253Fl%253Dfr%2526PHPSESSID%253Db2ui2vusdc0nug99befbkjmjv1");
    QUrl launch = QUrl("file:///c:/Users/phvdev64/Documents/Test/index.html");
    if(argc==2){
        QString str;
        str = argv[1];
        //On remplace webshell:// par http:// et webshells:// par https://
        if(str.startsWith("webshell://") || str.startsWith("webshells://")){
            str.replace(0,8,"http");
        }
        QUrl url = QUrl(str);
        //Si les conditions de validation de l'url en paramètre sont remplies, on remplace l'url de démarrage
        if(url.isValid() && !str.endsWith("//")){
            launch = url;
        }
    }

    if(!QCoreApplication::arguments().contains("dvp"))
            qInstallMessageHandler(myMessageOutput);

    //Si l'url n'a pas été choisie à partir des arguments, on prend celle mise au départ
    //Sinon, on prend l'url spécifiée plus tôt
    MainWindow *mw = new MainWindow(launch);

    //Affichage de la page de démarrage
    mw->show();

//        QWidget fenetre;
//        fenetre.setMinimumSize(1000,1000);
//        QLabel *label = new QLabel(&fenetre);
//        //label->setPixmap(mw.loadImage());

//        QVBoxLayout layout;
//        fenetre.setLayout(&layout);
//        layout.addWidget(label);
//        fenetre.show();

    return app.exec();
}
