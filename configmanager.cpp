#include "configmanager.h"
#include <iostream>
#include <QApplication>
#include <QDesktopWidget>

//On définit la taille des bordures de l'écran
#ifdef Q_OS_WIN
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#else
//Confirmer la taille des bordures sous mac
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#endif

/**
 * @brief Initialise les informations du webshell et de l'application à partir de l'URL de démarrage de l'application
 * @param launchUrl  URL de lancement de l'application
 */
ConfigManager::ConfigManager(QString launchUrl)
{
	this->InitWebshellParameters();

	QString appName(launchUrl);
	if(appName.startsWith("http"))
	{
		//On récupère dans appName la partie placée entre :// et le premier / suivant
		int index = appName.indexOf("//");
		index = index+2;
		appName.remove(0,index);
		index = appName.indexOf("/");
		if(index!=-1)
			appName.truncate(index);
	}
	else
	{
		//Cas file:/// -> On retire l'entête, et on récupère le nom du dossier contenant le fichier html
		int index = appName.indexOf("///");
		index = index+3;
		appName.remove(0,index);
		index = appName.lastIndexOf("/");
		if(index==-1)
		{
			qWarning() << "Nom de fichier incorrect";
			return;
		}
		appName.truncate(index);
		QDir localDir(appName);
		appName = localDir.dirName();
	}

	confFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+appName+".xml";
	this->launchUrl = launchUrl;

	//On place les attributs par défaut pouvant être remplacés par les valeurs du fichier xml
	fullscreen = false;
	developerToolsActivated = false;
	minimization = false;
	menuBarPresent = false;
	QRect rec = QApplication::desktop()->availableGeometry();
	minWidth = rec.width()/2;
	minHeight = rec.height()/2;
	defaultWidth = 3*rec.width()/4;
	defaultHeight = 3*rec.height()/4;
	icon = "";
	infosAppli = "";
	baseUrl = QStringList();

	QDomDocument dom("appli_xml");
	QFile file(confFilePath);
	if(!file.exists())
	{
		LoadParametersAppli();
	}
	else
	{
		if (!file.open(QIODevice::ReadOnly))
		{
			qWarning() << "Open appli conf File: constructor error";
			return;
		}
		if (!dom.setContent(&file))
		{
			qWarning() << "setContent appli conf File: constructor error";
			file.close();
			return;
		}

		file.close();
		QDomElement docElem = dom.documentElement();
		QDomNode n = docElem.firstChild();
		while(!n.isNull())
		{
			QDomElement e = n.toElement();
			if(e.attribute("name") == "fullscreen")
				fullscreen = (e.attribute("value") == "true" ? true : false);
			else if(e.attribute("name") == "developerToolsActivated")
				developerToolsActivated = (e.attribute("value") == "true" ? true : false);
			else if(e.attribute("name") == "minimization")
				minimization = (e.attribute("value") == "true" ? true : false);
			else if(e.attribute("name") == "menuBarPresent")
				menuBarPresent = (e.attribute("value") == "true" ? true : false);
			else if(e.attribute("name") == "minWidth")
				minWidth = e.attribute("value").toInt();
			else if(e.attribute("name") == "minHeight")
				minHeight = e.attribute("value").toInt();
			else if(e.attribute("name") == "defaultWidth")
				defaultWidth = e.attribute("value").toInt();
			else if(e.attribute("name") == "defaultHeight")
				defaultHeight = e.attribute("value").toInt();
			else if(e.attribute("name") == "icon")
				icon = e.attribute("value");
			else if(e.attribute("name") == "infos")
				infosAppli = e.attribute("value");
			else if(e.attribute("name") == "baseUrl")
				baseUrl.append(e.attribute("value"));

			n = n.nextSibling();
		}
		bool toLoad = false;
		if((3*rec.width()/4)<minWidth)
		{
			toLoad = true;
			this->minWidth = 3*rec.width()/4;
		}
		if((3*rec.height()/4)<minHeight)
		{
			toLoad = true;
			this->minHeight = 3*rec.height()/4;
		}
		if((rec.width() - WINDOW_FRAME_WIDTH)<defaultWidth)
		{
			toLoad = true;
			this->defaultWidth = rec.width() - WINDOW_FRAME_WIDTH;
		}
		if((rec.height() - WINDOW_FRAME_HEIGHT)<defaultHeight)
		{
			toLoad = true;
			this->defaultHeight = rec.height() - WINDOW_FRAME_HEIGHT;
		}
		if(toLoad)
			LoadParametersAppli();
	}
}

ConfigManager::~ConfigManager()
{

}

/**
 * @brief Met à jour le fichier xml du webshell avec son numéro de version
 */
void ConfigManager::InitWebshellParameters()
{
	//On place les attributs par défaut pouvant être remplacés par les valeurs du fichier xml
	installationFileToRemove = "";

	QDomDocument dom("webshell_xml");
	QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/webshell.xml");
	if(!file.exists())
	{
		LoadParametersWebshell();
	}
	else
	{
		if (!file.open(QIODevice::ReadOnly))
		{
			qWarning() << "Open webshell conf File: constructor error";
			return;
		}
		if (!dom.setContent(&file))
		{
			qWarning() << "setContent webshell conf File: constructor error";
			file.close();
			return;
		}

		file.close();
		QDomElement docElem = dom.documentElement();
		QDomNode n = docElem.firstChild();
		while(!n.isNull())
		{
			QDomElement e = n.toElement();
			if(e.attribute("name") == "installationFileToRemove")
				installationFileToRemove = e.attribute("value");

			n = n.nextSibling();
		}
	}
}

/**
 * @brief Enregistre les paramètres webshell dans le fichier xml associé
 */
void ConfigManager::LoadParametersWebshell()
{
	QDomDocument dom;
	QDomProcessingInstruction header = dom.createProcessingInstruction("xml","version='1.0' encoding='UTF-8'");
	dom.appendChild(header);
	QDomElement docElem = dom.createElement("settings");
	dom.appendChild(docElem);

	QDomElement write_elem;

	//Insertion du paramètre installationFileToRemove
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "installationFileToRemove");
	write_elem.setAttribute("value", installationFileToRemove);
	docElem.appendChild(write_elem);

	QString write_doc = dom.toString();

	QFile fichier(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/webshell.xml");
	if(!fichier.open(QIODevice::WriteOnly))
	{
		fichier.close();
		qDebug() << "Impossible d'écrire dans le document XML";
		return;
	}
	QTextStream stream(&fichier);
	stream.setCodec(QTextCodec::codecForName("UTF-8"));
	stream << write_doc; // On utilise l'opérateur << pour écrire write_doc dans le document XML.
}

/**
 * @brief Enregistre les paramètres application dans le fichier xml associé
 */
void ConfigManager::LoadParametersAppli()
{
	QDomDocument dom;
	QDomProcessingInstruction header = dom.createProcessingInstruction("xml","version='1.0' encoding='UTF-8'");
	dom.appendChild(header);
	QDomElement docElem = dom.createElement("settings");
	dom.appendChild(docElem);

	QDomElement write_elem;

	//Insertion du paramètre fullscreen
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "fullscreen");
	if(fullscreen) write_elem.setAttribute("value", "true");
	else write_elem.setAttribute("value", "false");
	docElem.appendChild(write_elem);

	//Insertion du paramètre developerToolsActivated
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "developerToolsActivated");
	if(developerToolsActivated) write_elem.setAttribute("value", "true");
	else write_elem.setAttribute("value", "false");
	docElem.appendChild(write_elem);

	//Insertion du paramètre minimization
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "minimization");
	if(minimization) write_elem.setAttribute("value", "true");
	else write_elem.setAttribute("value", "false");
	docElem.appendChild(write_elem);

	//Insertion du paramètre menuBarPresent
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "menuBarPresent");
	if(menuBarPresent) write_elem.setAttribute("value", "true");
	else write_elem.setAttribute("value", "false");
	docElem.appendChild(write_elem);

	//Insertion du paramètre minWidth
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "minWidth");
	write_elem.setAttribute("value", QString::number(minWidth));
	docElem.appendChild(write_elem);

	//Insertion du paramètre minHeight
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "minHeight");
	write_elem.setAttribute("value", QString::number(minHeight));
	docElem.appendChild(write_elem);

	//Insertion du paramètre defaultWidth
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "defaultWidth");
	write_elem.setAttribute("value", QString::number(defaultWidth));
	docElem.appendChild(write_elem);

	//Insertion du paramètre defaultHeight
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "defaultHeight");
	write_elem.setAttribute("value", QString::number(defaultHeight));
	docElem.appendChild(write_elem);

	//Insertion du paramètre launchUrl
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "launchUrl");
	write_elem.setAttribute("value", launchUrl);
	docElem.appendChild(write_elem);

	//Insertion du paramètre icon
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "icon");
	write_elem.setAttribute("value", icon);
	docElem.appendChild(write_elem);

	//Insertion du paramètre infosAppli
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "infos");
	write_elem.setAttribute("value", infosAppli);
	docElem.appendChild(write_elem);

	//Insertion du paramètre baseUrl
	QStringList::iterator i;
	for(i = baseUrl.begin(); i != baseUrl.end();++i)
	{
		write_elem = dom.createElement("setting");
		write_elem.setAttribute("name", "baseUrl");
		write_elem.setAttribute("value", *i);
		docElem.appendChild(write_elem);
	}

	QString write_doc;
	write_doc.append(dom.toString());

	QFile fichier(confFilePath);
	if(!fichier.open(QIODevice::WriteOnly))
	{
		fichier.close();
		qDebug() << "Impossible d'écrire dans le document XML";
		return;
	}
	QTextStream stream(&fichier);
	stream.setCodec(QTextCodec::codecForName("UTF-8"));
	stream << write_doc; // On utilise l'opérateur << pour écrire write_doc dans le document XML.
}

/**
 * @brief Indique si il existe le fichier d'installation du webshell à supprimer
 * @return installationFileToRemove
 */
QString ConfigManager::GetInstallationFileToRemove()
{
	return installationFileToRemove;
}

/**
 * @brief Met à jour le fichier d'installation du webshell à supprimer
 * @param installationFileToRemove	Si la valeur est non vide, c'est un fichier à supprimer au redémarrage
 */
void ConfigManager::SetInstallationFileToRemove(QString installationFileToRemove)
{
	this->installationFileToRemove = installationFileToRemove;
	LoadParametersWebshell();
}

/**
 * @brief Indique si l'utilisateur doit être en plein écran ou non
 * @return fullscreen
 */
bool ConfigManager::GetScreenMode()
{
	return fullscreen;
}

/**
 * @brief Met à jour le paramètre "fullscreen"
 * @param fullscreen	Nouvelle valeur de this->fullscreen
 */
void ConfigManager::SetScreenMode(bool fullscreen)
{
	this->fullscreen = fullscreen;
	LoadParametersAppli();
}

/**
 * @brief Indique si l'utilisateur a accès aux outils développeur
 * @return developerToolsActivated
 */
bool ConfigManager::GetDeveloperToolsMode()
{
	return developerToolsActivated;
}

/**
 * @brief Met à jour le paramètre "developerToolsActivated"
 * @param developerToolsActivated Nouvelle valeur de this->developerToolsActivated
 */
void ConfigManager::SetDeveloperToolsMode(bool developerToolsActivated)
{
	this->developerToolsActivated = developerToolsActivated;
	emit toolsMode(developerToolsActivated);
	LoadParametersAppli();
}

/**
 * @brief Indique le comportement du bouton de fermeture
 * @return minimization
 */
bool ConfigManager::GetCloseButtonBehaviour()
{
	return minimization;
}

/**
 * @brief Met à jour le paramètre "minimization"
 * @param minimization  Nouvelle valeur de this->minimization
 */
void ConfigManager::SetCloseButtonBehaviour(bool minimization)
{
	this->minimization = minimization;
	LoadParametersAppli();
}

/**
 * @brief Indique si la barre de menu est présente
 * @return menuBarPresent
 */
bool ConfigManager::GetMenuBarPresent()
{
	return menuBarPresent;
}

/**
 * @brief Met à jour le paramètre "menuBarPresent"
 * @param menuBarPresent  Nouvelle valeur de this->menuBarPresent
 */
void ConfigManager::SetMenuBarPresent(bool menuBarPresent)
{
	this->menuBarPresent = menuBarPresent;
	emit menuBarPresence(menuBarPresent);
	LoadParametersAppli();
}

/**
 * @brief Renvoie la largeur minimale de l'application
 * @return minWidth
 */
int ConfigManager::GetMinWidth() const
{
	return minWidth;
}

/**
 * @brief Renvoie la hauteur minimale de l'application
 * @return minHeight
 */
int ConfigManager::GetMinHeight() const
{
	return minHeight;
}

/**
 * @brief Met à jour la taille minimale de l'application\n
 * Si les dimensions indiquées sont supérieurs aux 3/4 de celles de l'écran, elles seront remplacées par ce seuil
 * @param minWidth	Nouvelle largeur minimale
 * @param minHeight	Nouvelle hauteur minimale
 */
void ConfigManager::SetMinSize(int minWidth, int minHeight)
{
	QRect rec = QApplication::desktop()->availableGeometry();
	if((3*rec.width()/4)<minWidth)
		this->minWidth = 3*rec.width()/4;
	else
		this->minWidth = minWidth;
	if((3*rec.height()/4)<minHeight)
		this->minHeight = 3*rec.height()/4;
	else
		this->minHeight = minHeight;
	LoadParametersAppli();
	emit minSize(this->minWidth,this->minHeight);
}

/**
 * @brief Renvoie la largeur par défaut de l'application
 * @return defaultWidth
 */
int ConfigManager::GetDefaultWidth() const
{
	return defaultWidth;
}

/**
 * @brief Renvoie la hauteur par défaut de l'application
 * @return defaultHeight
 */
int ConfigManager::GetDefaultHeight() const
{
	return defaultHeight;
}

/**
 * @brief Met à jour la taille par défaut de l'application\n
 * Si les dimensions indiquées sont supérieurs à celles de l'écran, elles seront remplacées par ce seuil
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void ConfigManager::SetDefaultSize(int defaultWidth, int defaultHeight)
{
	QRect rec = QApplication::desktop()->availableGeometry();
	if((rec.width() - WINDOW_FRAME_WIDTH)<defaultWidth)
		this->defaultWidth = rec.width() - WINDOW_FRAME_WIDTH;
	else
		this->defaultWidth = defaultWidth;
	if((rec.height() - WINDOW_FRAME_HEIGHT)<defaultHeight)
		this->defaultHeight = rec.height() - WINDOW_FRAME_HEIGHT;
	else
		this->defaultHeight = defaultHeight;
	LoadParametersAppli();
	emit defaultSize(this->defaultWidth,this->defaultHeight);
}


/**
 * @brief Indique l'adresse de démarrage de l'application
 * @return  launchUrl
 */
QString ConfigManager::GetLaunchUrl()
{
	return launchUrl;
}

/**
 * @brief Met à jour launchUrl
 * @param launchUrl Nouvelle valeur de this->launchUrl
 */
void ConfigManager::SetLaunchUrl(QString launchUrl)
{
	this->launchUrl = launchUrl;
	LoadParametersAppli();
}

/**
 * @brief Indique l'url de l'icône par défaut de l'application
 * @return icon
 */
QString ConfigManager::GetIcon()
{
	return icon;
}

/**
 * @brief Met à jour icon
 * @param icon Nouvelle valeur de this->icon
 */
void ConfigManager::SetIcon(QString icon)
{
	this->icon = icon;
	LoadParametersAppli();
}

/**
 * @brief Indique les infos du service
 * @return infosAppli
 */
QString ConfigManager::GetInfos()
{
	return infosAppli;
}

/**
 * @brief Met à jour les infos service
 * @param infosAppli Nouvelle valeur des infos service
 */
void ConfigManager::SetInfos(QString infosAppli)
{
	this->infosAppli = infosAppli;
	LoadParametersAppli();
}

/**
 * @brief Indique baseUrl associé à l'application
 * @return QStringList(baseUrl)
 */
QStringList ConfigManager::GetBaseUrl()
{
	return QStringList(baseUrl);
}

/**
 * @brief Met à jour baseUrl
 * @param baseUrl Nouvelle valeur de this->baseUrl
 */
void ConfigManager::SetBaseUrl(QStringList baseUrl)
{
	this->baseUrl = QStringList(baseUrl);
	LoadParametersAppli();
}
