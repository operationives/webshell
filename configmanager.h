#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtXml>

class ConfigManager : public QObject{
    Q_OBJECT

public:
    ConfigManager();
    ~ConfigManager();
    bool getScreenMode();
    bool getCloseButtonBehaviour();
    bool getDeveloperToolsMode();
    void setScreenMode(bool fullscreen);
    void setCloseButtonBehaviour(bool minimization);
    void setDeveloperToolsMode(bool developerToolsActivated);

private:
    bool fullscreen;
    bool minimization;
    bool developerToolsActivated;
    void changeParameter(QString parameter, bool value);
};

#endif // CONFIGMANAGER_H
