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

    //Setters et getters pour paramètres webshell
    bool GetScreenMode();
    bool GetCloseButtonBehaviour();
    bool GetDeveloperToolsMode();
    void SetScreenMode(bool fullscreen);
    void SetCloseButtonBehaviour(bool minimization);
    void SetDeveloperToolsMode(bool developerToolsActivated);

    //Setters et getters pour paramètres appli
    QString GetLaunchUrl();
    QString GetIcon();
    QList<QString> *GetBaseUrl();
    void SetLaunchUrl(QString launchUrl);
    void SetIcon(QString icon);
    void SetBaseUrl(QList<QString> *baseUrl);

private:
    //Paramètres webshell
    bool fullscreen;
    bool minimization;
    bool developerToolsActivated;

    //Paramètres application
    QString launchUrl;
    QString icon;
    QList<QString> *baseUrl;

    QString confFilePath;

    void LoadParametersWebshell();
    void LoadParametersAppli();
    void InitWebshellParameters();
};

#endif // CONFIGMANAGER_H
