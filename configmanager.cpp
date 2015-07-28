#include "configmanager.h"
#include <iostream>
#include <QApplication>

/**
 * @brief ConfigManager::ConfigManager Initialise les paramètres du webshell
 */
ConfigManager::ConfigManager()
{
	this->InitWebshellParameters();
}

/**
 * @brief ConfigManager::ConfigManager Effectue les actions du constructeur de base, puis sauvegarde les paramètres de l'application présents dans le fichier en paramètre
 * @param confFilePath  Chemin vers le fichier xml de l'application
 */
ConfigManager::ConfigManager(QString confFilePath)
{
	this->InitWebshellParameters();
	this->confFilePath = confFilePath;

	QDomDocument dom("appli_xml");
	QFile file(confFilePath);
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
	baseUrl = new QStringList();
	while(!n.isNull())
	{
		QDomElement e = n.toElement();
		if(e.attribute("name") == "fullscreen")
			fullscreen = (e.attribute("value") == "true" ? true : false);
		else if(e.attribute("name") == "developerToolsActivated")
			developerToolsActivated = (e.attribute("value") == "true" ? true : false);
		else if(e.attribute("name") == "minimization")
			minimization = (e.attribute("value") == "true" ? true : false);
		else if(e.attribute("name") == "launchUrl")
			launchUrl = e.attribute("value");
		else if(e.attribute("name") == "icon")
			icon = e.attribute("value");
		else if(e.attribute("name") == "infos")
			infosAppli = e.attribute("value");
		else if(e.attribute("name") == "baseUrl")
			baseUrl->append(e.attribute("value"));

		n = n.nextSibling();
	}
}

/**
 * @brief ConfigManager::~ConfigManager Destructeur de ConfigManager
 */
ConfigManager::~ConfigManager()
{
	delete baseUrl;
}

/**
 * @brief ConfigManager::InitWebshellParameters Place les paramètres spécifiés dans le fichier xml du dossier courant dans les attributs de la classe
 */
void ConfigManager::InitWebshellParameters()
{
	QDomDocument dom("webshell_xml");
	QFile file(QApplication::applicationDirPath()+"/webshell.xml");
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
		if(e.attribute("name") == "version")
			version = e.attribute("value");

		n = n.nextSibling();
	}
}

/**
 * @brief Xml_Dom::LoadParametersWebshell Enregistre les paramètres webshell dans le fichier xml associé
 */
void ConfigManager::LoadParametersWebshell()
{
	QDomDocument dom("webshell_xml");
	QFile doc_xml(QApplication::applicationDirPath()+"/webshell.xml");
	if(!doc_xml.open(QIODevice::ReadOnly))
	{
		qWarning() << "Open webshell conf File: load error";
		return;
	}
	if(!dom.setContent(&doc_xml))
	{
		qWarning() << "setContent webshell conf File: load error";
		doc_xml.close();
		return;
	}
	QDomElement docElem = dom.documentElement();
	QDomNode n = docElem.firstChild();
	while(!n.isNull())
	{
		QDomNode remove = n;
		n = n.nextSibling();
		docElem.removeChild(remove);
	}

	QDomElement write_elem;

	//Insertion du paramètre version
	write_elem = dom.createElement("setting");
	write_elem.setAttribute("name", "version");
	write_elem.setAttribute("value", version);
	docElem.appendChild(write_elem);

	doc_xml.close();

	QString write_doc = dom.toString();

	QFile fichier(QApplication::applicationDirPath()+"/webshell.xml");
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
 * @brief ConfigManager::LoadParametersAppli Enregistre les paramètres application dans le fichier xml associé
 */
void ConfigManager::LoadParametersAppli()
{
	QDomDocument dom("appli_xml");
	QFile doc_xml(confFilePath);
	if(!doc_xml.open(QIODevice::ReadOnly))
	{
		qWarning() << "Open appli conf File: load error";
		return;
	}
	if(!dom.setContent(&doc_xml))
	{
		qWarning() << "setContent appli conf File: load error";
		doc_xml.close();
		return;
	}
	QDomElement docElem = dom.documentElement();
	QDomNode n = docElem.firstChild();
	while(!n.isNull())
	{
		QDomNode remove = n;
		n = n.nextSibling();
		docElem.removeChild(remove);
	}

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
	for(i = baseUrl->begin(); i != baseUrl->end();++i)
	{
		write_elem = dom.createElement("setting");
		write_elem.setAttribute("name", "baseUrl");
		write_elem.setAttribute("value", *i);
		docElem.appendChild(write_elem);
	}

	doc_xml.close();

	QString write_doc = dom.toString();

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
 * @brief ConfigManager::GetVersion Indique la version du webshell
 * @return version
 */
QString ConfigManager::GetVersion()
{
	return version;
}

/**
 * @brief ConfigManager::SetVersion Met à jour la version du webshell
 * @param version   Nouveau numéro de version du webshell
 */
void ConfigManager::SetVersion(QString version)
{
	this->version = version;
	LoadParametersWebshell();
}

/**
 * @brief ConfigManager::Getscreenmode Indique si l'utilisateur doit être en plein écran ou non
 * @return fullscreen
 */
bool ConfigManager::GetScreenMode()
{
	return fullscreen;
}

/**
 * @brief ConfigManager::SetScreenMode Met à jour le paramètre "fullscreen"
 * @param fullscreen	Nouvelle valeur de this->fullscreen
 */
void ConfigManager::SetScreenMode(bool fullscreen)
{
	this->fullscreen = fullscreen;
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetDeveloperToolsMode Indique si l'utilisateur a accès aux outils développeur
 * @return developerToolsActivated
 */
bool ConfigManager::GetDeveloperToolsMode()
{
	return developerToolsActivated;
}

/**
 * @brief ConfigManager::SetDeveloperToolsMode Met à jour le paramètre "developerToolsActivated"
 * @param developerToolsActivated Nouvelle valeur de this->developerToolsActivated
 */
void ConfigManager::SetDeveloperToolsMode(bool developerToolsActivated)
{
	this->developerToolsActivated = developerToolsActivated;
	emit toolsMode(developerToolsActivated);
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetCloseButtonBehaviour Indique le comportement du bouton de fermeture
 * @return minimization
 */
bool ConfigManager::GetCloseButtonBehaviour()
{
	return minimization;
}

/**
 * @brief ConfigManager::SetCloseButtonBehaviour Met à jour le paramètre "minimization"
 * @param minimization  Nouvelle valeur de this->minimization
 */
void ConfigManager::SetCloseButtonBehaviour(bool minimization)
{
	this->minimization = minimization;
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetLaunchUrl Indique l'adresse de démarrage de l'application
 * @return  launchUrl
 */
QString ConfigManager::GetLaunchUrl()
{
	return launchUrl;
}

/**
 * @brief ConfigManager::SetLaunchUrl Met à jour launchUrl
 * @param launchUrl Nouvelle valeur de this->launchUrl
 */
void ConfigManager::SetLaunchUrl(QString launchUrl)
{
	this->launchUrl = launchUrl;
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetIcon Indique l'url de l'icône par défaut de l'application
 * @return icon
 */
QString ConfigManager::GetIcon()
{
	return icon;
}

/**
 * @brief ConfigManager::SetIcon Met à jour icon
 * @param icon Nouvelle valeur de this->icon
 */
void ConfigManager::SetIcon(QString icon)
{
	this->icon = icon;
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetIcon Indique les infos du service
 * @return infosAppli
 */
QString ConfigManager::GetInfos()
{
	return infosAppli;
}

/**
 * @brief ConfigManager::SetInfos Met à jour les infos service
 * @param infosAppli Nouvelle valeur des infos service
 */
void ConfigManager::SetInfos(QString infosAppli)
{
	this->infosAppli = infosAppli;
	LoadParametersAppli();
}

/**
 * @brief ConfigManager::GetBaseUrl Indique baseUrl associé à l'application
 * @return new QStringList(*baseUrl)
 */
QStringList *ConfigManager::GetBaseUrl()
{
	return new QStringList(*baseUrl);
}

/**
 * @brief ConfigManager::SetBaseUrl Met à jour baseUrl
 * @param baseUrl Nouvelle valeur de this->baseUrl
 */
void ConfigManager::SetBaseUrl(QStringList *baseUrl)
{
	delete this->baseUrl;
	this->baseUrl = new QStringList(*baseUrl);
	LoadParametersAppli();
}
