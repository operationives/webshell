#ifndef DEVELOPERTOOLSACTIVATED_H
#define DEVELOPERTOOLSACTIVATED_H

#include <QObject>

class WebshellParameters : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool developerToolsActivated READ DeveloperToolsActivated WRITE SetDeveloperToolsActivated)
	Q_PROPERTY(bool minimization READ Minimization WRITE SetMinimization)
	Q_PROPERTY(bool menuBarPresent READ MenuBarPresent WRITE SetMenuBarPresent)
	Q_PROPERTY(QString target READ Target WRITE SetTarget)
	Q_PROPERTY(QString lang READ Lang WRITE SetLang)
public:
	WebshellParameters();


private:
	QString m_target;
	bool DeveloperToolsActivated() const;
	void SetDeveloperToolsActivated(bool developerToolsActivated);
	bool Minimization() const;
	void SetMinimization(bool minimization);
	bool MenuBarPresent() const;
	void SetMenuBarPresent(bool menuBarPresent);
	QString Target() const;
	void SetTarget(const QString &target);
	QString Lang() const;
	void SetLang(const QString &lang);
};

#endif // DEVELOPERTOOLSACTIVATED_H

