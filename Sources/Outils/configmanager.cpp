#include "configmanager.h"
#include <iostream>
#include <QApplication>
#include <QDesktopWidget>

#define PARENT_TAG_NAME "settings"
#define CHILDREN_TAG_NAME "setting"
#define CHILDREN_NAME_ATTRIBUTE "name"
#define CHILDREN_VALUE_ATTRIBUTE "value"

//Création de l'instance de ConfigManager
ConfigManager ConfigManager::m_instance=ConfigManager();

ConfigManager& ConfigManager::Instance()
{
	return m_instance;
}


/**
 * @brief Construit le gestionnaire de configuration.\n
 * Il ne sera correctement initialisé que lorsque la méthode InitApplicationParameters sera appelée.
 */
ConfigManager::ConfigManager()
{
	this->InitWebshellParameters();
}

/**
 * @brief Met à jour le fichier xml du webshell avec son numéro de version
 */
void ConfigManager::InitWebshellParameters()
{
	//On place les attributs par défaut pouvant être remplacés par les valeurs du fichier xml
	installationFileToRemove = "";
	//L'adresse sauvegardée correspond à l'adresse de la page en cours lors d'une perte de connexion
	//Elle n'a d'intérêt que lors de la session courante, il n'y a donc pas besoin de la stocker dans le fichier xml
	savedAdress= "";

	QDomDocument dom("webshell_xml");
	QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/webshell.xml");
	if(!file.exists())
	{
		StoreParametersWebshell();
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
		QDomElement domElem = dom.documentElement();
		QDomNode n = domElem.firstChild();
		while(!n.isNull())
		{
			QDomElement e = n.toElement();
			if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "installationFileToRemove")
				installationFileToRemove = e.attribute(CHILDREN_VALUE_ATTRIBUTE);

			n = n.nextSibling();
		}
	}
}

/**
 * @brief Initialise les informations de l'application à partir de l'URL de démarrage de l'application
 * @param launchUrl  URL de lancement de l'application
 */
void ConfigManager::InitApplicationParameters(QString launchUrl)
{
	appName = launchUrl;
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
	lang = QLocale::system().uiLanguages().first();
	lang.truncate(2);
	baseUrl = QStringList();

	QDomDocument dom("appli_xml");
	QFile file(confFilePath);
	if(!file.exists())
	{
		StoreParametersAppli();
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
		QDomElement domElem = dom.documentElement();
		QDomNode n = domElem.firstChild();
		while(!n.isNull())
		{
			QDomElement e = n.toElement();
			if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "fullscreen")
				fullscreen = (e.attribute(CHILDREN_VALUE_ATTRIBUTE) == "true" ? true : false);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "developerToolsActivated")
				developerToolsActivated = (e.attribute(CHILDREN_VALUE_ATTRIBUTE) == "true" ? true : false);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "minimization")
				minimization = (e.attribute(CHILDREN_VALUE_ATTRIBUTE) == "true" ? true : false);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "menuBarPresent")
				menuBarPresent = (e.attribute(CHILDREN_VALUE_ATTRIBUTE) == "true" ? true : false);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "minWidth")
				minWidth = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "minHeight")
				minHeight = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "defaultWidth")
				defaultWidth = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "defaultHeight")
				defaultHeight = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

            else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "userWidth")
                userWidth = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

            else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "userHeight")
                userHeight = e.attribute(CHILDREN_VALUE_ATTRIBUTE).toInt();

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "icon")
				icon = e.attribute(CHILDREN_VALUE_ATTRIBUTE);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "infos")
				infosAppli = e.attribute(CHILDREN_VALUE_ATTRIBUTE);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "lang")
				lang = e.attribute(CHILDREN_VALUE_ATTRIBUTE);

			else if(e.attribute(CHILDREN_NAME_ATTRIBUTE) == "baseUrl")
				baseUrl.append(e.attribute(CHILDREN_VALUE_ATTRIBUTE));

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
			StoreParametersAppli();
	}
}

/**
 * @brief Enregistre les paramètres webshell dans le fichier xml associé
 */
void ConfigManager::StoreParametersWebshell()
{
	QDomDocument dom;
	QDomProcessingInstruction header = dom.createProcessingInstruction("xml","version='1.0' encoding='UTF-8'");
	dom.appendChild(header);
	QDomElement domElem = dom.createElement(PARENT_TAG_NAME);
	dom.appendChild(domElem);

	QDomElement write_elem;

	//Insertion du paramètre installationFileToRemove
	write_elem = dom.createElement(CHILDREN_TAG_NAME);
	write_elem.setAttribute(CHILDREN_NAME_ATTRIBUTE, "installationFileToRemove");
	write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, installationFileToRemove);
	domElem.appendChild(write_elem);

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
void ConfigManager::StoreParametersAppli()
{
	QDomDocument dom;
	QDomProcessingInstruction header = dom.createProcessingInstruction("xml","version='1.0' encoding='UTF-8'");
	dom.appendChild(header);
	QDomElement domElem = dom.createElement(PARENT_TAG_NAME);
	dom.appendChild(domElem);

	//Insertion du paramètre fullscreen
	AppendNode("fullscreen",fullscreen,domElem);

	//Insertion du paramètre developerToolsActivated
	AppendNode("developerToolsActivated",developerToolsActivated,domElem);

	//Insertion du paramètre minimization
	AppendNode("minimization",minimization,domElem);

	//Insertion du paramètre menuBarPresent
	AppendNode("menuBarPresent",menuBarPresent,domElem);

	//Insertion du paramètre minWidth
	AppendNode("minWidth",minWidth,domElem);

	//Insertion du paramètre minHeight
	AppendNode("minHeight",minHeight,domElem);

	//Insertion du paramètre defaultWidth
	AppendNode("defaultWidth",defaultWidth,domElem);

	//Insertion du paramètre defaultHeight
	AppendNode("defaultHeight",defaultHeight,domElem);

    //Insertion du paramètre userWidth
    AppendNode("userWidth",userWidth,domElem);

    //Insertion du paramètre userHeight
    AppendNode("userHeight",userHeight,domElem);

	//Insertion du paramètre launchUrl
	AppendNode("launchUrl",launchUrl,domElem);

	//Insertion du paramètre icon
	AppendNode("icon",icon,domElem);

	//Insertion du paramètre infosAppli
	AppendNode("infos",infosAppli,domElem);

	//Insertion du lang lang
	AppendNode("lang",lang,domElem);

	//Insertion du paramètre baseUrl
	AppendNode("baseUrl",baseUrl,domElem);

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
 * @brief Ajoute à l'élément domElem une balise avec les attributs name et value
 * @param name		Nom de la valeur
 * @param value		Valeur
 * @param domElem	Elément auquel ajouter le noeud
 */
void ConfigManager::AppendNode(const QString &name, const QVariant &value, QDomElement &domElem)
{
	//Insertion du paramètre fullscreen
	QDomDocument dom;
	QDomElement write_elem;

	write_elem = dom.createElement(CHILDREN_TAG_NAME);

	QStringList list;
	switch(value.type())
	{
	case QVariant::Bool:
		write_elem.setAttribute(CHILDREN_NAME_ATTRIBUTE, name);
		if(value.toBool()) write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, "true");
		else write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, "false");
		break;
	case QVariant::String:
		write_elem.setAttribute(CHILDREN_NAME_ATTRIBUTE, name);
		write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, value.toString());
		break;
	case QVariant::Int:
		write_elem.setAttribute(CHILDREN_NAME_ATTRIBUTE, name);
		write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, QString::number(value.toInt()));
		break;
	case QVariant::StringList:
		list = value.toStringList();
		for(QStringList::iterator i = list.begin(); i != list.end();++i)
		{
			write_elem = dom.createElement(CHILDREN_TAG_NAME);
			write_elem.setAttribute(CHILDREN_NAME_ATTRIBUTE, name);
			write_elem.setAttribute(CHILDREN_VALUE_ATTRIBUTE, *i);
			domElem.appendChild(write_elem);
		}
		break;
	default:
		break;
	}

	domElem.appendChild(write_elem);
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
	StoreParametersWebshell();
}

/**
 * @brief Indique l'adresse de la page sauvegardée lors d'une perte de connexion
 * @return savedAdress
 */
QString ConfigManager::GetSavedAdress()
{
	return savedAdress;
}

/**
 * @brief Sauvegarde l'adresse en cas de perte de connexion ou place une chaîne vide lors d'une reconnexion
 * @param savedAdress	Si la valeur est non vide, c'est l'adresse à récupérer lorsque la connexion est récupérée
 */
void ConfigManager::SetSavedAdress(QString savedAdress)
{
	this->savedAdress = savedAdress;
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
	StoreParametersAppli();
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
	StoreParametersAppli();
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
	StoreParametersAppli();
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
	StoreParametersAppli();
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
	StoreParametersAppli();
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
 * @brief Renvoie la largeur de l'application personnalisée par l'utilisateur
 * @return defaultWidth
 */
int ConfigManager::GetUserWidth() const
{
    return userWidth;
}

/**
 * @brief Renvoie la hauteur de l'application personnalisée par l'utilisateur
 * @return defaultHeight
 */
int ConfigManager::GetUserHeight() const
{
    return userHeight;
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
	StoreParametersAppli();
	emit defaultSize(this->defaultWidth,this->defaultHeight);
}

/**
 * @brief Met à jour la taille de l'application personnalisée par l'utilisateur\n
 * @param defaultWidth	Nouvelle largeur par défaut
 * @param defaultHeight	Nouvelle hauteur par défaut
 */
void ConfigManager::SetUserSize(int userWidth, int userHeight)
{
    this->userWidth = userWidth;
    this->userHeight = userHeight;
    StoreParametersAppli();
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
	StoreParametersAppli();
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
	StoreParametersAppli();
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
	StoreParametersAppli();
}

/**
 * @brief Indique la langue de l'application
 * @return lang
 */
QString ConfigManager::GetLanguage()
{
	return lang;
}

/**
 * @brief Met à jour la langue de l'applicaton
 * @param lang Nouvelle langue de l'application
 */
void ConfigManager::SetLanguage(QString lang)
{
	this->lang = lang;
	emit newLanguage(lang);
	StoreParametersAppli();
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
	StoreParametersAppli();
}

/**
 * @brief Renvoie le nom de l'application
 * @return appName
 */
QString ConfigManager::GetAppName()
{
	return appName;
}
