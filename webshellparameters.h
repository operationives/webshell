#ifndef DEVELOPERTOOLSACTIVATED_H
#define DEVELOPERTOOLSACTIVATED_H

#include <QObject>

class WebshellParameters : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool developerToolsActivated READ DeveloperToolsActivated WRITE SetDeveloperToolsActivated)
	Q_PROPERTY(bool minimization READ Minimization WRITE SetMinimization)
	Q_PROPERTY(QString target READ Target WRITE SetTarget)
public:
	WebshellParameters();


private:
	bool m_developerToolsActivated;
	bool m_minimization;
	QString m_target;
	bool DeveloperToolsActivated() const;
	void SetDeveloperToolsActivated(bool developerToolsActivated);
	bool Minimization() const;
	void SetMinimization(bool minimization);
	QString Target() const;
	void SetTarget(const QString &target);

};

#endif // DEVELOPERTOOLSACTIVATED_H

