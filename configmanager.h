#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtXml>

class ConfigManager : public QObject
{
	Q_OBJECT

public:
	ConfigManager(QString launchUrl);
	~ConfigManager();

	//Getters pour attributs webshell
	QString GetVersion();

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
	QStringList GetBaseUrl();

public slots:
	//Setters pour attributs webshell
	void SetVersion(QString version);

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
	void SetBaseUrl(QStringList baseUrl);

signals:
	void toolsMode(bool toolsActivated);
	void menuBarPresence(bool menuBarPresent);
	void minSize(int minWidth, int minHeight);
	void defaultSize(int defaultWidth, int defaultHeight);

private:
	//Attributs webshell
	QString version;

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
	QStringList baseUrl;

	QString confFilePath;

	void LoadParametersWebshell();
	void LoadParametersAppli();
	void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
