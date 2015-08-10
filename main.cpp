#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "mainwindow.h"
#include "global.h"

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
	QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + qAppName() + ".log");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	file.write(QString("[").toLatin1()+QDateTime::currentDateTime().toString().toLatin1()+QString("]\t").toLatin1());
	std::string text = msg.toLatin1().constData();
	std::cout << text << "\n";
	std::cout.flush();
	switch (type)
	{
	case QtDebugMsg:
		file.write(QString("Debug:\t\t").toLatin1()+msg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtWarningMsg:
		file.write(QString("Warning:\t").toLatin1()+msg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtCriticalMsg:
		file.write(QString("Critical:\t").toLatin1()+msg.toLatin1()+QString("\r\n").toLatin1());
		break;
	case QtFatalMsg:
		file.write(QString("Fatal:\t\t").toLatin1()+msg.toLatin1()+QString("\r\n").toLatin1());
		abort();
	default:
		file.write(QString("Other:\t\t").toLatin1()+msg.toLatin1()+QString("\r\n").toLatin1());
		break;
	}
}

/**
 * @brief Méthode appelée lors de l'exécution du webshell
 * @param argc  Nombre d'arguments
 * @param argv  Arguments de la forme -u URL -i /chemin/vers/icône.ico
 * @return app.exec()
 */
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName(QString("Webshell"));
	app.setApplicationVersion(QString("0.1.0"));

	//Permet de placer dans un fichier .log ce qui est affiché dans la console
	qInstallMessageHandler(myMessageOutput);

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
		qWarning() << "Pas d'url fournie en paramètre. Fin de l'application.\nExemple d'utilisation: webshell.exe -u http://www.google.fr/";
		return 1;
	}

	QString launch = parser.value(urlOption);
	//On remplace webshell:// par http:// et webshells:// par https://
	if(launch.startsWith("webshell://") || launch.startsWith("webshells://"))
	{
		launch.replace(0,8,"http");
	}
	//On remplace webshellf:// par file:///
	if(launch.startsWith("webshellf://"))
	{
		launch.replace(0,12,"file:///");
	}
	QUrl url = QUrl(launch);
	//Si les conditions de validation de l'url en paramètre sont remplies, on continue l'exécution
	if(!url.isValid() || launch.endsWith("//"))
	{
		qWarning() << "URL invalide: On arrête l'application";
		return 1;
	}

	config = new ConfigManager(launch);

	if(!config->GetInstallationFileToRemove().isEmpty())
	{
		QFile::remove(config->GetInstallationFileToRemove());
		config->SetInstallationFileToRemove("");
	}

	//Si l'icône est spécifié on lance la mainWindow avec le bon icône, sinon on le lance tel quel
	MainWindow *mw;
	if(parser.isSet(iconOption))
		mw = new MainWindow(parser.value(iconOption));
	else
		mw = new MainWindow();

	mw->show();

	return app.exec();
}
