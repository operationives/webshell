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

    //Setters et getters pour attributs webshell
    bool GetScreenMode();
    bool GetCloseButtonBehaviour();
    bool GetDeveloperToolsMode();
    QString GetVersion();
    void SetScreenMode(bool fullscreen);
    void SetCloseButtonBehaviour(bool minimization);
    void SetDeveloperToolsMode(bool developerToolsActivated);
    void SetVersion(QString version);

    //Setters et getters pour attributs appli
    QString GetLaunchUrl();
    QString GetIcon();
    QStringList *GetBaseUrl();
    void SetLaunchUrl(QString launchUrl);
    void SetIcon(QString icon);
    void SetBaseUrl(QStringList *baseUrl);

private:
    //Attributs webshell
    bool fullscreen;
    bool minimization;
    bool developerToolsActivated;
    QString version;

    //Attributs application
    QString launchUrl;
    QString icon;
    QStringList *baseUrl;

    QString confFilePath;

    void LoadParametersWebshell();
    void LoadParametersAppli();
    void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
