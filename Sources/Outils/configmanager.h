#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtXml>

//On définit la taille des bordures de l'écran
#ifdef Q_OS_WIN
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#else
//Confirmer la taille des bordures sous mac
#define WINDOW_FRAME_HEIGHT 37
#define WINDOW_FRAME_WIDTH 17
#endif

//Définition des valeurs de langue
#define FR "fr"
#define EN "en"

#define FORCED_REFRESH_TICK_TIMER 100
#define FORCED_REFRESH_DURATION 3000

enum SCREEN_MODE
{
    MAXIMIZED = 0,
    WINDOWED,
    FULLSCREEN
};

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
    SCREEN_MODE GetScreenMode();
	bool GetDeveloperToolsMode();
	bool GetCloseButtonBehaviour();
	bool GetMenuBarPresent();
	int GetMinWidth() const;
	int GetMinHeight() const;
	int GetDefaultWidth() const;
	int GetDefaultHeight() const;
    int GetUserWidth() const;
    int GetUserHeight() const;
	QString GetLaunchUrl();
	QString GetIcon();
	QString GetInfos();
	QString GetLanguage();
	QStringList GetBaseUrl();
    QStringList GetLoginList();
    QString GetLastLogin();
	QString GetAppName();

public slots:
	//Setters pour attributs webshell
	void SetInstallationFileToRemove(QString installationFileToRemove);
	void SetSavedAdress(QString savedAdress);

	//Setters pour attributs appli
    void SetScreenMode(SCREEN_MODE screenMode);
	void SetDeveloperToolsMode(bool developerToolsActivated);
	void SetCloseButtonBehaviour(bool minimization);
	void SetMenuBarPresent(bool menuBarPresent);
	void SetMinSize(int minWidth, int minHeight);
	void SetDefaultSize(int defaultWidth, int defaultHeight);
    void SetUserSize(int userWidth, int userHeight);
	void SetLaunchUrl(QString launchUrl);
	void SetIcon(QString icon);
	void SetInfos(QString infosAppli);
	void SetLanguage(QString lang);
	void SetBaseUrl(QStringList baseUrl);
    void SetLoginList(QStringList loginList);
    void SetLastLogin(QString lastLogin);

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
    SCREEN_MODE screenMode;
	bool developerToolsActivated;
	bool minimization;
	bool menuBarPresent;
	int minWidth;
	int minHeight;
	int defaultWidth;
	int defaultHeight;
    int userWidth;
    int userHeight;
	QString launchUrl;
	QString icon;
	QString infosAppli;
	QString lang;
    QString lastLogin;
	QStringList baseUrl;
    QStringList loginList;

	QString appName;
	QString confFilePath;

	void StoreParametersWebshell();
	void StoreParametersAppli();
	void AppendNode(const QString &name, const QVariant &value, QDomElement &domElem);
	void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
