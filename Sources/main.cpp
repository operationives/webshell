#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "Widgets/mainwindow.h"
#include "Outils/configmanager.h"
#include "Widgets/singleapplication.h"
#ifdef Q_OS_WIN
    #include <QtWinExtras>
#endif


class UrlDialog;

/** @mainpage Webshell
 *
 * Le webshell est un logiciel permettant d'ouvrir des applications web HTML5 de façon à ce qu'elles réagissent de la même manière que si elles étaient dans un navigateur web supportant les NPAPI Plugins. Le webshell contenant une application spécifique peut s'exécuter de manière similaire à une application de bureau standard.
 *
 * Le webshell pourra communiquer avec ces applications à l'aide d'une API JavaScript dédiée. Cela permet d'offrir plus de possibilités pour le développement de l'application ouverte par le webshell.
 * Le webshell disposera par ailleurs de plusieurs outils natifs indépendants de l'application utilisée.
 *
 * Le but de ce projet est de proposer un support aux applications web HTML5 ayant des fonctionnalités plus adaptées qu'un navigateur standard. Cela permettra ainsi de proposer un support sous la forme d'une application bureau, plus accueillante que les étapes à effectuer à travers le navigateur pour accéder aux services de l'application.
 */


//A faire: gestion de la taille maximale du fichier .log
/**
 * @brief Gestion des sorties consoles à placer dans les logs
 * @param type			Catégorie de message
 * @param logcontext	Contexte (vide?)
 * @param msg			Contenu du message
 */
void myMessageOutput(QtMsgType type, const QMessageLogContext & logcontext,const QString & msg)
{
	Q_UNUSED(logcontext);
    QString filteredMsg= "";
    filteredMsg.append(msg);
	QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + qAppName() + ".log");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	file.write(QString("[").toLatin1()+QDateTime::currentDateTime().toString().toLatin1()+QString("]\t").toLatin1());

    // hash info deletion:

    // Remove hash infos in logs messages
    // Case 1: www.domain.com/index.php?hash=xxx => www.domain.com/index.php
    // Case 2: www.domain.com/index.php?hash=xxx&variable1=xxx => www.domain.com/index.php?variable1=xxx
    // Case 3: www.domain.com/index.php?variable1=xxx&hash=xxx => www.domain.com/index.php?variable1=xxx
    // Case 4: www.domain.com/index.php?variable1=xxx&hash=xxx&variable2=xxx => www.domain.com/index.php?variable1=xxx&variable2=xxx

    int hash_end    = -1;
    int hash_start  = -1;

    hash_start  = msg.indexOf("?hash");

    if (hash_start != -1) // cases 1 and 2
    {
        hash_end = msg.indexOf("&",hash_start);
        if (hash_end == -1) // case 1: deletes ?hash=xxx
        {
            hash_end = msg.length();
            filteredMsg.remove(hash_start,hash_end-hash_start);
        }
        else // case 2: deletes hash=xxx&
        {
            filteredMsg.remove(hash_start+1,hash_end-hash_start); // excludes '?' character
        }
    }
    else if ((hash_start = msg.indexOf("&hash")) != -1) // cases 3 and 4
    {
        hash_end = msg.indexOf("\&",hash_start+1);

        if (hash_end == -1) // case 3: deletes &hash=xxx in end of url
        {
            hash_end = msg.length();
            filteredMsg.remove(hash_start,hash_end-hash_start);
        }
        else // case 4: deletes &hash=xxx
        {
            filteredMsg.remove(hash_start,hash_end-hash_start);
        }
    }
    else
    {
        // Nothing to do
    }

    std::string text = filteredMsg.toLatin1().constData();
	std::cout << text << "\n";
	std::cout.flush();

	switch (type)
	{       
	case QtDebugMsg:
        file.write(QString("Debug:\t\t").toLatin1()+filteredMsg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtWarningMsg:
        file.write(QString("Warning:\t").toLatin1()+filteredMsg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtCriticalMsg:
        file.write(QString("Critical:\t").toLatin1()+filteredMsg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtFatalMsg:
        file.write(QString("Fatal:\t\t").toLatin1()+filteredMsg.toLatin1()+QString("\r\n").toLatin1());
		abort();
	default:
        file.write(QString("Other:\t\t").toLatin1()+filteredMsg.toLatin1()+QString("\r\n").toLatin1());
		break;
	}
}

void askLaunchUrl(QString & launchUrl)
{
    QEventLoop loop;
    UrlDialog dialog;
    QDialogButtonBox url_window(QDialogButtonBox::Save | QDialogButtonBox::Abort);
    QVBoxLayout layout;
    QLabel Label;
    QLineEdit lineEdit;
    QString os_language = QLocale::system().uiLanguages().first();

    url_window.setGeometry(0, 0, 400, 20);
    layout.addWidget(&Label);
    layout.addWidget(&lineEdit);
    layout.addWidget(&url_window);

    //Qt::WindowFlags flags = url_window->windowFlags();
    //url_window->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

    dialog.setLayout(&layout);
    dialog.show();
    dialog.setMinimumSize(400,80);

    os_language.truncate(2);
    if (os_language == FR)
    {
        dialog.setWindowTitle("Configuration de l'URL du service");
        Label.setText("Entrez l'URL d'accès au service : ");
    }
    else
    {
        dialog.setWindowTitle("Service URL Configuration");
        Label.setText("Enter the service URL: ");
    }

    QObject::connect(&url_window,SIGNAL(accepted()),&loop,SLOT(quit()));
    QObject::connect(&url_window,SIGNAL(rejected()),&loop,SLOT(quit()));
    QObject::connect(&dialog,SIGNAL(dialogClosed()),&loop,SLOT(quit()));
    loop.exec();

    launchUrl = lineEdit.text();
}

/**
 * @brief Méthode appelée lors de l'exécution du webshell
 * @param argc  Nombre d'arguments
 * @param argv  Arguments de la forme -u URL -i /chemin/vers/icône.ico
 * @return app.exec()
 */
int main(int argc, char** argv)
{
    QString launch_url;
    QString encoded_launch_url;

    SimpleCrypt crypto(KEY_CODE);
    SingleApplication app(argc, argv);
    app.setApplicationName(QString("Webshell"));
    app.setApplicationVersion(QString("1.0.7"));
    QSettings settings;
    ConfigManager &config = ConfigManager::Instance();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

	//Permet de placer dans un fichier .log ce qui est affiché dans la console
	qInstallMessageHandler(myMessageOutput);

    qDebug() << "------------------------------------------";
    qDebug() << QCoreApplication::applicationName() << " version "<< QCoreApplication::applicationVersion() << " is starting";
    qDebug() << "Build date: " << __DATE__ << " at " << __TIME__;

	QDir appdata(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	if(!appdata.exists())
	{
		appdata.mkpath(appdata.path());
	}

	QCommandLineParser parser;
	//Compléter la gestion de l'option Icône
	QCommandLineOption iconOption(QStringList() << "i" << "icon", "Chemin d'accès vers l'icône de l'application <confFile>.", "Icône");
	QCommandLineOption urlOption(QStringList() << "u" << "url", "Write generated data into <url>.", "Url");

	parser.addOption(iconOption);
	parser.addOption(urlOption);
	parser.process(app);

    if(!parser.isSet(urlOption))
    {
#ifdef POINT_URL
        if ((launch_url = QString(POINT_URL)).isEmpty() == false)
        {
            qDebug() << "Url point defined by preprocessor variable: " << launch_url;
        }
#else

        /*if ((launch_url = config.GetLaunchUrl()).isEmpty() == false)
        {
             qDebug() << "Launch url retrieved in configuration file: " << launch_url;
        }
        else */if (QString(settings.value("config/point").toString()).isEmpty())
        {
            askLaunchUrl(launch_url);
            qDebug() << "Url point defined: " << launch_url;

            //encoded_launch_url = crypto.encryptToString(launch_url);
            //settings.setValue("config/point", encoded_launch_url);
            //settings.setValue("config/point", launch_url);
        }
        else
            launch_url = crypto.decryptToString(settings.value("config/point").toString());
            //launch_url = settings.value("config/point").toString();
#endif
    }
    else
    {
        launch_url = parser.value(urlOption);
    }

    //On remplace webshell:// par http:// et webshells:// par https://
    if(launch_url.startsWith("webshell://") || launch_url.startsWith("webshells://"))
    {
        launch_url.replace(0,8,"http");
    }
    //On remplace webshellf:// par file:///
    if(launch_url.startsWith("webshellf://"))
    {
        launch_url.replace(0,12,"file:///");
    }
    QUrl url = QUrl(launch_url);
    //Si les conditions de validation de l'url en paramètre sont remplies, on continue l'exécution

    if(!url.isValid() || launch_url.endsWith("//"))
    {
        qWarning() << "URL invalide: On arrête l'application";
        //askLaunchUrl(launch_url);
        return 1;
    }
    else
    {
        encoded_launch_url = crypto.encryptToString(launch_url);
        settings.setValue("config/point", encoded_launch_url);
    }

    config.InitApplicationParameters(launch_url);

    if(!config.GetInstallationFileToRemove().isEmpty())
    {
        QFile::remove(config.GetInstallationFileToRemove());
        config.SetInstallationFileToRemove("");
    }

    //QWebSecurityOrigin::addLocalScheme("about");

	//Si l'icône est spécifié on lance la mainWindow avec le bon icône, sinon on le lance tel quel
	MainWindow *mw;
	if(parser.isSet(iconOption))
		mw = new MainWindow(parser.value(iconOption));
	else
		mw = new MainWindow();

    QObject::connect(QApplication::instance(), SIGNAL(showUp()), mw, SLOT(raise()));

    mw->show();

	return app.exec();
}
