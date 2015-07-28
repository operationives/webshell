#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtXml>

class ConfigManager : public QObject
{
	Q_OBJECT

public:
	ConfigManager();
	ConfigManager(QString confFilePath);
	~ConfigManager();

	//Getters pour attributs webshell
	QString GetVersion();

	//Getters pour attributs appli
	bool GetScreenMode();
	bool GetDeveloperToolsMode();
	bool GetCloseButtonBehaviour();
	QString GetLaunchUrl();
	QString GetIcon();
	QString GetInfos();
	QStringList *GetBaseUrl();

public slots:
	//Setters pour attributs webshell
	void SetVersion(QString version);

	//Setters pour attributs appli
	void SetScreenMode(bool fullscreen);
	void SetDeveloperToolsMode(bool developerToolsActivated);
	void SetCloseButtonBehaviour(bool minimization);
	void SetLaunchUrl(QString launchUrl);
	void SetIcon(QString icon);
	void SetInfos(QString infosAppli);
	void SetBaseUrl(QStringList *baseUrl);

signals:
	void toolsMode(bool toolsActivated);

private:
	//Attributs webshell
	bool fullscreen;
	bool minimization;
	bool developerToolsActivated;
	QString version;

	//Attributs application
	QString launchUrl;
	QString icon;
	QString infosAppli;
	QStringList *baseUrl;

	QString confFilePath;

	void LoadParametersWebshell();
	void LoadParametersAppli();
	void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
