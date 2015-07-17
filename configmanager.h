#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtXml>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    ConfigManager();
    ~ConfigManager();
    bool GetScreenMode();
    bool GetCloseButtonBehaviour();
    bool GetDeveloperToolsMode();
    void SetScreenMode(bool fullscreen);
    void SetCloseButtonBehaviour(bool minimization);
    void SetDeveloperToolsMode(bool developerToolsActivated);

private:
    bool fullscreen;
    bool minimization;
    bool developerToolsActivated;
    void ChangeParameter(QString parameter, bool value);
};

#endif // CONFIGMANAGER_H
