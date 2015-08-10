#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QStringList>

//On définit la taille des bordures de l'écran
#ifdef Q_OS_WIN
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#else
//Confirmer la taille des bordures sous mac
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#endif

class ConfigManager : public QObject
{
	Q_OBJECT

public:
	static ConfigManager& Instance();
	void InitApplicationParameters(QString launchUrl);

	//Getters pour attributs webshell
	QString GetInstallationFileToRemove();
	QString GetSavedAdress();

	//Getters pour attributs appli
	bool GetScreenMode();
	bool GetDeveloperToolsMode();
	bool GetCloseButtonBehaviour();
	bool GetMenuBarPresent();
	int GetMinWidth() const;
	int GetMinHeight() const;
	int GetDefaultWidth() const;
	int GetDefaultHeight() const;
	QString GetLaunchUrl();
	QString GetIcon();
	QString GetInfos();
	QString GetLanguage();
	QStringList GetBaseUrl();

public slots:
	//Setters pour attributs webshell
	void SetInstallationFileToRemove(QString installationFileToRemove);
	void SetSavedAdress(QString savedAdress);

	//Setters pour attributs appli
	void SetScreenMode(bool fullscreen);
	void SetDeveloperToolsMode(bool developerToolsActivated);
	void SetCloseButtonBehaviour(bool minimization);
	void SetMenuBarPresent(bool menuBarPresent);
	void SetMinSize(int minWidth, int minHeight);
	void SetDefaultSize(int defaultWidth, int defaultHeight);
	void SetLaunchUrl(QString launchUrl);
	void SetIcon(QString icon);
	void SetInfos(QString infosAppli);
	void SetLanguage(QString lang);
	void SetBaseUrl(QStringList baseUrl);

signals:
	void toolsMode(bool toolsActivated);
	void menuBarPresence(bool menuBarPresent);
	void minSize(int minWidth, int minHeight);
	void defaultSize(int defaultWidth, int defaultHeight);
	void newLanguage(QString lang);

private:
	ConfigManager& operator= (const ConfigManager&){}
	ConfigManager (const ConfigManager&){}

	static ConfigManager m_instance;
	ConfigManager();
	~ConfigManager(){}

	//Attributs webshell
	QString installationFileToRemove;
	QString savedAdress;

	//Attributs application
	bool fullscreen;
	bool developerToolsActivated;
	bool minimization;
	bool menuBarPresent;
	int minWidth;
	int minHeight;
	int defaultWidth;
	int defaultHeight;
	QString launchUrl;
	QString icon;
	QString infosAppli;
	QString lang;
	QStringList baseUrl;

	QString confFilePath;

	void LoadParametersWebshell();
	void LoadParametersAppli();
	void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
